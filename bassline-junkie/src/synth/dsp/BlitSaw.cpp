#include "BlitSaw.h"

#include <limits>

#include "fast_trig.h"

namespace bassline {
namespace dsp {

namespace {
const double kPi = 3.14159265358979323846;
}

BlitSaw::BlitSaw(double frequency)
    : nHarmonics_(0),
      m_(1),
      frequency_(220.0),
      sampleRate_(44100.0),
      rate_(0.0),
      phase_(0.0),
      p_(0.0),
      c2_(0.0),
      a_(0.0),
      state_(0.0) {
    reset();
    setFrequency(frequency);
}

void BlitSaw::reset() {
    phase_ = 0.0;
    state_ = 0.0;
}

void BlitSaw::setFrequency(double frequency) {
    if (frequency <= 0.0) {
        return;
    }

    frequency_ = frequency;
    updateFrequencyState();
}

void BlitSaw::setHarmonics(unsigned int nHarmonics) {
    nHarmonics_ = nHarmonics;
    updateHarmonics();
    state_ = -0.5 * a_;
}

void BlitSaw::setSampleRate(double sampleRate) {
    if (sampleRate <= 0.0) {
        return;
    }

    sampleRate_ = sampleRate;
    updateFrequencyState();
}

double BlitSaw::tick() {
    double tmp;
    const double denominator = bassline::math::sin(phase_);
    if (bassline::math::fabs(denominator) <= std::numeric_limits<double>::epsilon()) {
        tmp = a_;
    } else {
        tmp = bassline::math::sin(static_cast<double>(m_) * phase_);
        tmp /= p_ * denominator;
    }

    tmp += state_ - c2_;
    state_ = tmp * 0.995;

    phase_ += rate_;
    while (phase_ >= kPi) {
        phase_ -= kPi;
    }

    return tmp;
}

void BlitSaw::updateFrequencyState() {
    p_ = sampleRate_ / frequency_;
    c2_ = 1.0 / p_;
    rate_ = kPi * c2_;
    updateHarmonics();
}

void BlitSaw::updateHarmonics() {
    if (nHarmonics_ == 0) {
        const unsigned int maxHarmonics =
            static_cast<unsigned int>(bassline::math::floor(0.5 * p_));
        m_ = 2 * maxHarmonics + 1;
    } else {
        m_ = 2 * nHarmonics_ + 1;
    }

    a_ = static_cast<double>(m_) / p_;
}

}  // namespace dsp
}  // namespace bassline
