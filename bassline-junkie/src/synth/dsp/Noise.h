#ifndef BASSLINE_JUNKIE_SRC_SYNTH_DSP_NOISE_H_
#define BASSLINE_JUNKIE_SRC_SYNTH_DSP_NOISE_H_

#include <random>

namespace bassline {
namespace dsp {

// Based on the STK Noise generator, localized for this repo.
class Noise {
public:
    explicit Noise(unsigned int seed = 0);

    void setSeed(unsigned int seed = 0);
    double tick();

private:
    std::mt19937 generator_;
    std::uniform_real_distribution<double> distribution_;
};

}  // namespace dsp
}  // namespace bassline

#endif  // BASSLINE_JUNKIE_SRC_SYNTH_DSP_NOISE_H_
