#include "Engine.h"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

// Offline, stereo interleaved IEEE float32 renderer. Parameter files accept
// stable parameter keys or measurement IDs followed by normalized values.
int main(int argc, char **argv) {
    if (argc < 3) {
        std::cerr << "Usage: strip-render input.f32 output.f32 [parameters.txt] [sample-rate] [block-size] "
                     "[noise-seed] [settle-frames]\n";
        return 2;
    }
    auto parameters = strip::defaults();
    if (argc > 3) {
        std::ifstream config(argv[3]);
        if (!config) {
            std::cerr << "Cannot open parameter file\n";
            return 1;
        }
        std::string line;
        while (std::getline(config, line)) {
            std::istringstream row(line);
            std::string key;
            float value;
            if (!(row >> key) || key[0] == '#')
                continue;
            if (!(row >> value)) {
                std::cerr << "Invalid parameter value\n";
                return 1;
            }
            bool found = false;
            for (std::size_t i = 0; i < strip::parameterCount; ++i) {
                const auto &spec = strip::parameterSpecs[i];
                if (key == spec.id || key == std::to_string(spec.probeId)) {
                    parameters[i] = value;
                    found = true;
                    break;
                }
            }
            if (!found) {
                std::cerr << "Unknown parameter: " << key << '\n';
                return 1;
            }
        }
    }
    const double rate = argc > 4 ? std::atof(argv[4]) : 48000;
    const int block = argc > 5 ? std::atoi(argv[5]) : 256;
    if (!std::isfinite(rate) || rate < 8000 || block < 1 || block > 1048576) {
        std::cerr << "Invalid processing setup\n";
        return 1;
    }
    std::ifstream input(argv[1], std::ios::binary);
    std::ofstream output(argv[2], std::ios::binary);
    if (!input || !output) {
        std::cerr << "Cannot open audio file\n";
        return 1;
    }
    strip::Engine engine;
    engine.prepare(rate, block);
    engine.setParameters(parameters);
    if (argc > 7) {
        const int settle = std::max(0, std::atoi(argv[7]));
        std::vector<float> a(static_cast<std::size_t>(block)), b(a.size());
        for (int i = 0; i < settle; i += block) {
            std::fill(a.begin(), a.end(), 0);
            std::fill(b.begin(), b.end(), 0);
            engine.process(a.data(), b.data(), nullptr, nullptr, std::min(block, settle - i));
        }
        engine.reset();
    }
    if (argc > 6)
        engine.setNoiseSeed(static_cast<std::uint32_t>(std::strtoul(argv[6], nullptr, 10)));
    std::vector<float> interleaved(static_cast<std::size_t>(block) * 2),
        left(static_cast<std::size_t>(block)), right(static_cast<std::size_t>(block));
    std::size_t frames = 0;
    while (input.read(reinterpret_cast<char *>(interleaved.data()),
                      static_cast<std::streamsize>(interleaved.size() * sizeof(float))) ||
           input.gcount()) {
        const auto bytes = input.gcount();
        if (bytes % 8) {
            std::cerr << "Incomplete stereo frame\n";
            return 1;
        }
        const int n = static_cast<int>(bytes / 8);
        for (int i = 0; i < n; ++i) {
            left[static_cast<std::size_t>(i)] = interleaved[static_cast<std::size_t>(i) * 2];
            right[static_cast<std::size_t>(i)] = interleaved[static_cast<std::size_t>(i) * 2 + 1];
        }
        engine.process(left.data(), right.data(), nullptr, nullptr, n);
        for (int i = 0; i < n; ++i) {
            interleaved[static_cast<std::size_t>(i) * 2] = left[static_cast<std::size_t>(i)];
            interleaved[static_cast<std::size_t>(i) * 2 + 1] = right[static_cast<std::size_t>(i)];
        }
        output.write(reinterpret_cast<const char *>(interleaved.data()), bytes);
        frames += static_cast<std::size_t>(n);
    }
    std::cout << "Rendered " << frames << " stereo frames\n";
    return output ? 0 : 1;
}
