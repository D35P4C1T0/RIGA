#pragma once
#include "ControlRamp.h"
#include "Parameters.h"
#include <array>
namespace strip {
class Tone {
  public:
    void prepare(double sampleRate);
    void reset();
    void setParameters(const Parameters &parameters, bool immediate = false);
    void beginControlCycle(float reciprocalLength);
    void advanceControls();
    float processFilters(float input, int channel);
    float processEq(float input, int channel);
    float processEqStage(float input, int channel, int stage);

  private:
    struct Section {
        std::array<float, 5> c{1, 0, 0, 0, 0};
        std::array<float, 5> lattice{1, 0, 0, 0, 0};
        ControlRamp<5> ramp;
        bool lowTopology = false;
        float s1 = 0, s2 = 0;
        float process(float x) noexcept;
    };
    double rate = 48000;
    bool hpEnabled = true, lpEnabled = true, eqEnabled = true;
    ControlRamp<2> filterMix;
    std::array<std::array<Section, 3>, 2> filters;
    std::array<std::array<Section, 4>, 2> bands;
};
} // namespace strip
