#include "Noise.h"

#include <chrono>
#include <cstdint>

namespace bassline {
namespace dsp {

Noise::Noise(unsigned int seed)
    : generator_(),
      distribution_(-1.0, 1.0) {
    setSeed(seed);
}

void Noise::setSeed(unsigned int seed) {
    if (seed == 0) {
        seed = static_cast<unsigned int>(
            std::chrono::system_clock::now().time_since_epoch().count());
        seed ^= static_cast<unsigned int>(reinterpret_cast<std::uintptr_t>(this));
    }

    generator_.seed(seed);
}

double Noise::tick() {
    return distribution_(generator_);
}

}  // namespace dsp
}  // namespace bassline
