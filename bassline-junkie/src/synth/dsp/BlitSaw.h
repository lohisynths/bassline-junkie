#ifndef BASSLINE_JUNKIE_SRC_SYNTH_DSP_BLITSAW_H_
#define BASSLINE_JUNKIE_SRC_SYNTH_DSP_BLITSAW_H_

#include <cstddef>

namespace bassline {
namespace dsp {

// Based on the STK BlitSaw oscillator, localized for this repo.
class BlitSaw {
public:
    explicit BlitSaw(double frequency = 220.0);

    void reset();
    void setFrequency(double frequency);
    void setHarmonics(unsigned int nHarmonics = 0);
    void setSampleRate(double sampleRate);
    double tick();

private:
    void updateFrequencyState();
    void updateHarmonics();

    unsigned int nHarmonics_;
    unsigned int m_;
    double frequency_;
    double sampleRate_;
    double rate_;
    double phase_;
    double p_;
    double c2_;
    double a_;
    double state_;
};

}  // namespace dsp
}  // namespace bassline

#endif  // BASSLINE_JUNKIE_SRC_SYNTH_DSP_BLITSAW_H_
