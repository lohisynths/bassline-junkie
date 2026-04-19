#include "Noise.h"

#include <chrono>
#include <cstdint>

namespace bassline {
namespace dsp {

Noise::Noise(unsigned int seed)
    : state_(0) {
    setSeed(seed);
}

void Noise::setSeed(unsigned int seed) {
    if (seed == 0) {
        seed = static_cast<unsigned int>(
            std::chrono::system_clock::now().time_since_epoch().count());
        seed ^= static_cast<unsigned int>(reinterpret_cast<std::uintptr_t>(this));
    }

    state_ = seed;
    if (state_ == 0) {
        state_ = 0x6d2b79f5u;
    }
}

double Noise::tick() {
    state_ ^= state_ << 13;
    state_ ^= state_ >> 17;
    state_ ^= state_ << 5;

    return static_cast<double>(state_) * (2.0 / 4294967295.0) - 1.0;
}

}  // namespace dsp
}  // namespace bassline
