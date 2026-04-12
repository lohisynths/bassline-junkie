#ifndef BASSLINE_JUNKIE_SRC_SYNTH_DSP_MINBLEP_OSCILLATOR_H_
#define BASSLINE_JUNKIE_SRC_SYNTH_DSP_MINBLEP_OSCILLATOR_H_

namespace bassline {
namespace dsp {

// Band-limited oscillator using a linear-phase BLEP (Band-Limited stEP)
// for alias suppression.  Supports sawtooth and square (pulse) waveforms.
//
// Architecture (following VCV Rack / ExperimentalScene best practice):
//   - corrBuf_:   ring buffer accumulating ONLY BLEP residuals
//   - delayLine_: delays the naive waveform by kZeroCrossings samples
//   - output = delayedNaive + correction
//
// The BLEP table is a Blackman-Harris-windowed sinc integrated into a step
// function.  The linear-phase design spreads corrections symmetrically
// around each discontinuity, requiring an output delay of kZeroCrossings
// samples (~0.73 ms at 44.1 kHz) but avoiding the STFT spectral leakage
// inherent to minimum-phase variants.
class MinBLEPOscillator {
public:
    enum Waveform {
        SAW,
        SQUARE
    };

    explicit MinBLEPOscillator(double frequency = 220.0);

    void reset();
    void setFrequency(double frequency);
    void setSampleRate(double sampleRate);
    void setWaveform(Waveform waveform);
    void setPulseWidth(double pw);   // 0.0 – 1.0, default 0.5
    double tick();

private:
    // 32 zero-crossings with Blackman-Harris (-92 dB sidelobes) and
    // bandlimit = 0.9 gives a transition band of ~3.8 kHz at 44.1 kHz,
    // putting the -92 dB stopband comfortably below Nyquist.
    static constexpr int kZeroCrossings = 32;
    static constexpr int kOversampling  = 64;
    static constexpr int kBlepLen       = 2 * kZeroCrossings;        // 64
    static constexpr int kTableSize     = kBlepLen * kOversampling;  // 4096

    void updatePhaseIncrement();
    void buildTable();
    void insertBLEP(double fracDelay, double amplitude);

    double tickSaw();
    double tickSquare();

    Waveform waveform_;
    double frequency_;
    double sampleRate_;
    double phase_;
    double phaseIncrement_;
    double pulseWidth_;            // Square wave duty cycle (0.0 – 1.0)

    // Integrated windowed sinc [0..1], plus one guard point for interpolation
    double blepTable_[kTableSize + 1];

    // Ring buffer for accumulated BLEP corrections (SEPARATE from naive)
    double corrBuf_[kBlepLen];
    int    corrPos_;

    // Delay line for the naive waveform (linear-phase symmetry requires this)
    double delayLine_[kZeroCrossings];
    // Delay line for the raw phase (needed for high-frequency sine crossfade)
    double phaseDelay_[kZeroCrossings];
    int    delayPos_;
};

}  // namespace dsp
}  // namespace bassline

#endif  // BASSLINE_JUNKIE_SRC_SYNTH_DSP_MINBLEP_OSCILLATOR_H_
