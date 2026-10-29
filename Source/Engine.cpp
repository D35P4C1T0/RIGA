#include "Engine.h"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstring>

namespace strip {
namespace {
float reciprocalMagnitude(float value) noexcept {
    const float square = value * value;
    std::uint32_t bits;
    std::memcpy(&bits, &square, sizeof(bits));
    bits = 0x5f3759dfu - (bits >> 1);
    float estimate;
    std::memcpy(&estimate, &bits, sizeof(estimate));
    estimate = (1.5f - square * 0.5f * estimate * estimate) * estimate;
    return value < 0 ? -estimate : estimate;
}
} // namespace

float Engine::DcSection::process(float input) noexcept {
    const float residual = input - first * c[4];
    const float core = residual - second * c[2];
    const float outer = residual * c[4] + first;
    first = core * c[2] + second;
    second = core;
    return outer * c[3] + first * c[1] + core * c[0];
}

void Engine::prepare(double sampleRate, int maximumBlockSize, int channels) {
    rate = std::isfinite(sampleRate) && sampleRate >= 8000 ? sampleRate : 48000;
    channelCount = channels == 1 ? 1 : 2;
    tone.prepare(rate);
    dynamics.prepare(rate, channelCount);
    hasProcessed = false;
    controls.initialized = false;
    const int block = std::max(1, maximumBlockSize);
    const int desired = std::max(1, static_cast<int>(rate * .05 + .5));
    controlLength = ((desired + block - 1) / block) * block;
    if (desired < block) {
        controlLength = desired;
        int previous = block;
        for (int divisor = 2; divisor < block; ++divisor)
            if (block % divisor == 0) {
                const int candidate = block / divisor;
                if (candidate < desired) {
                    controlLength = previous;
                    break;
                }
                previous = candidate;
            }
    }
    controlPosition = 0;
    initialized = false;
    setParameters(values);
    reset();
}

void Engine::reset() {
    tone.reset();
    dynamics.reset();
    for (auto &section : dc)
        section.first = section.second = 0;
    const auto ticks = std::chrono::duration_cast<std::chrono::milliseconds>(
                           std::chrono::steady_clock::now().time_since_epoch())
                           .count();
    setNoiseSeed(static_cast<std::uint32_t>(ticks) | 1u);
    meter = {};
    controlPosition = 0;
}

void Engine::setParameters(const Parameters &parameters) {
    Parameters next = parameters;
    for (auto &v : next)
        v = std::isfinite(v) ? std::clamp(v, 0.0f, 1.0f) : 0.0f;
    if (initialized && next == values)
        return;
    values = next;
    initialized = true;
    tone.setParameters(values, !hasProcessed);
    dynamics.setParameters(values, !hasProcessed);
    const double corner = values[index(Param::eqType)] >= .5f ? 4.4 : 5.6;
    const double pi = 3.14159265358979323846, w2 = corner * corner * 39.47841760435743;
    const double asymptote = rate * pi / std::sqrt(rate * rate * pi * pi + w2),
                 square = asymptote * asymptote;
    const double fourth = square * square * 0.5000000000000001;
    double pole = std::tan(std::sqrt(w2 / (1 - fourth)) * square * 0.7071067811865476 * (.5 / rate));
    pole *= std::sqrt(square - fourth) / (square * 0.7071067811865476);
    const double b0 = asymptote / (1 + pole), b1 = -b0, reflection = (pole - 1) / (pole + 1);
    for (auto &section : dc)
        section.c = {static_cast<float>(b0 - b1 * reflection), static_cast<float>(b1),
                     static_cast<float>(reflection), 0, 0};
    inputGain = static_cast<float>(
        std::pow(10.0, physical(Param::inputGain, values[index(Param::inputGain)]) * 0.05));
    outputGain = static_cast<float>(
        std::pow(10.0, physical(Param::outputGain, values[index(Param::outputGain)]) * 0.05));
    const double noiseDb = physical(Param::noise, values[index(Param::noise)]);
    noise = static_cast<float>(std::clamp(std::pow(10.0, noiseDb * 0.05), 0.0, 0.00031622776601683794));
    const double amplitude = std::pow(10.0, physical(Param::drive, values[index(Param::drive)]) * 0.05);
    constexpr double scale = 1.0001643499350519;
    cubicLinear = static_cast<float>(1.0 + 3.0 * amplitude);
    cubicThird = static_cast<float>(-4.0 * amplitude * scale * scale);
    cubicLimit =
        static_cast<float>(std::sqrt(-(1.0 + 3.0 * amplitude) / (-12.0 * amplitude * scale * scale)));
    const std::array<Param, 4> gains{Param::highGain, Param::highMidGain, Param::lowGain, Param::lowMidGain};
    for (int i = 0; i < 4; ++i) {
        normalizer[static_cast<std::size_t>(i)] =
            i == 0
                ? 1.0f
                : static_cast<float>(std::pow(
                      10.0,
                      std::max(0.0, (double(values[index(gains[static_cast<std::size_t>(i)])]) - .5) * 2.5 -
                                        .75)));
        inverseNormalizer[static_cast<std::size_t>(i)] =
            i == 0 ? 1.0f : reciprocalMagnitude(normalizer[static_cast<std::size_t>(i)]);
    }
    if (values[index(Param::drive)] <= 1e-14f) {
        cubicLinear = 1;
        cubicThird = 0;
        cubicLimit = 1e6f;
        normalizer.fill(1);
        inverseNormalizer.fill(1);
    }
    std::array<float, 26> target{inputGain,
                                 outputGain,
                                 noise,
                                 values[index(Param::power)],
                                 values[index(Param::mute)],
                                 values[index(Param::eqOn)],
                                 cubicLinear,
                                 cubicThird,
                                 cubicLimit};
    std::copy(normalizer.begin(), normalizer.end(), target.begin() + 9);
    std::copy(inverseNormalizer.begin(), inverseNormalizer.end(), target.begin() + 13);
    std::copy(dc[0].c.begin(), dc[0].c.end(), target.begin() + 17);
    const int monitor = static_cast<int>(std::floor(values[index(Param::monitor)] * 2 + .5f));
    constexpr float matrix = .7071067811865475f;
    if (monitor == 0) {
        target[22] = target[25] = 1;
    } else {
        target[22] = target[24] = matrix;
        target[23] = target[25] = monitor == 1 ? matrix : -matrix;
    }
    if (!hasProcessed)
        controls.initialized = false;
    controls.set(target);
}

void Engine::randomPair(float &left, float &right) noexcept {
    const auto sample = [this]() {
        randomState *= 16807u;
        std::int32_t signedState;
        std::memcpy(&signedState, &randomState, sizeof(signedState));
        return static_cast<float>(signedState) * 4.656613e-10f;
    };
    left = sample();
    right = channelCount == 2 ? sample() : left;
}

float Engine::equalize(float input, int channel) noexcept {
    const float dry = input;
    const auto &sm = controls.current;
    const bool driven = sm[7] != 0;
    for (int stage = 0; stage < 4; ++stage) {
        input = tone.processEqStage(input, channel, stage);
        if (driven) {
            const auto i = static_cast<std::size_t>(stage);
            const float limited = std::clamp(input * sm[9 + i], -sm[8], sm[8]);
            const float cube = limited * limited * limited;
            input = (sm[6] * limited + sm[7] * cube) * sm[13 + i];
        }
    }
    return sm[5] == 1 ? input : sm[5] == 0 ? dry : input * sm[5] + dry * (1 - sm[5]);
}

void Engine::process(float *left, float *right, const float *keyLeft, const float *keyRight,
                     int count) noexcept {
    if (!left || !right || count <= 0)
        return;
    hasProcessed = true;
    meter = {};
    const bool filtersInKey = values[index(Param::filterPosition)] >= .5f;
    const int eqPosition = static_cast<int>(std::floor(values[index(Param::eqPosition)] * 2 + .5f));
    const bool external = values[index(Param::externalKey)] >= .5f;
    const bool invert = values[index(Param::phase)] >= .5f;
    for (int i = 0; i < count; ++i) {
        const float dryL = left[i], dryR = right[i];
        meter.inputL = std::max(meter.inputL, std::abs(dryL));
        meter.inputR = std::max(meter.inputR, std::abs(dryR));
        if (values[index(Param::bypass)] >= .5f) {
            meter.outputL = std::max(meter.outputL, std::abs(dryL));
            meter.outputR = std::max(meter.outputR, std::abs(dryR));
            continue;
        }
        if (controlPosition == 0) {
            const float reciprocal = 1.0f / static_cast<float>(controlLength);
            controls.begin(reciprocal);
            tone.beginControlCycle(reciprocal);
            dynamics.beginControlCycle(reciprocal);
        }
        controls.advance();
        tone.advanceControls();
        dynamics.advanceControls();
        controlPosition = (controlPosition + 1) % controlLength;
        const auto &sm = controls.current;
        for (auto &section : dc)
            std::copy_n(sm.begin() + 17, 5, section.c.begin());
        float nl, nr;
        randomPair(nl, nr);
        float l = dc[0].process(sm[0] * dryL + sm[2] * nl);
        float r = dc[1].process(sm[0] * dryR + sm[2] * nr);
        if (!filtersInKey) {
            l = tone.processFilters(l, 0);
            r = tone.processFilters(r, 1);
        }
        if (eqPosition == 0) {
            l = equalize(l, 0);
            r = equalize(r, 1);
        }
        float kl = external ? (keyLeft ? keyLeft[i] : 0) : l;
        float kr = external ? (keyRight ? keyRight[i] : kl) : r;
        if (filtersInKey) {
            kl = tone.processFilters(kl, 0);
            kr = tone.processFilters(kr, 1);
        }
        if (eqPosition == 1) {
            kl = equalize(kl, 0);
            kr = equalize(kr, 1);
        }
        dynamics.process(l, r, kl, kr);
        if (eqPosition == 2) {
            l = equalize(l, 0);
            r = equalize(r, 1);
        }
        l *= sm[1];
        r *= sm[1];
        if (invert) {
            l = -l;
            r = -r;
        }
        if (channelCount == 2) {
            const float a = l, b = r;
            l = b * sm[23] + a * sm[22];
            r = b * sm[25] + a * sm[24];
        }
        l *= 1 - sm[4];
        r *= 1 - sm[4];
        if (sm[3] != 1) {
            l = l * sm[3] + dryL * (1 - sm[3]);
            r = r * sm[3] + dryR * (1 - sm[3]);
        }
        left[i] = l;
        right[i] = r;
        meter.outputL = std::max(meter.outputL, std::abs(l));
        meter.outputR = std::max(meter.outputR, std::abs(r));
        meter.compression = std::max(meter.compression, dynamics.compressionDb());
        meter.gate = std::max(meter.gate, dynamics.gateDb());
    }
}
} // namespace strip
