#ifndef BASSLINE_JUNKIE_SRC_SYNTH_DSP_SINEWAVE_H_
#define BASSLINE_JUNKIE_SRC_SYNTH_DSP_SINEWAVE_H_

#include <array>

namespace bassline {
namespace dsp {

// Based on the STK SineWave oscillator, localized for this repo.
class SineWave {
public:
    SineWave();

    void reset();
    void setRate(double rate);
    void setFrequency(double frequency);
    void addTime(double time);
    void addPhase(double phase);
    void addPhaseOffset(double phaseOffset);
    void setSampleRate(double sampleRate);
    double tick();

private:
    static void initTable();

    static const unsigned long kTableSize = 2048;
    static std::array<double, kTableSize + 1> table_;
    static bool tableInitialized_;

    double time_;
    double rate_;
    double phaseOffset_;
    double sampleRate_;
};

}  // namespace dsp
}  // namespace bassline

#endif  // BASSLINE_JUNKIE_SRC_SYNTH_DSP_SINEWAVE_H_
