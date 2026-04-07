#include "SineWave.h"

#include "fast_trig.h"

namespace bassline {
namespace dsp {

namespace {
const double kTwoPi = 6.28318530717958647692;
}

std::array<double, SineWave::kTableSize + 1> SineWave::table_;
bool SineWave::tableInitialized_ = false;

SineWave::SineWave()
    : time_(0.0),
      rate_(1.0),
      phaseOffset_(0.0),
      sampleRate_(44100.0) {
    initTable();
}

void SineWave::reset() {
    time_ = 0.0;
}

void SineWave::setRate(double rate) {
    rate_ = rate;
}

void SineWave::setFrequency(double frequency) {
    setRate(static_cast<double>(kTableSize) * frequency / sampleRate_);
}

void SineWave::addTime(double time) {
    time_ += time;
}

void SineWave::addPhase(double phase) {
    time_ += static_cast<double>(kTableSize) * phase;
}

void SineWave::addPhaseOffset(double phaseOffset) {
    time_ += (phaseOffset - phaseOffset_) * static_cast<double>(kTableSize);
    phaseOffset_ = phaseOffset;
}

void SineWave::setSampleRate(double sampleRate) {
    if (sampleRate <= 0.0) {
        return;
    }

    rate_ = sampleRate_ * rate_ / sampleRate;
    sampleRate_ = sampleRate;
}

double SineWave::tick() {
    while (time_ < 0.0) {
        time_ += static_cast<double>(kTableSize);
    }
    while (time_ >= static_cast<double>(kTableSize)) {
        time_ -= static_cast<double>(kTableSize);
    }

    const unsigned int index = static_cast<unsigned int>(time_);
    const double alpha = time_ - static_cast<double>(index);
    double tmp = table_[index];
    tmp += alpha * (table_[index + 1] - tmp);

    time_ += rate_;
    return tmp;
}

void SineWave::initTable() {
    if (tableInitialized_) {
        return;
    }

    const double step = 1.0 / static_cast<double>(kTableSize);
    for (unsigned long i = 0; i <= kTableSize; ++i) {
        table_[i] = bassline::math::sin(kTwoPi * static_cast<double>(i) * step);
    }

    tableInitialized_ = true;
}

}  // namespace dsp
}  // namespace bassline
