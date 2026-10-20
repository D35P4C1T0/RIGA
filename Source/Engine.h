#pragma once
#include "Dynamics.h"
#include "Parameters.h"
#include "Tone.h"
#include <array>
#include <cstdint>

namespace strip {
struct Meters {
    float inputL{}, inputR{}, outputL{}, outputR{}, compression{}, gate{};
};

class Engine {
  public:
    void prepare(double sampleRate, int maximumBlockSize, int channels = 2);
    void reset();
    void setParameters(const Parameters &parameters);
    void process(float *left, float *right, const float *keyLeft, const float *keyRight, int count) noexcept;
    void setNoiseSeed(std::uint32_t seed) noexcept {
        randomState = seed;
    }
    Meters meters() const noexcept {
        return meter;
    }

  private:
    struct DcSection {
        std::array<float, 5> c{};
        float first{}, second{};
        float process(float input) noexcept;
    };
    float equalize(float input, int channel) noexcept;
    void randomPair(float &left, float &right) noexcept;
    Tone tone;
    Dynamics dynamics;
    Parameters values = defaults();
    std::array<DcSection, 2> dc{};
    std::array<float, 4> normalizer{1, 1, 1, 1};
    std::array<float, 4> inverseNormalizer{1, 1, 1, 1};
    double rate = 48000;
    float inputGain = 1, outputGain = 1, noise = 0;
    float cubicLinear = 1, cubicThird = 0, cubicLimit = 1;
    std::uint32_t randomState = 1;
    Meters meter{};
    bool initialized = false, hasProcessed = false;
    ControlRamp<26> controls;
    int controlLength = 2560, controlPosition = 0, channelCount = 2;
};
} // namespace strip
