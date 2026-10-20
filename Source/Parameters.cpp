#include "Parameters.h"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <limits>

namespace strip {
namespace {
template <std::size_t N> double interpolate(const double (&knots)[N], double normalized) noexcept {
    const double position = normalized * (N - 1);
    const auto lower = std::min(static_cast<std::size_t>(position), N - 1);
    const auto upper = std::min(lower + 1, N - 1);
    return knots[lower] + (knots[upper] - knots[lower]) * (position - lower);
}
constexpr double output[]{-120.0, -41.2, -30.9, -20.9, -10.2, -5.09, 0.019, 4.987, 10.0};
constexpr double ratio[]{1, 2, 3, 5, 8, 20, 100};
constexpr double range[]{0, 5, 10, 20, 30, 35, 40};
constexpr double threshold[]{-30, -25, -20, -12, -4, 5, 10};
constexpr double quality[]{3, 2, 1.5, 1, 0.5};
constexpr double hpfLog[]{2.772588722239781, 2.995732273553991, 4.248495242049359, 4.787491742782046,
                          5.298317366548036, 5.703782474656201, 5.857933154483459};
constexpr double lpfLog[]{9.998797732340453, 9.392661928770137, 8.987196820661973, 8.517193191416238,
                          8.294049640102028, 8.160518247477505, 8.006367567650246};
constexpr double highLog[]{7.313220387090301, 7.600902459542082, 8.517193191416238, 8.987196820661973,
                           9.210340371976184, 9.546812608597396, 9.680344001221918};
constexpr double highMidLog[]{6.396929655216146, 6.684611727667927, 7.313220387090301, 8.006367567650246,
                              8.411832675758411, 8.699514748210191, 8.85366542803745};
constexpr double lowMidLog[]{5.298317366548036, 5.703782474656201, 6.684611727667927, 6.907755278982137,
                             7.313220387090301, 7.600902459542082, 7.824046010856292};
constexpr double lowLog[]{3.4011973816621555, 3.912023005428146, 4.605170185988092, 5.298317366548036,
                          5.703782474656201,  5.857933154483459, 6.1092475827643655};
constexpr double releaseLog[]{
    -2.3025850929940455, -1.8971199848858813, -1.6094379124341003, -0.916290731874155, 0,
    0.6931471805599453,  1.3862943611198906};
double bounded(double value) noexcept {
    return std::isfinite(value) ? std::clamp(value, 0.0, 1.0) : 0.0;
}
std::string number(double value, int decimals, const char *suffix = "", bool sign = false) {
    char buffer[80];
    std::snprintf(buffer, sizeof(buffer), sign && value > 0 ? "%+.*f%s" : "%.*f%s", decimals, value, suffix);
    return buffer;
}
} // namespace

double physical(Param p, double normalized) noexcept {
    const double n = bounded(normalized);
    switch (p) {
    case Param::inputGain:
        return n * 20 - 10;
    case Param::noise:
        return n <= 1e-6 ? -std::numeric_limits<double>::infinity() : n * 50 - 120;
    case Param::drive:
        return n * 90 - 120;
    case Param::outputGain:
        return interpolate(output, n);
    case Param::channel:
        return std::floor(n * 71 + 0.5) + 1;
    case Param::hpfFreq:
        return std::exp(interpolate(hpfLog, n));
    case Param::lpfFreq:
        return std::exp(interpolate(lpfLog, n));
    case Param::ratio:
        return interpolate(ratio, n);
    case Param::compThreshold:
        return 10 - 30 * n;
    case Param::compRelease:
    case Param::gateRelease:
        return std::exp(interpolate(releaseLog, n));
    case Param::compMix:
    case Param::secondThreshold:
        return n * 100;
    case Param::sidechainHP:
        return n <= 0 ? 0 : 10 * std::pow(200.0, n);
    case Param::gateRange:
        return interpolate(range, n);
    // The range switch applies a separate -30 dB offset to this base curve.
    case Param::gateThreshold:
        return interpolate(threshold, n);
    case Param::gateThresholdRange:
        return n < 0.5 ? 0 : -30;
    case Param::hysteresis:
        return -25 * n;
    case Param::highGain:
    case Param::highMidGain:
    case Param::lowMidGain:
    case Param::lowGain:
        return n * 30 - 15;
    case Param::highFreq:
        return std::exp(interpolate(highLog, n));
    case Param::highMidFreq:
        return std::exp(interpolate(highMidLog, n));
    case Param::lowMidFreq:
        return std::exp(interpolate(lowMidLog, n));
    case Param::lowFreq:
        return std::exp(interpolate(lowLog, n));
    case Param::highMidQ:
    case Param::lowMidQ:
        return interpolate(quality, n);
    default: {
        const auto i = index(p);
        return i < parameterCount && parameterSpecs[i].steps > 0
                   ? std::floor(n * parameterSpecs[i].steps + 0.5)
                   : n;
    }
    }
}

std::string parameterText(Param p, double normalized) {
    const double n = bounded(normalized), value = physical(p, n);
    const bool on = n >= 0.5;
    switch (p) {
    case Param::setting:
        return std::string(1, static_cast<char>('A' + static_cast<int>(value)));
    case Param::monitor:
        return value < 1 ? "Stereo" : value < 2 ? "Mid" : "Side";
    case Param::channel:
        return number(value, 0);
    case Param::stereoMode:
        return on ? "Matched" : "Varied";
    case Param::filterPosition:
        return on ? "Sidechain" : "Input";
    case Param::gateMode:
        return on ? "Gate" : "Expander";
    case Param::vcaMode:
        return on ? "Curve B" : "Curve A";
    case Param::eqType:
        return on ? "Curve B" : "Curve A";
    case Param::highBell:
    case Param::lowBell:
        return on ? "Bell" : "Shelf";
    case Param::eqPosition:
        return value < 1 ? "Pre Dynamics" : value < 2 ? "Dynamics Sidechain" : "Post Dynamics";
    case Param::meterPosition:
        return on ? "Input" : "Output";
    case Param::noise:
        return n <= 1e-6 ? "-inf dB" : number(value, 1, " dB");
    case Param::drive:
        return n <= 1e-14 ? "Off" : number(value, 1, " dB");
    case Param::inputGain:
    case Param::outputGain:
    case Param::compThreshold:
    case Param::gateRange:
    case Param::gateThreshold:
    case Param::gateThresholdRange:
    case Param::hysteresis:
    case Param::highGain:
    case Param::highMidGain:
    case Param::lowMidGain:
    case Param::lowGain:
        return number(value, 1, " dB");
    case Param::ratio:
        return n >= 1 ? "inf:1" : number(value, 1, ":1");
    case Param::compRelease:
    case Param::gateRelease:
        return number(value, 2, " s");
    case Param::compMix:
    case Param::secondThreshold:
        return number(value, 1, " %");
    case Param::sidechainHP:
        return n <= 0 ? "Off" : number(value, 1, " Hz");
    case Param::hpfFreq:
    case Param::lpfFreq:
    case Param::highFreq:
    case Param::highMidFreq:
    case Param::lowMidFreq:
    case Param::lowFreq:
        return value >= 999.5 ? number(value * 0.001, 1, " kHz") : number(value, 1, " Hz");
    case Param::highMidQ:
    case Param::lowMidQ:
        return number(value, 1);
    default:
        return on ? "On" : "Off";
    }
}
} // namespace strip
