#include "MinBLEPOscillator.h"
#include "fast_trig.h"

#include <cstring>

namespace bassline {
namespace dsp {

namespace {

constexpr double kPi    = 3.14159265358979323846;
constexpr double kTwoPi = 6.28318530717958647692;

}  // anonymous namespace

// ===========================================================================
// Table construction
// ===========================================================================

void MinBLEPOscillator::buildTable() {
    const int    N         = kTableSize;   // 4096
    const double bandlimit = 0.9;
    const double halfWidth = static_cast<double>(kZeroCrossings) * bandlimit;

    // Build a Blackman-Harris-windowed sinc and integrate (cumulative sum)
    // to produce the band-limited step function.
    double runSum = 0.0;
    for (int i = 0; i < N; ++i) {
        // t maps [0, N-1] -> [0.0, 1.0]
        const double t = static_cast<double>(i) / static_cast<double>(N - 1);

        // x maps [0.0, 1.0] -> [-halfWidth, +halfWidth]  (sinc domain)
        const double x = halfWidth * (2.0 * t - 1.0);

        // Normalised sinc: sin(pi*x) / (pi*x)
        const double sinc = (bassline::math::fabs(x) < 1e-12)
            ? 1.0
            : bassline::math::sin(kPi * x) / (kPi * x);

        // 4-term Blackman-Harris window: -92 dB sidelobe attenuation
        const double w = 0.35875
            - 0.48829 * bassline::math::cos(kTwoPi * t)
            + 0.14128 * bassline::math::cos(2.0 * kTwoPi * t)
            - 0.01168 * bassline::math::cos(3.0 * kTwoPi * t);

        runSum += sinc * w;
        blepTable_[i] = runSum;
    }

    // Normalise so the step goes from 0.0 to 1.0
    const double norm = blepTable_[N - 1];
    if (bassline::math::fabs(norm) > 1e-10) {
        const double invNorm = 1.0 / norm;
        for (int i = 0; i < N; ++i)
            blepTable_[i] *= invNorm;
    }

    // Guard point at index N for safe interpolation at the last entry
    blepTable_[N] = 1.0;
}

// ===========================================================================
// Lifecycle
// ===========================================================================

MinBLEPOscillator::MinBLEPOscillator(double frequency)
    : waveform_(SAW),
      frequency_(frequency),
      sampleRate_(44100.0),
      phase_(0.0),
      phaseIncrement_(0.0),
      pulseWidth_(0.5),
      corrPos_(0),
      delayPos_(0)
{
    std::memset(corrBuf_,    0, sizeof(corrBuf_));
    std::memset(delayLine_,  0, sizeof(delayLine_));
    std::memset(phaseDelay_, 0, sizeof(phaseDelay_));
    buildTable();
    updatePhaseIncrement();
}

void MinBLEPOscillator::reset() {
    phase_    = 0.0;
    corrPos_  = 0;
    delayPos_ = 0;
    std::memset(corrBuf_,    0, sizeof(corrBuf_));
    std::memset(delayLine_,  0, sizeof(delayLine_));
    std::memset(phaseDelay_, 0, sizeof(phaseDelay_));
}

void MinBLEPOscillator::setFrequency(double frequency) {
    if (frequency > 0.0) {
        frequency_ = frequency;
        updatePhaseIncrement();
    }
}

void MinBLEPOscillator::setSampleRate(double sampleRate) {
    if (sampleRate > 0.0) {
        sampleRate_ = sampleRate;
        updatePhaseIncrement();
    }
}

void MinBLEPOscillator::setWaveform(Waveform waveform) {
    waveform_ = waveform;
}

void MinBLEPOscillator::setPulseWidth(double pw) {
    if (pw < 0.01) pw = 0.01;
    if (pw > 0.99) pw = 0.99;
    pulseWidth_ = pw;
}

void MinBLEPOscillator::updatePhaseIncrement() {
    phaseIncrement_ = frequency_ / sampleRate_;
}

// ===========================================================================
// BLEP insertion
// ===========================================================================

void MinBLEPOscillator::insertBLEP(double fracDelay, double amplitude) {
    // fracDelay in [0, 1): fractional sample elapsed AFTER the discontinuity.
    //
    // The correction at output buffer position i (0 <= i < kBlepLen) is:
    //
    //   amplitude * ( blepTable(tablePos) - heaviside(i - kZeroCrossings) )
    //
    // For i <  kZeroCrossings (before step centre): heaviside = 0
    // For i >= kZeroCrossings (at/after step centre): heaviside = 1

    const double tOff = fracDelay * static_cast<double>(kOversampling);
    const int    iOff = static_cast<int>(tOff);
    const double frac = tOff - static_cast<double>(iOff);

    for (int i = 0; i < kBlepLen; ++i) {
        const int idx = iOff + i * kOversampling;

        double val;
        if (idx >= kTableSize) {
            val = 1.0;
        } else {
            // Linear interpolation (guard point at kTableSize ensures idx+1 is valid)
            val = blepTable_[idx] + frac * (blepTable_[idx + 1] - blepTable_[idx]);
        }

        const double h = (i < kZeroCrossings) ? 0.0 : 1.0;
        corrBuf_[(corrPos_ + i) % kBlepLen] += amplitude * (val - h);
    }
}

// ===========================================================================
// Per-sample processing
// ===========================================================================

double MinBLEPOscillator::tickSaw() {
    // 1. Advance phase
    phase_ += phaseIncrement_;

    // 2. Detect sawtooth reset (phase wrap at 1.0)
    if (phase_ >= 1.0) {
        phase_ -= 1.0;
        const double fracDelay = phase_ / phaseIncrement_;
        // Sawtooth drops from +1 to -1 => step amplitude = -2.0
        insertBLEP(fracDelay, -2.0);
    }

    // 3. Naive sawtooth for the CURRENT sample: ramp from -1 to +1
    const double naive = 2.0 * phase_ - 1.0;

    // 4. Read accumulated BLEP correction for this output slot, then clear
    const double correction = corrBuf_[corrPos_];
    corrBuf_[corrPos_] = 0.0;
    corrPos_ = (corrPos_ + 1) % kBlepLen;

    // 5. Delay the naive waveform and phase by kZeroCrossings samples
    const double delayed      = delayLine_[delayPos_];
    const double delayedPhase = phaseDelay_[delayPos_];
    delayLine_[delayPos_]  = naive;
    phaseDelay_[delayPos_] = phase_;
    delayPos_ = (delayPos_ + 1) % kZeroCrossings;

    // 6. High-frequency crossfade to sine.
    //    Saw 2nd harmonic hits Nyquist at freq = sampleRate/4.
    //    Start blending at sampleRate/6, pure sine at sampleRate/4.
    const double ratio = frequency_ / sampleRate_;
    constexpr double kBlendStart = 1.0 / 6.0;   // ~7.4 kHz @ 44.1 kHz
    constexpr double kBlendEnd   = 1.0 / 4.0;   // ~11 kHz  @ 44.1 kHz

    if (ratio >= kBlendEnd) {
        return bassline::math::sin(kTwoPi * delayedPhase);
    }

    const double output = delayed + correction;

    if (ratio > kBlendStart) {
        double t = (ratio - kBlendStart) / (kBlendEnd - kBlendStart);
        t = t * t * (3.0 - 2.0 * t);  // smoothstep
        const double sine = bassline::math::sin(kTwoPi * delayedPhase);
        return output * (1.0 - t) + sine * t;
    }

    return output;
}

double MinBLEPOscillator::tickSquare() {
    // The square wave has two discontinuities per cycle:
    //   - Rising edge at phase = 0.0  (wrap):        -1 -> +1  (step = +2)
    //   - Falling edge at phase = pulseWidth_:        +1 -> -1  (step = -2)

    phase_ += phaseIncrement_;

    // --- Rising edge: phase wraps past 1.0 ---
    if (phase_ >= 1.0) {
        phase_ -= 1.0;
        const double fracDelay = phase_ / phaseIncrement_;
        insertBLEP(fracDelay, 2.0);   // step from -1 to +1
    }

    // --- Falling edge: phase crosses pulseWidth_ ---
    // Compute the phase value just before this sample's increment.
    // If a wrap occurred, prevPhase was near 1.0 (above pulseWidth_),
    // so the crossing check correctly returns false.
    double effectivePrev = phase_ - phaseIncrement_;
    if (effectivePrev < 0.0) effectivePrev += 1.0;

    if (effectivePrev < pulseWidth_ && phase_ >= pulseWidth_) {
        const double overshoot = phase_ - pulseWidth_;
        const double fracDelay = overshoot / phaseIncrement_;
        insertBLEP(fracDelay, -2.0);  // step from +1 to -1
    }

    // Naive square: +1 in first half, -1 in second half
    const double naive = (phase_ < pulseWidth_) ? 1.0 : -1.0;

    // Read accumulated BLEP correction for this output slot, then clear
    const double correction = corrBuf_[corrPos_];
    corrBuf_[corrPos_] = 0.0;
    corrPos_ = (corrPos_ + 1) % kBlepLen;

    // Delay the naive waveform and phase by kZeroCrossings samples
    const double delayed      = delayLine_[delayPos_];
    const double delayedPhase = phaseDelay_[delayPos_];
    delayLine_[delayPos_]  = naive;
    phaseDelay_[delayPos_] = phase_;
    delayPos_ = (delayPos_ + 1) % kZeroCrossings;

    // High-frequency crossfade to sine.
    //    Square 3rd harmonic hits Nyquist at freq = sampleRate/6.
    //    Start blending at sampleRate/8, pure sine at sampleRate/6.
    const double ratio = frequency_ / sampleRate_;
    constexpr double kBlendStart = 1.0 / 8.0;   // ~5.5 kHz @ 44.1 kHz
    constexpr double kBlendEnd   = 1.0 / 6.0;   // ~7.4 kHz @ 44.1 kHz

    if (ratio >= kBlendEnd) {
        return bassline::math::sin(kTwoPi * delayedPhase);
    }

    const double output = delayed + correction;

    if (ratio > kBlendStart) {
        double t = (ratio - kBlendStart) / (kBlendEnd - kBlendStart);
        t = t * t * (3.0 - 2.0 * t);  // smoothstep
        const double sine = bassline::math::sin(kTwoPi * delayedPhase);
        return output * (1.0 - t) + sine * t;
    }

    return output;
}

double MinBLEPOscillator::tick() {
    switch (waveform_) {
        case SQUARE: return tickSquare();
        default:     return tickSaw();
    }
}

}  // namespace dsp
}  // namespace bassline
