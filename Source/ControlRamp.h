#pragma once
#include <array>
#include <cstddef>

namespace strip {
// Control changes enter on a fixed processing cycle. Each cycle starts at the
// previous endpoint, avoiding accumulated float error across successive ramps.
template <std::size_t N> struct ControlRamp {
    std::array<float, N> current{}, target{}, previous{}, increment{};
    bool initialized = false;
    void set(const std::array<float, N> &value) noexcept {
        target = value;
        if (!initialized) {
            current = previous = value;
            increment.fill(0);
            initialized = true;
        }
    }
    void begin(float reciprocalLength) noexcept {
        current = previous;
        previous = target;
        for (std::size_t i = 0; i < N; ++i)
            increment[i] = (target[i] - current[i]) * reciprocalLength;
    }
    void advance() noexcept {
        for (std::size_t i = 0; i < N; ++i)
            current[i] += increment[i];
    }
};
} // namespace strip
