#include "SoftClipper.h"
#include "fast_trig.h"

#include <algorithm>
#include <cmath>
#include <numeric>

namespace {

constexpr double kPi = 3.14159265358979323846;
constexpr std::size_t kSoftClipperFirTaps = 32;
constexpr std::size_t kCompensationSamples = 2048;
constexpr double kReferenceInputRms = 0.70710678118654752440;
constexpr double kLevelAttack = 0.010362670322278;
constexpr double kLevelRelease = 0.000260382444765;
constexpr double kLevelEpsilon = 1e-9;
constexpr uint8_t kClipStartMidi = 97;
constexpr std::size_t kClipTableSize = 127 - kClipStartMidi + 1;

double sinc(double x)
{
    if (std::abs(x) < 1e-12) {
        return 1.0;
    }
    return bassline::math::sin(kPi * x) / (kPi * x);
}

std::array<double, kSoftClipperFirTaps> makePrototype(double cutoff_cycles)
{
    std::array<double, kSoftClipperFirTaps> taps{};
    const double midpoint = 0.5 * static_cast<double>(kSoftClipperFirTaps - 1);

    for (size_t index = 0; index < taps.size(); ++index) {
        const double centered = static_cast<double>(index) - midpoint;
        const double phase = (2.0 * kPi * static_cast<double>(index))
                           / static_cast<double>(kSoftClipperFirTaps - 1);
        const double window = 0.42 - 0.5 * bassline::math::cos(phase) + 0.08 * bassline::math::cos(2.0 * phase);
        taps[index] = (2.0 * cutoff_cycles) * sinc(2.0 * cutoff_cycles * centered) * window;
    }

    return taps;
}

double normaliseWetSample(double input, double drive)
{
    const double tanh_drive = std::tanh(drive);
    if (std::abs(tanh_drive) < 1e-12) {
        return input;
    }
    return std::tanh(drive * input) / tanh_drive;
}

void updateEnvelope(double &envelope, double value)
{
    const double coefficient = (value > envelope) ? kLevelAttack : kLevelRelease;
    envelope += (value - envelope) * coefficient;
}

std::array<double, kClipTableSize> makeOutputGainTable()
{
    std::array<double, kClipTableSize> table{};

    for (std::size_t index = 0; index < table.size(); ++index) {
        const uint8_t midi_value = static_cast<uint8_t>(kClipStartMidi + index);
        const double clip_t = std::max(0.0, std::min(1.0, (static_cast<double>(midi_value) - 96.0) / 31.0));
        const double drive = 1.0 + 8.0 * clip_t * clip_t;

        double squared_sum = 0.0;
        for (std::size_t sample_index = 0; sample_index < kCompensationSamples; ++sample_index) {
            const double phase = (2.0 * kPi * static_cast<double>(sample_index))
                               / static_cast<double>(kCompensationSamples);
            const double input = bassline::math::sin(phase);
            const double shaped = normaliseWetSample(input, drive);
            squared_sum += shaped * shaped;
        }

        const double wet_rms = std::sqrt(squared_sum / static_cast<double>(kCompensationSamples));
        table[index] = (wet_rms > 1e-12) ? (kReferenceInputRms / wet_rms) : 1.0;
    }

    return table;
}

const std::array<double, kClipTableSize>& getOutputGainTable()
{
    static const std::array<double, kClipTableSize> table = makeOutputGainTable();
    return table;
}

}  // namespace

SoftClipper::SoftClipper()
{
    (void)getOutputGainTable();
    initialiseFilters();
    reset();
}

void SoftClipper::setVolumeMidi(uint8_t value)
{
    if (value == last_volume_midi_) {
        return;
    }
    last_volume_midi_ = value;

    if (value <= 96) {
        enabled_ = false;
        target_drive_ = 1.0;
        current_drive_ = 1.0;
        target_output_gain_ = 1.0;
        current_output_gain_ = 1.0;
        current_level_gain_ = 1.0;
        dry_level_ = 0.0;
        wet_level_ = 0.0;
        return;
    }

    const double clip_t = std::max(0.0, std::min(1.0, (static_cast<double>(value) - 96.0) / 31.0));
    enabled_ = true;
    target_drive_ = 1.0 + 8.0 * clip_t * clip_t;
    target_output_gain_ = getOutputGainTable()[value - kClipStartMidi];
}

double SoftClipper::process(double input)
{
    previous_input_ = input;
    pushInputSample(input);

    for (size_t phase = 0; phase < kOversampleFactor; ++phase) {
        const double oversampled = interpolatePhase(phase);

        double shaped = oversampled;
        if (enabled_) {
            current_drive_ += (target_drive_ - current_drive_) * kDriveSmoothing;
            current_output_gain_ += (target_output_gain_ - current_output_gain_) * kDriveSmoothing;
            shaped = normaliseWetSample(oversampled, current_drive_);
        }

        pushShapedSample(shaped);
    }

    if (!enabled_) {
        return input;
    }

    const double wet_output = decimate() * current_output_gain_;

    updateEnvelope(dry_level_, input * input);
    updateEnvelope(wet_level_, wet_output * wet_output);

    const double target_level_gain = std::min(1.0, std::sqrt((dry_level_ + kLevelEpsilon) / (wet_level_ + kLevelEpsilon)));
    const double gain_coefficient = (target_level_gain < current_level_gain_) ? kLevelAttack : kLevelRelease;
    current_level_gain_ += (target_level_gain - current_level_gain_) * gain_coefficient;

    return wet_output * current_level_gain_;
}

void SoftClipper::reset()
{
    previous_input_ = 0.0;
    current_drive_ = enabled_ ? target_drive_ : 1.0;
    current_output_gain_ = enabled_ ? target_output_gain_ : 1.0;
    current_level_gain_ = 1.0;
    dry_level_ = 0.0;
    wet_level_ = 0.0;
    std::fill(input_history_.begin(), input_history_.end(), 0.0);
    std::fill(shaped_history_.begin(), shaped_history_.end(), 0.0);
}

double SoftClipper::calculateTargetOutputGain(double drive)
{
    double squared_sum = 0.0;

    for (std::size_t index = 0; index < kCompensationSamples; ++index) {
        const double phase = (2.0 * kPi * static_cast<double>(index)) / static_cast<double>(kCompensationSamples);
        const double input = bassline::math::sin(phase);
        const double shaped = normaliseWetSample(input, drive);
        squared_sum += shaped * shaped;
    }

    const double wet_rms = std::sqrt(squared_sum / static_cast<double>(kCompensationSamples));
    if (wet_rms < 1e-12) {
        return 1.0;
    }

    return kReferenceInputRms / wet_rms;
}

void SoftClipper::pushInputSample(double input)
{
    std::move(input_history_.begin() + 1, input_history_.end(), input_history_.begin());
    input_history_.back() = input;
}

void SoftClipper::pushShapedSample(double input)
{
    std::move(shaped_history_.begin() + 1, shaped_history_.end(), shaped_history_.begin());
    shaped_history_.back() = input;
}

double SoftClipper::interpolatePhase(std::size_t phase) const
{
    double output = 0.0;
    for (size_t tap = 0; tap < kPhaseTaps; ++tap) {
        output += upsample_taps_[phase][tap] * input_history_[kPhaseTaps - 1 - tap];
    }
    return output;
}

double SoftClipper::decimate() const
{
    double output = 0.0;
    for (size_t tap = 0; tap < kFirTaps; ++tap) {
        output += decimate_taps_[tap] * shaped_history_[kFirTaps - 1 - tap];
    }
    return output;
}

void SoftClipper::initialiseFilters()
{
    const auto prototype = makePrototype(kCutoffCycles);

    for (size_t phase = 0; phase < kOversampleFactor; ++phase) {
        double phase_sum = 0.0;
        for (size_t tap = 0; tap < kPhaseTaps; ++tap) {
            const double coeff = prototype[phase + tap * kOversampleFactor];
            upsample_taps_[phase][tap] = coeff;
            phase_sum += coeff;
        }

        if (std::abs(phase_sum) > 1e-12) {
            for (double &coeff : upsample_taps_[phase]) {
                coeff /= phase_sum;
            }
        }
    }

    decimate_taps_ = prototype;
    const double full_sum = std::accumulate(decimate_taps_.begin(), decimate_taps_.end(), 0.0);
    if (std::abs(full_sum) > 1e-12) {
        for (double &coeff : decimate_taps_) {
            coeff /= full_sum;
        }
    }
}
