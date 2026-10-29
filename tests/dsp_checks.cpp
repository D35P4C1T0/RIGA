#include "Engine.h"
#include "impulse_fixture.h"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <random>
#include <vector>

namespace {
int failures = 0;
void check(bool condition, const char *message) {
    if (!condition) {
        std::fprintf(stderr, "FAIL: %s\n", message);
        ++failures;
    }
}
std::vector<float> render(double rate, int block, const strip::Parameters &p) {
    strip::Engine engine;
    engine.prepare(rate, block);
    engine.setParameters(p);
    engine.setNoiseSeed(1);
    std::vector<float> result(8192), left(block), right(block);
    for (int offset = 0; offset < 4096; offset += block) {
        const int count = std::min(block, 4096 - offset);
        for (int i = 0; i < count; ++i) {
            left[i] = .3f * std::sin(float(offset + i) * .193f);
            right[i] = .2f * std::cos(float(offset + i) * .071f);
        }
        engine.process(left.data(), right.data(), nullptr, nullptr, count);
        for (int i = 0; i < count; ++i) {
            result[2 * (offset + i)] = left[i];
            result[2 * (offset + i) + 1] = right[i];
        }
    }
    return result;
}
} // namespace
int main() {
    for (float noise : {0.0f, .6f}) {
        strip::Engine engine;
        engine.prepare(48000, 256);
        auto parameters = strip::defaults();
        parameters[strip::index(strip::Param::noise)] = noise;
        engine.setParameters(parameters);
        engine.setNoiseSeed(1);
        std::vector<float> left(4096), right(4096);
        engine.process(left.data(), right.data(), nullptr, nullptr, 4096);
        const bool silent = std::all_of(left.begin(), left.end(), [](float x) { return x == 0; }) &&
                            std::all_of(right.begin(), right.end(), [](float x) { return x == 0; });
        check(silent == (noise == 0), "noise off must be silent and noise on must generate audio");
    }
    {
        strip::Engine engine;
        engine.prepare(48000, 256);
        engine.setNoiseSeed(1);
        std::vector<float> left(16384), right(16384);
        left[0] = right[0] = .5f;
        for (int i = 0; i < 16384; i += 256)
            engine.process(left.data() + i, right.data() + i, nullptr, nullptr, 256);
        float worst = 0;
        for (const auto &p : strip::fixtures::impulse)
            worst = std::max({worst, std::abs(left[p.frame] - p.left), std::abs(right[p.frame] - p.right)});
        check(worst < 2e-7f, "deterministic impulse regression mismatch");
        std::printf("Impulse regression checkpoints: maximum absolute error %.9g\n", worst);
    }
    {
        strip::Engine mono;
        mono.prepare(48000, 256, 1);
        mono.setNoiseSeed(1);
        std::vector<float> left(16384), right(16384);
        left[0] = right[0] = .5f;
        for (int i = 0; i < 16384; i += 256)
            mono.process(left.data() + i, right.data() + i, nullptr, nullptr, 256);
        float worst = 0;
        for (const auto &p : strip::fixtures::monoImpulse)
            worst = std::max(worst, std::abs(left[p.frame] - p.left));
        check(worst < 2e-7f, "mono impulse regression mismatch");
    }
    for (int mode = 1; mode <= 2; ++mode) {
        auto p = strip::defaults();
        auto stereo = render(48000, 256, p);
        p[strip::index(strip::Param::monitor)] = float(mode) / 2;
        const auto monitored = render(48000, 256, p);
        float worst = 0;
        for (int i = 0; i < 4096; ++i) {
            const float expected = stereo[2 * i] * .7071067690849304f +
                                   stereo[2 * i + 1] * (mode == 1 ? .7071067690849304f : -.7071067690849304f);
            worst = std::max(
                {worst, std::abs(monitored[2 * i] - expected), std::abs(monitored[2 * i + 1] - expected)});
        }
        check(worst < 1e-6f, "monitor matrix has incorrect gain or polarity");
    }
    std::mt19937 rng(17);
    std::uniform_real_distribution<float> uniform(0, 1);
    for (double rate : {22050., 44100., 48000., 96000., 192000.}) {
        const auto defaults = strip::defaults();
        check(render(rate, 1, defaults) == render(rate, 257, defaults), "block size changes audio");
        auto bypass = defaults;
        bypass[strip::index(strip::Param::bypass)] = 1;
        const auto dry = render(rate, 127, bypass);
        for (int i = 0; i < 4096; ++i) {
            if (dry[2 * i] != .3f * std::sin(float(i) * .193f) ||
                dry[2 * i + 1] != .2f * std::cos(float(i) * .071f)) {
                check(false, "bypass changes input");
                break;
            }
        }
        for (int trial = 0; trial < 40; ++trial) {
            auto p = defaults;
            for (std::size_t i = 0; i < p.size(); ++i) {
                const auto steps = strip::parameterSpecs[i].steps;
                p[i] = steps > 0 ? std::round(uniform(rng) * float(steps)) / float(steps) : uniform(rng);
            }
            p[strip::index(strip::Param::power)] = 1;
            p[strip::index(strip::Param::bypass)] = 0;
            p[strip::index(strip::Param::mute)] = 0;
            const auto audio = render(rate, 127, p);
            check(std::all_of(audio.begin(), audio.end(), [](float x) { return std::isfinite(x); }),
                  "non-finite audio at valid settings");
        }
    }
    if (!failures)
        std::puts("PASS: deterministic block processing, transparent bypass, 200 randomized configurations "
                  "at five sample rates");
    return failures ? 1 : 0;
}
