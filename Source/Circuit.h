#pragma once
#include <cstdint>
#include <cstring>
namespace strip {
inline double circuitComponent(double value, int channel, bool capacitor = false) noexcept {
    std::uint64_t bits;
    std::memcpy(&bits, &value, sizeof(bits));
    std::uint32_t state = (std::uint32_t(bits) ^ std::uint32_t(bits >> 32)) | 1u;
    constexpr std::uint32_t multipliers[]{0x41a7u,     0x10d63af1u, 0x60b7a437u, 0xb87e16e1u,
                                          0x5e100dc7u, 0x24960e57u, 0x6ff883d1u, 0xdd3d35e7u,
                                          0xf78771c1u, 0xdd9dd0b1u, 0xe6cf90a1u, 0xa3f01477u};
    double sum = 0;
    for (int j = 0; j < channel; ++j) {
        sum = 0;
        for (auto multiplier : multipliers) {
            const std::uint32_t raw = state * multiplier;
            std::int32_t signedValue;
            std::memcpy(&signedValue, &raw, sizeof(raw));
            sum += double(signedValue);
        }
        state *= 0xe6cf90a1u;
    }
    return value + value * (capacitor ? 10.0 : 3.0) * sum * 4.656612873077393e-10 * 0.005;
}
} // namespace strip
