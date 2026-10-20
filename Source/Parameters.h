#pragma once
#include <array>
#include <cstdint>
#include <string>
namespace strip {
enum class Param : std::size_t {
    power,
    mute,
    setting,
    inputGain,
    noise,
    drive,
    phase,
    outputGain,
    monitor,
    channel,
    stereoMode,
    hpfOn,
    hpfFreq,
    hpfTriple,
    lpfOn,
    lpfFreq,
    lpfThird,
    filterPosition,
    dynamicsOn,
    ratio,
    compThreshold,
    compRelease,
    compFast,
    compMix,
    compLink,
    sidechainHP,
    secondThreshold,
    gateMode,
    gateInvert,
    gateRange,
    gateThreshold,
    gateThresholdRange,
    gateRelease,
    gateFast,
    hysteresis,
    vcaMode,
    externalKey,
    eqOn,
    highGain,
    highFreq,
    highBell,
    highMidGain,
    highMidFreq,
    highMidQ,
    eqType,
    lowMidGain,
    lowMidFreq,
    lowMidQ,
    lowGain,
    lowFreq,
    lowBell,
    eqPosition,
    meterPosition,
    bypass
};
inline constexpr std::size_t parameterCount = 54;
struct ParamSpec {
    const char *id;
    const char *name;
    const char *unit;
    float initial;
    int steps;
    std::uint32_t probeId;
};
inline constexpr std::array<ParamSpec, parameterCount> parameterSpecs{{
    {"power", "Power", "", 1.0000000000f, 1, 1349482354u},
    {"mute", "Mute", "", 0.0000000000f, 1, 1299543141u},
    {"setting", "Setting", "", 0.0000000000f, 3, 1349674355u},
    {"inputGain", "Input Gain", "dB", 0.5000000000f, 0, 1231964014u},
    {"noise", "Noise Level", "dB", 0.6000000238f, 0, 1315924339u},
    {"drive", "Drive", "dB", 0.6669999957f, 0, 1414022259u},
    {"phase", "Phase", "", 0.0000000000f, 1, 1349022565u},
    {"outputGain", "Out Gain", "dB", 0.7500000000f, 0, 1333097543u},
    {"monitor", "Monitoring", "", 0.0000000000f, 2, 1299084402u},
    {"channel", "Channel", "", 0.0000000000f, 71, 1128491084u},
    {"stereoMode", "Channel Variation", "", 0.0000000000f, 1, 1400130927u},
    {"hpfOn", "HPF On/Off", "", 1.0000000000f, 1, 1213221230u},
    {"hpfFreq", "HPF Frequency", "Hz", 0.0000000000f, 0, 1181501520u},
    {"hpfTriple", "HPF x3", "", 0.0000000000f, 1, 1179144276u},
    {"lpfOn", "LPF On/Off", "", 1.0000000000f, 1, 1280330094u},
    {"lpfFreq", "LPF Frequency", "Hz", 0.0000000000f, 0, 1181502544u},
    {"lpfThird", "LPF /3", "", 0.0000000000f, 1, 1179406404u},
    {"filterPosition", "Filter Routing", "", 0.0000000000f, 1, 1179406451u},
    {"dynamicsOn", "Dynamics On/Off", "", 1.0000000000f, 1, 1148808777u},
    {"ratio", "Compressor Ratio", "", 0.5000000000f, 0, 1383360879u},
    {"compThreshold", "Compressor Threshold", "dB", 0.0000000000f, 0, 1129605234u},
    {"compRelease", "Compressor Release", "s", 0.5000000000f, 0, 1129473388u},
    {"compFast", "Compressor Attack", "", 0.0000000000f, 1, 1128677748u},
    {"compMix", "Compressor Mix", "%", 1.0000000000f, 0, 1129146744u},
    {"compLink", "Compressor Link", "", 0.0000000000f, 1, 1281977963u},
    {"sidechainHP", "Compressor Highpass", "", 0.0000000000f, 0, 1128812614u},
    {"secondThreshold", "Compressor 2nd Thresh Level", "%", 0.0000000000f, 0, 1129605202u},
    {"gateMode", "Gate Mode", "", 0.0000000000f, 1, 1162305908u},
    {"gateInvert", "Gate Invert", "", 0.0000000000f, 1, 1162440310u},
    {"gateRange", "Gate Range", "dB", 0.5000000000f, 0, 1163030119u},
    {"gateThreshold", "Gate Threshold", "dB", 0.0000000000f, 0, 1163159666u},
    {"gateThresholdRange", "Gate Threshold Range", "dB", 1.0000000000f, 1, 1163159634u},
    {"gateRelease", "Gate Release", "s", 0.5000000000f, 0, 1163027820u},
    {"gateFast", "Gate Attack", "", 0.0000000000f, 1, 1162232180u},
    {"hysteresis", "Gate Hysteresis", "dB", 0.0000000000f, 0, 1215918964u},
    {"vcaMode", "Dynamics VCA Series", "", 0.0000000000f, 1, 1399157353u},
    {"externalKey", "Dynamics Key", "", 0.0000000000f, 1, 1165513571u},
    {"eqOn", "EQ On/Off", "", 1.0000000000f, 1, 1162955118u},
    {"highGain", "EQ High Gain", "dB", 0.5000000000f, 0, 1212565358u},
    {"highFreq", "EQ High Frequency", "Hz", 0.5000000000f, 0, 1212565105u},
    {"highBell", "EQ High Bell", "", 0.0000000000f, 1, 1212564076u},
    {"highMidGain", "EQ High Mid Gain", "dB", 0.5000000000f, 0, 1213024110u},
    {"highMidFreq", "EQ High Mid Frequency", "Hz", 0.5000000000f, 0, 1213023857u},
    {"highMidQ", "EQ High Mid Q", "", 0.5000000000f, 0, 1213031505u},
    {"eqType", "EQ Curve", "", 0.0000000000f, 1, 1417244773u},
    {"lowMidGain", "EQ Low Mid Gain", "dB", 0.5000000000f, 0, 1280132974u},
    {"lowMidFreq", "EQ Low Mid Frequency", "Hz", 0.5000000000f, 0, 1280132721u},
    {"lowMidQ", "EQ Low Mid Q", "", 0.5000000000f, 0, 1280140369u},
    {"lowGain", "EQ Low Gain", "dB", 0.5000000000f, 0, 1279674222u},
    {"lowFreq", "EQ Low Frequency", "Hz", 0.5000000000f, 0, 1279673969u},
    {"lowBell", "EQ Low Bell", "", 0.0000000000f, 1, 1279672940u},
    {"eqPosition", "EQ Position", "", 1.0000000000f, 2, 1162953849u},
    {"meterPosition", "Meter Position", "", 0.0000000000f, 1, 1229809012u},
    {"bypass", "Bypass", "", 0.0000000000f, 1, 1296202096u},
}};
using Parameters = std::array<float, parameterCount>;
constexpr std::size_t index(Param p) noexcept {
    return static_cast<std::size_t>(p);
}
inline Parameters defaults() noexcept {
    Parameters p{};
    for (std::size_t i = 0; i < p.size(); ++i)
        p[i] = parameterSpecs[i].initial;
    return p;
}
double physical(Param p, double normalized) noexcept;
std::string parameterText(Param p, double normalized);
} // namespace strip
