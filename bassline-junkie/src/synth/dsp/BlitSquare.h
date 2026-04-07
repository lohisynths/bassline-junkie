#ifndef BASSLINE_JUNKIE_SRC_SYNTH_DSP_BLITSQUARE_H_
#define BASSLINE_JUNKIE_SRC_SYNTH_DSP_BLITSQUARE_H_

namespace bassline {
namespace dsp {

// Based on the STK BlitSquare oscillator, localized for this repo.
class BlitSquare {
public:
    explicit BlitSquare(double frequency = 220.0);

    void reset();
    void setPhase(double normalizedPhase);
    double getPhase() const;
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
    double a_;
    double lastBlitOutput_;
    double dcbState_;
    double lastOutput_;
};

}  // namespace dsp
}  // namespace bassline

#endif  // BASSLINE_JUNKIE_SRC_SYNTH_DSP_BLITSQUARE_H_
