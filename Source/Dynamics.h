#pragma once
#include "ControlRamp.h"
#include "Parameters.h"
#include <array>
namespace strip {
class Dynamics {
  public:
    void prepare(double sampleRate, int channels = 2);
    void reset();
    void setParameters(const Parameters &, bool immediate = false);
    void beginControlCycle(float reciprocalLength) {
        controls_.begin(reciprocalLength);
    }
    void advanceControls() {
        controls_.advance();
    }
    void process(float &left, float &right, float keyLeft, float keyRight);
    float compressionDb() const noexcept {
        return compressorMeter_;
    }
    float gateDb() const noexcept {
        return gateMeter_;
    }

  private:
    struct Lattice {
        std::array<float, 5> coefficient{};
        float first{}, second{};
        float process(float);
    };
    struct Channel {
        Lattice keyFilter, compressorFilter;
        std::array<float, 8> capacitor{};
        float compressorPower{}, gatePower{}, first{}, second{}, idle{}, average{};
        float gateState{}, switchState{}, dcState{};
        float ratio{}, threshold{}, offset{}, gateThreshold{}, gateAttack{}, gateRelease{}, range{};
    };
    std::array<Channel, 2> channels_{};
    Parameters parameters_ = defaults();
    std::array<float, 5> vca_{};
    std::array<float, 3> dc_{};
    ControlRamp<12> controls_;
    bool mono_ = false;
    double sampleRate_ = 48000;
    float detector_{}, idleRate_{}, averageRate_{}, compressorMeter_{}, gateMeter_{};
};
} // namespace strip
