#include "BlitSquare.h"

#include <limits>

#include "fast_trig.h"

namespace bassline {
namespace dsp {

namespace {
const double kPi = 3.14159265358979323846;
const double kTwoPi = 2.0 * kPi;
}

BlitSquare::BlitSquare(double frequency)
    : nHarmonics_(0),
      m_(2),
      frequency_(220.0),
      sampleRate_(44100.0),
      rate_(0.0),
      phase_(0.0),
      p_(0.0),
      a_(0.0),
      lastBlitOutput_(0.0),
      dcbState_(0.0),
      lastOutput_(0.0) {
    setFrequency(frequency);
    reset();
}

void BlitSquare::reset() {
    phase_ = 0.0;
    lastOutput_ = 0.0;
    dcbState_ = 0.0;
    lastBlitOutput_ = 0.0;
}

void BlitSquare::setPhase(double normalizedPhase) {
    const double wrapped = normalizedPhase - bassline::math::floor(normalizedPhase);
    phase_ = kPi * wrapped;
}

double BlitSquare::getPhase() const {
    return phase_ / kPi;
}

void BlitSquare::setFrequency(double frequency) {
    if (frequency <= 0.0) {
        return;
    }

    frequency_ = frequency;
    updateFrequencyState();
}

void BlitSquare::setHarmonics(unsigned int nHarmonics) {
    nHarmonics_ = nHarmonics;
    updateHarmonics();
}

void BlitSquare::setSampleRate(double sampleRate) {
    if (sampleRate <= 0.0) {
        return;
    }

    sampleRate_ = sampleRate;
    updateFrequencyState();
}

double BlitSquare::tick() {
    const double temp = lastBlitOutput_;
    const double denominator = bassline::math::sin(phase_);
    if (bassline::math::fabs(denominator) < std::numeric_limits<double>::epsilon()) {
        if (phase_ < 0.1 || phase_ > kTwoPi - 0.1) {
            lastBlitOutput_ = a_;
        } else {
            lastBlitOutput_ = -a_;
        }
    } else {
        lastBlitOutput_ = bassline::math::sin(static_cast<double>(m_) * phase_);
        lastBlitOutput_ /= p_ * denominator;
    }

    lastBlitOutput_ += temp;
    lastOutput_ = lastBlitOutput_ - dcbState_ + 0.999 * lastOutput_;
    dcbState_ = lastBlitOutput_;

    phase_ += rate_;
    while (phase_ >= kTwoPi) {
        phase_ -= kTwoPi;
    }

    return lastOutput_;
}

void BlitSquare::updateFrequencyState() {
    p_ = 0.5 * sampleRate_ / frequency_;
    rate_ = kPi / p_;
    updateHarmonics();
}

void BlitSquare::updateHarmonics() {
    if (nHarmonics_ == 0) {
        const unsigned int maxHarmonics =
            static_cast<unsigned int>(bassline::math::floor(0.5 * p_));
        m_ = 2 * (maxHarmonics + 1);
    } else {
        m_ = 2 * (nHarmonics_ + 1);
    }

    a_ = static_cast<double>(m_) / p_;
}

}  // namespace dsp
}  // namespace bassline
