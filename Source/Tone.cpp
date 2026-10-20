#include "Tone.h"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstring>
namespace strip {
namespace {
constexpr double pi = 3.14159265358979323846;
using Coeff = std::array<double, 5>;
constexpr std::array<double, 23> taper{{0.0,  0.01, 0.02,  0.043, 0.068, 0.092, 0.115, 0.135,
                                        0.16, 0.18, 0.205, 0.23,  0.25,  0.275, 0.36,  0.46,
                                        0.55, 0.64, 0.72,  0.81,  0.9,   0.96,  1.0}};
constexpr std::array<double, 23> reverseTaper{{0.0,  0.01, 0.04, 0.1, 0.14, 0.19, 0.23, 0.28,
                                               0.34, 0.4,  0.45, 0.5, 0.55, 0.6,  0.66, 0.72,
                                               0.77, 0.81, 0.86, 0.9, 0.96, 0.99, 1.0}};
constexpr std::array<double, 11> hpCurve{{1.0, 0.93, 0.72, 0.46, 0.19, 0.08, 0.07, 0.05, 0.03, 0.01, 0.0}};
constexpr std::array<double, 21> hpQ{{1.46, 1.46, 1.46, 1.46, 1.46, 1.26, 1.26, 1.02, 1.02, 0.3, 0.76,
                                      0.76, 0.76, 0.76, 0.76, 0.87, 0.87, 1.11, 1.11, 1.57, 1.57}};
constexpr std::array<double, 5> hpLevel{{0.0, 0.0, 0.0, -0.1, -0.4}};
constexpr std::array<double, 11> highFrequency{
    {1.0, 0.86, 0.69, 0.46, 0.22, 0.13, 0.09, 0.07, 0.06, 0.04, 0.03}};
constexpr std::array<double, 6> highGainA{{0.05, 0.25, 0.35, 0.47, 0.7, 1.0}};
constexpr std::array<double, 6> highGainB{{0.5, 0.55, 0.55, 0.55, 0.75, 0.96}};
constexpr std::array<double, 11> hmFrequency{
    {1.0, 0.9, 0.68, 0.47, 0.24, 0.11, 0.08, 0.06, 0.04, 0.02, 0.008}};
constexpr std::array<double, 6> hmGainA{{0.05, 0.35, 0.47, 0.58, 0.79, 1.0}};
constexpr std::array<double, 6> hmGainB{{0.4, 0.6, 0.57, 0.57, 0.83, 1.0}};
constexpr std::array<double, 11> lowFrequency{
    {0.0, 0.009, 0.027, 0.042, 0.059, 0.08, 0.219, 0.39, 0.565, 0.724, 0.783}};
constexpr std::array<double, 11> lowPot{{0.0, 0.18, 0.34, 0.4, 0.44, 0.5, 0.56, 0.6, 0.66, 0.79, 1.0}};
constexpr std::array<double, 6> lowGainA{{0.5, 0.76, 1.0, 1.0, 1.0, 1.1}};
constexpr std::array<double, 6> lowGainB{{1.0, 1.0, 1.0, 1.0, 1.0, 1.05}};
constexpr std::array<double, 11> lmFrequencyA{
    {0.93, 0.84, 0.67, 0.46, 0.25, 0.11, 0.08, 0.05, 0.03, 0.015, 0.005}};
constexpr std::array<double, 11> lmFrequencyB{
    {0.92, 0.8, 0.61, 0.46, 0.24, 0.12, 0.1, 0.09, 0.06, 0.04, 0.03}};
constexpr std::array<double, 11> lmPot{{0.0, 0.02, 0.3, 0.4, 0.46, 0.5, 0.52, 0.56, 0.62, 0.74, 1.0}};
constexpr std::array<double, 6> lmGainA{{1.0, 1.0, 1.0, 1.0, 0.86, 1.0}};
constexpr std::array<double, 6> lmGainB{{0.8, 0.82, 0.7, 0.7, 1.0, 1.1}};
constexpr std::array<double, 6> hmCorrectionTopA{{0.0, 0.0, 0.0, -2.8, -6.2, -11.0}};
constexpr std::array<double, 6> hmCorrectionBottomA{{0.0, 0.0, 0.0, 0.0, -3.3, -8.1}};
constexpr std::array<double, 6> hmCorrectionTopB{{0.0, -4.8, -7.4, -9.3, -12.9, -16.5}};
constexpr std::array<double, 6> hmCorrectionBottomB{{0.0, -4.3, -6.8, -8.7, -12.1, -16.0}};
constexpr std::array<double, 6> lmCorrectionTopA{{0.0, -2.8, -4.0, -5.1, -7.5, -13.0}};
constexpr std::array<double, 6> lmCorrectionBottomA{{0.0, 0.0, 0.0, 0.0, -1.5, -6.0}};
constexpr std::array<double, 6> lmCorrectionTopB{{0.0, -5.3, -6.9, -8.6, -11.1, -16.1}};
constexpr std::array<double, 6> lmCorrectionBottomB{{0.0, -5.0, -5.7, -8.1, -11.4, -15.3}};
constexpr std::array<double, 11> highCorrectionB{
    {1.0, 1.0, 0.98, 0.97, 0.97, 0.95, 0.94, 0.94, 0.92, 0.9, 0.88}};
template <std::size_t N> double interp(const std::array<double, N> &t, double v) {
    const double x = std::clamp(v, 0.0, 1.0) * double(N - 1);
    const auto i = static_cast<std::size_t>(x);
    const auto j = std::min(i + 1, N - 1);
    return t[i] + (t[j] - t[i]) * (x - double(i));
}
double frequencyPot(double x, double middle) {
    return x < 0.5 ? 2 * x * middle : middle + 2 * (x - 0.5) * (1 - middle);
}
double component(double value, int channel, bool capacitor = false) {
    std::uint64_t bits;
    std::memcpy(&bits, &value, sizeof(bits));
    std::uint32_t state = (std::uint32_t(bits) ^ std::uint32_t(bits >> 32)) | 1u;
    constexpr std::uint32_t multiplier[] = {0x41a7u,     0x10d63af1u, 0x60b7a437u, 0xb87e16e1u,
                                            0x5e100dc7u, 0x24960e57u, 0x6ff883d1u, 0xdd3d35e7u,
                                            0xf78771c1u, 0xdd9dd0b1u, 0xe6cf90a1u, 0xa3f01477u};
    double sum = 0;
    for (int j = 0; j < channel; ++j) {
        sum = 0;
        for (auto m : multiplier) {
            std::uint32_t u = state * m;
            std::int32_t s;
            std::memcpy(&s, &u, 4);
            sum += double(s);
        }
        state *= 0xe6cf90a1u;
    }
    return value + value * (capacitor ? 10.0 : 1.5) * sum * 4.656612873077393e-10 * 0.005;
}
Coeff normalized(double b0, double b1, double b2, double a0, double a1, double a2) {
    return {b0 / a0, b1 / a0, a1 / a0, b2 / a0, a2 / a0};
}
Coeff reciprocal(Coeff c) {
    return {1 / c[0], c[2] / c[0], c[1] / c[0], c[4] / c[0], c[3] / c[0]};
}
Coeff scale(Coeff c, double g) {
    c[0] *= g;
    c[1] *= g;
    c[3] *= g;
    return c;
}
Coeff addDry(Coeff c, double g) {
    c[0] += g;
    c[1] += g * c[2];
    c[3] += g * c[4];
    return c;
}
// A frequency-matched second-order resonator. Matching at two frequencies
// preserves the analog curve close to the sampling limit.
Coeff resonator(double fs, double frequency, double q, double gain) {
    const double w = 2 * pi * frequency, w2 = w * w, q2 = q * q;
    const double probe = std::min(w, fs * 3.1337386719558187);
    const double p2 = probe * probe;
    const double boost = std::abs(gain) + 1;
    const double g2 = boost * boost;
    const double constant = frequency * frequency * frequency * frequency * q2 * 1558.5454565440389;
    const double den = p2 * p2 * q2 - 2 * p2 * q2 * w2 + constant;
    const double matching = (p2 * w2 * g2 + den) / (p2 * w2 + den);
    const double root = std::sqrt(matching);
    const double k = std::tan(probe * (0.5 / fs));
    const double d = (2 * root * q2 + g2 - root - 2 * q2) -
                     std::sqrt(((4 * root * q2 - root - 4 * q2) + g2) * (g2 - root));
    const double warped = std::tan(std::sqrt(2 * d / (root - 1)) * w * (0.5 / q) * (0.5 / fs));
    const double fs2 = fs * fs;
    const double analogAtNyq =
        (fs2 * fs2 * q2 * 97.40909103400243 - fs2 * w2 * 2 * q2 * 9.869604401089358) + constant;
    const double numerator = fs2 * 9.869604401089358 * w2;
    const double nyquist = std::min((root - 1) * 0.999999 + 1,
                                    std::sqrt((numerator * g2 + analogAtNyq) / (numerator + analogAtNyq)));
    const double diff = matching - nyquist * nyquist;
    const double z = std::sqrt(diff / (matching - 1)) * k * k;
    const double u = warped * warped;
    const double correction = 2 * z * (matching - nyquist - std::sqrt((matching - 1) * diff));
    const double v = u * nyquist - z;
    const double t = (((u - z) * (u - z) * root - v * v) + u * correction) / ((matching - root) * u);
    const double a = std::sqrt(t), b = std::sqrt(t * matching - correction);
    return normalized(nyquist + z + b, -2 * (nyquist - z), nyquist - b + z, 1 + z + a, -2 * (1 - z),
                      1 + z - a);
}
Coeff bandPass(double fs, double f, double q, double g) {
    auto c = resonator(fs, f, q, g);
    c = addDry(c, -1);
    return scale(c, g < 0 ? -1 : 1);
}
Coeff highPass1(double fs, double f, double level) {
    const double f2 = f * f * 39.47841760435743;
    const double a = fs * pi / std::sqrt(fs * fs * pi * pi + f2), a2 = a * a;
    const double a4 = a2 * a2 * 0.5000000000000001;
    double pole = std::tan(std::sqrt(f2 / (1 - a4)) * a2 * 0.7071067811865476 * (0.5 / fs));
    pole = pole * std::sqrt(a2 - a4) / (a2 * 0.7071067811865476);
    return normalized(a * level, -a * level, 0, 1 + pole, pole - 1, 0);
}
Coeff highPass2(double fs, double f, double q, double level) {
    const double angle = std::min(2 * pi * f, fs * pi * 0.65), k = std::tan(angle * (0.5 / fs));
    const double f2 = f * f, fs2 = fs * fs;
    const double term = f2 * 1558.5454565440389 * q * f2 * q;
    const double b = (fs2 * 9.869604401089358 * q * level) /
                     std::sqrt(fs2 * f2 * 389.6363641360097 - q * fs2 * f2 * q * 779.2727282720194 + term +
                               q * q * 97.40909103400243 * fs2 * fs2);
    const double angle2 = angle * angle;
    const double damp = 1 / ((q * level * angle2) / (std::sqrt(angle2 * f2 * 39.47841760435743 -
                                                               q * q * f2 * angle2 * 78.95683520871486 +
                                                               angle2 * q * angle2 * q + term) *
                                                     b));
    return normalized(b, -2 * b, b, (damp + k) * k + 1, 2 * (k * k - 1), k * k - damp * k + 1);
}
Coeff lowPass2(double fs, double f, double q, double level) {
    const double w = 2 * pi * f, q2 = q * q;
    const double aa = 2 - fs * 0.5000000000000001 / f / f * fs;
    double g = 2 / std::sqrt((fs / (f * q)) * (fs / (f * q)) + aa * aa);
    if (std::abs(g - 1) < 1e-6)
        g = g < 1 ? 0.999999 : 1.000001;
    double k;
    if (q <= 0.7071077811865476) {
        const double r = std::sqrt((std::sqrt((1 - 4 * q2) / (q2 * q2) + 4 / g) + 2 - 1 / q2) * 0.5);
        k = std::min(std::tan(r * w * (0.5 / fs)), std::sqrt(std::sqrt(1 - g * g)) * w * (0.5 / fs));
    } else {
        const double probe = std::min(std::sqrt(1 - 0.5 / q2) * w, fs * 3.1258846903218442);
        const double dd = 1 - (probe / w) * (probe / w), qq = probe / (w * q);
        const double peak = std::max(1 / std::sqrt(qq * qq + dd * dd), g * 1.000001);
        k = std::sqrt(std::sqrt((peak * peak - g * g) / (peak * peak - 1))) *
            std::clamp(std::abs(std::tan(probe * (0.5 / fs))), 1e-6, 1e6);
    }
    const double rg = std::sqrt(g), t1 = 2 * std::atan(k) * fs, t2 = 2 * std::atan(k / rg) * fs;
    const auto analog = [&](double t) {
        double d = 1 - (t / w) * (t / w), v = t / (w * q);
        return 1 / std::sqrt(v * v + d * d);
    };
    const double x = analog(t1) * analog(t1), y = analog(t2) * analog(t2), dg = (g - 1) * (g - 1);
    const double a = std::sqrt((x - y) * g * g / ((x + g) * y * dg));
    const double b = std::sqrt((x - y) * g / ((y + g) * dg));
    const double h = k * k * a, j = a * g;
    return normalized((h + rg * k + j) * b * level, 2 * b * level * (h - j), (h - rg * k + j) * b * level,
                      (k + k * k * b + b) * a, 2 * a * (k * k * b - b), (k * k * b - k + b) * a);
}
Coeff formBand(double fs, double f, double q, double bpGain, double amount, double pathGain, bool cut) {
    auto c = bandPass(fs, f, q, bpGain);
    c = scale(c, -amount);
    c = addDry(c, pathGain);
    if (cut)
        c = reciprocal(c);
    return c;
}
} // namespace
float Tone::Section::process(float x) noexcept {
    if (lowTopology) {
        const float t = (x - s2 * lattice[4]) - s1 * lattice[2];
        const float y = s2 * lattice[3] + s1 * lattice[1] + t * lattice[0];
        s2 = -s2 - t;
        s1 = s1 + t;
        return y;
    }
    const float t = x - s1 * lattice[4];
    const float u = t - s2 * lattice[3];
    const float v = t * lattice[4] + s1;
    s1 = u * lattice[3] + s2;
    s2 = u;
    return s1 * lattice[1] + v * lattice[2] + u * lattice[0];
}
void Tone::prepare(double fs) {
    rate = fs;
    filterMix.initialized = false;
    for (auto &ch : filters)
        for (auto &f : ch)
            f.ramp.initialized = false;
    for (auto &ch : bands)
        for (auto &f : ch)
            f.ramp.initialized = false;
    reset();
    setParameters(defaults());
}
void Tone::beginControlCycle(float r) {
    filterMix.begin(r);
    for (auto &ch : filters)
        for (auto &f : ch)
            f.ramp.begin(r);
    for (auto &ch : bands)
        for (auto &f : ch)
            f.ramp.begin(r);
}
void Tone::advanceControls() {
    filterMix.advance();
    for (auto &ch : filters)
        for (auto &f : ch) {
            f.ramp.advance();
            f.lattice = f.ramp.current;
        }
    for (auto &ch : bands)
        for (auto &f : ch) {
            f.ramp.advance();
            f.lattice = f.ramp.current;
        }
}
void Tone::reset() {
    for (auto &channel : filters)
        for (auto &f : channel)
            f.s1 = f.s2 = 0;
    for (auto &channel : bands)
        for (auto &f : channel)
            f.s1 = f.s2 = 0;
}
void Tone::setParameters(const Parameters &p, bool immediate) {
    if (immediate) {
        filterMix.initialized = false;
        for (auto &ch : filters)
            for (auto &f : ch)
                f.ramp.initialized = false;
        for (auto &ch : bands)
            for (auto &f : ch)
                f.ramp.initialized = false;
    }
    auto value = [&](Param key) { return double(p[index(key)]); };
    hpEnabled = value(Param::hpfOn) >= 0.5;
    lpEnabled = value(Param::lpfOn) >= 0.5;
    eqEnabled = value(Param::eqOn) >= 0.5;
    filterMix.set({hpEnabled ? 1.0f : 0.0f, lpEnabled ? 1.0f : 0.0f});
    const bool typeB = value(Param::eqType) >= 0.5;
    const bool highBell = value(Param::highBell) >= 0.5, lowBell = value(Param::lowBell) >= 0.5;
    const int selected = int(std::round(value(Param::channel) * 71));
    const int left = selected == 0 ? 0 : selected + 72;
    for (int ch = 0; ch < 2; ++ch) {
        const int channel = ch == 0 || value(Param::stereoMode) >= 0.5 ? left : left == 0 ? 72 : left + 1;
        auto R = [&](double n) { return component(n, channel); };
        auto C = [&](double n) { return component(n, channel, true); };
        auto install = [](Section &target, const Coeff &coeff, bool low = false) {
            for (std::size_t i = 0; i < 5; ++i)
                target.c[i] = float(coeff[i]);
            const double k = coeff[2] / (coeff[4] + 1);
            const double v = coeff[1] - (coeff[4] + 1) * k * coeff[3];
            target.lattice = {float(coeff[0] - v * k - coeff[3] * coeff[4]), float(v), float(k),
                              float(coeff[3]), float(coeff[4])};
            std::swap(target.lattice[2], target.lattice[3]);
            target.lowTopology = low;
            if (low)
                target.lattice = {float(coeff[0]), float((coeff[3] + coeff[0] + coeff[1]) * 0.5),
                                  float((coeff[4] + coeff[2] + 1) * 0.5),
                                  float((coeff[0] - coeff[1] + coeff[3]) * 0.5),
                                  float((1 - coeff[2] + coeff[4]) * 0.5)};
            target.ramp.set(target.lattice);
            target.lattice = target.ramp.current;
        };
        const double hp = value(Param::hpfFreq), hpMultiplier = value(Param::hpfTriple) >= 0.5 ? 3 : 1;
        const double pot = typeB ? R(50000) * interp(hpCurve, hp) : R(47000) * interp(taper, 1 - hp);
        const double r1 = R(470), r2 = R(300),
                     r3 = R(1000000) * (R(1800) + pot) / (R(1800) + R(1000000) + pot);
        const double den = (r3 + r2) * r1 + r3 * r2;
        const double f1 = hpMultiplier / (C(2.2e-7) * (den / r1) * 2 * pi);
        install(filters[ch][0], typeB ? Coeff{1, 0, 0, 0, 0} : highPass1(rate, f1, r3 * r1 / den));
        const double rr1 = R(1800) + pot, rr2 = R(1800) + pot;
        const double c1 = C(2.2e-7), c2 = C(typeB ? 3.3e-7 : 2.2e-7);
        const double rq = R(5600) * (typeB ? interp(hpQ, hp) : 1), rf = R(5600), rg = R(7500);
        const double rc = std::sqrt(rr1 * rr2 * c1 * c2);
        const double f2 = hpMultiplier / (2 * pi * rc),
                     q2 = rc / (c2 * rr2 + c1 * rr1 - rg / (rf + rq) * c2 * rr1);
        const double level = typeB ? std::pow(10.0, interp(hpLevel, hp) * 0.05) : 1 + rg / (rf + rq);
        install(filters[ch][1], highPass2(rate, f2, q2, level));
        const double lpPot = R(10000) * interp(reverseTaper, value(Param::lpfFreq));
        const double lr1 = R(680) + lpPot, lr2 = R(680) + lpPot, lc1 = C(6.8e-9), lc2 = C(3.3e-9);
        const double lc = std::sqrt(lr1 * lr2 * lc1 * lc2);
        const double lf = (value(Param::lpfThird) >= 0.5 ? 1.0 / 3 : 1) / (2 * pi * lc);
        install(filters[ch][2], lowPass2(rate, lf, lc / ((lr1 + lr2) * lc2), 1));
        // High band: a gain pot drives a frequency-dependent two-capacitor network.
        const double hf = frequencyPot(value(Param::highFreq), typeB ? 0.45299999999999996 : 0.589);
        const double hg = value(Param::highGain), hmagnitude = 2 * std::abs(hg - 0.5);
        const double gainBase = R(25000), parallel = (R(1800) * R(4700)) / (R(1800) + R(4700));
        double highPot = R(50000) * (typeB ? interp(highFrequency, hf) : interp(taper, 1 - hf));
        double hAmount = (2 * hg - 1) * gainBase / (2 * parallel + gainBase);
        if (typeB && !highBell)
            hAmount *= ((1 - float(hf)) * 0.1 + 0.9) * interp(highCorrectionB, hf);
        if (!typeB && !highBell)
            hAmount *= 1.06 - hf * 0.56;
        hAmount *= interp(typeB ? highGainB : highGainA, hmagnitude);
        double hr1, hr2, hfb, hground;
        if (typeB) {
            hr1 = R(4300) + highPot;
            hr2 = R(5100) + highPot;
            hfb = R(1800);
            hground = R(1000);
        } else {
            hr1 = R(3000) + (highBell ? 0 : R(5000)) + highPot;
            hr2 = R(3600) + (highBell ? 0 : R(5000)) + highPot;
            hfb = R(2400);
            hground = R(2000);
        }
        const double hc1 = C(highBell ? 2.2e-9
                             : typeB  ? 5.41e-9
                                      : 1.585e-8),
                     hc2 = C(highBell ? 2.2e-9 : 3.831e-11);
        const double htime = std::sqrt(hc1 * hc2 * hr1 * hr2), hsum = hc1 * hr1 + hc2 * hr2,
                     hcouple = hground * hc1 * hr2;
        const double hq = htime / (hsum - hcouple / hfb), hfrequency = 1 / (2 * pi * htime);
        const double feed =
            typeB ? R(highBell ? 26330 : 7500)
                  : R(43000) * R(highBell ? 133000 : 3100) / (R(43000) + R(highBell ? 133000 : 3100));
        const double hgain =
            feed / R(typeB ? 750 : 3000) * (hground + hfb) * hc1 * hr2 / (hsum * hfb - hcouple);
        install(bands[ch][0],
                formBand(rate, hfrequency, hq, hgain, std::abs(hAmount) * R(4750) / R(typeB ? 27400 : 10000),
                         -R(4750) / R(4750), hAmount < 0));
        // The mid networks share the same resistor bridge, with distinct caps.
        auto mid = [&](bool upper) {
            const double fn = value(upper ? Param::highMidFreq : Param::lowMidFreq),
                         gn = value(upper ? Param::highMidGain : Param::lowMidGain),
                         qn = value(upper ? Param::highMidQ : Param::lowMidQ);
            const double fp = frequencyPot(fn, upper ? (typeB ? 0.608 : 0.444) : (typeB ? 0.431 : 0.452));
            const double freqPot =
                R(50000) * (upper ? (typeB ? interp(taper, 1 - fp) : interp(hmFrequency, fp))
                                  : interp(typeB ? lmFrequencyB : lmFrequencyA, fp));
            const double qp = R(10000) * interp(taper, qn);
            const double a1 = R(typeB ? (upper ? 4300 : 2750) : 3000) + freqPot;
            const double a2 = R(typeB ? (upper ? 5100 : 3260) : 3600) + freqPot;
            const double cap = C(upper ? 4.7e-9 : typeB ? 1.5e-8 : 2.2e-8);
            double amount = (2 * gn - 1) * parallel / (parallel + R(1000));
            const double magn = 2 * std::abs(gn - 0.5);
            if (!upper && !typeB)
                amount = (2 * interp(lmPot, gn) - 1) * parallel / (parallel + R(1000));
            amount *= interp(upper ? (typeB ? hmGainB : hmGainA) : (typeB ? lmGainB : lmGainA), magn);
            double qVariable = qp;
            if (!typeB) {
                qVariable += fp * 400;
                amount *= 1 + ((qn * 0.21999999999999997 - 0.33999999999999997) * fp);
            }
            const double bridge = R(typeB ? 1000 : 1800), rb = R(100000), rd = R(10000) - qVariable;
            const double inv = 1 / (bridge + rb + rd), u = rd * rb * inv, v = R(18000) + rd * bridge * inv;
            const double inv2 = 1 / (u + v + qVariable), coupling = bridge * rb * inv + u * v * inv2;
            const double total = R(910) + R(3340) + qVariable * v * inv2;
            const double sum = cap * a2 + cap * a1, term = cap * a2 * coupling;
            const double time = std::sqrt(a1 * a2 * cap * cap);
            double q = time / (sum - term / total);
            // The alternate lower-mid curve adds a Q compensation law.
            if (typeB && !upper)
                q *= 1.11 + (1 - float(qn)) * 0.11;
            const double response =
                (R(750) + qVariable) / R(750) * ((total + coupling) * cap * a2) / (sum * total - term);
            const double upperGain = interp(typeB ? hmCorrectionTopB : hmCorrectionTopA,
                                            std::max(0.0, 2 * value(Param::highGain) - 1));
            const double lowerGain = interp(typeB ? hmCorrectionBottomB : hmCorrectionBottomA,
                                            std::max(0.0, 2 * value(Param::highGain) - 1));
            const double lowUpper = interp(typeB ? lmCorrectionTopB : lmCorrectionTopA,
                                           std::max(0.0, 2 * value(Param::lowGain) - 1));
            const double lowLower = interp(typeB ? lmCorrectionBottomB : lmCorrectionBottomA,
                                           std::max(0.0, 2 * value(Param::lowGain) - 1));
            const double correction =
                std::pow(10.0, (qn * ((upper ? upperGain : lowUpper) - (upper ? lowerGain : lowLower)) +
                                (upper ? lowerGain : lowLower)) *
                                   0.05);
            return formBand(rate, 1 / (2 * pi * time), q, response,
                            std::abs(amount) * correction * R(4750) /
                                R(upper   ? (typeB ? 4500 : 4570)
                                  : typeB ? 4000
                                          : 4570),
                            -R(4750) / R(4750), amount < 0);
        };
        install(bands[ch][1], mid(true));
        install(bands[ch][3], mid(false));
        const double ln = value(Param::lowFreq), lg = value(Param::lowGain), lmagn = 2 * std::abs(lg - 0.5);
        const double lfp = frequencyPot(ln, typeB ? 0.447 : 0.47),
                     lpot = R(50000) * interp(lowFrequency, 1 - lfp);
        const double lowParallel = component(1800, ch) * R(4700) / (component(1800, ch) + R(4700));
        double lAmount = (2 * interp(lowPot, lg) - 1) * gainBase / (lowParallel + gainBase + lowParallel);
        lAmount *= interp(typeB ? lowGainB : lowGainA, lmagn);
        if (typeB)
            lAmount *= 0.85;
        else
            lAmount *= lowBell ? (lfp < 0.5 ? 1.19 : 1.38 - lfp * 0.3799999999999999)
                               : 1 - lfp * 0.050000000000000044;
        const double la = R(4300) + lpot, lb = R(typeB ? 5100 : 4700) + lpot;
        const double lr = R(typeB ? 1000 : 2400) + R(lowBell ? 0
                                                     : typeB ? 2200
                                                             : 100000),
                     lgnd = R(typeB ? 1000 : 3600);
        const double lcap = lowBell ? C(3.3e-8) + C(4.7e-8) : C(0.0001), lcap2 = C(4.7e-8) + C(3.3e-8);
        const double ltime = std::sqrt(lcap2 * lcap * lb * la), lsum = lcap2 * lb + lcap * la,
                     lterm = lgnd * lcap * lb;
        const double lfrequency = (1 / (2 * pi * ltime)) * (typeB     ? 1
                                                            : lowBell ? lfp * 0.08999999999999997 + 0.85
                                                                      : 1.05);
        const double lq = ltime / (lsum - lterm / lr);
        const double lfeedback = R(typeB ? 3300 : lowBell ? 30700 : 130700);
        const double lresponse =
            lfeedback / R(typeB ? 750 : 3000) * (lgnd + lr) * lcap * lb / (lr * lsum - lterm);
        install(bands[ch][2],
                formBand(rate, lfrequency, lq, lresponse,
                         std::abs(lAmount) * R(4750) / R(typeB ? 7500 : 36560), -R(4750) / R(4750),
                         lAmount < 0),
                true);
    }
}
float Tone::processFilters(float x, int channel) {
    auto &f = filters[std::clamp(channel, 0, 1)];
    const float hp = f[1].process(f[0].process(x));
    x = filterMix.current[0] == 1   ? hp
        : filterMix.current[0] == 0 ? x
                                    : hp * filterMix.current[0] + x * (1 - filterMix.current[0]);
    const float lp = f[2].process(x);
    return filterMix.current[1] == 1   ? lp
           : filterMix.current[1] == 0 ? x
                                       : lp * filterMix.current[1] + x * (1 - filterMix.current[1]);
}
float Tone::processEqStage(float x, int channel, int stage) {
    return bands[std::clamp(channel, 0, 1)][std::clamp(stage, 0, 3)].process(x);
}
float Tone::processEq(float x, int channel) {
    if (!eqEnabled)
        return x;
    for (auto &b : bands[std::clamp(channel, 0, 1)])
        x = b.process(x);
    return x;
}
} // namespace strip
