#include "WavetableOscillator.h"

#include <algorithm>
#include <cmath>
#include <limits>

#include "../config.h"

namespace bassline {
namespace dsp {

namespace {

const double kPi = 3.14159265358979323846;

bool isOddOnlyWaveform(WavetableOscillator::Waveform waveform) {
    return waveform == WavetableOscillator::TRIANGLE
        || waveform == WavetableOscillator::SQUARE;
}

void addHarmonic(std::vector<double>& waveform, unsigned int harmonic, double amplitude) {
    const double angleStep =
        2.0 * kPi * static_cast<double>(harmonic)
        / static_cast<double>(WavetableOscillator::kTableSize);
    const double sinStep = std::sin(angleStep);
    const double cosStep = std::cos(angleStep);

    double sine = 0.0;
    double cosine = 1.0;

    for (std::size_t i = 0; i < waveform.size(); ++i) {
        waveform[i] += amplitude * sine;

        const double nextSine = sine * cosStep + cosine * sinStep;
        const double nextCosine = cosine * cosStep - sine * sinStep;
        sine = nextSine;
        cosine = nextCosine;
    }
}

void removeDc(std::vector<double>& waveform) {
    if (waveform.empty()) {
        return;
    }

    double sum = 0.0;
    for (std::size_t i = 0; i < waveform.size(); ++i) {
        sum += waveform[i];
    }

    const double mean = sum / static_cast<double>(waveform.size());

    for (std::size_t i = 0; i < waveform.size(); ++i) {
        waveform[i] -= mean;
    }
}

std::vector<float> buildIntegratedTable(const std::vector<double>& waveform) {
    std::vector<float> integrated(WavetableOscillator::kTableSize + 1u, 0.0f);

    const double scale = 1.0 / static_cast<double>(WavetableOscillator::kTableSize);
    double cumulative = 0.0;
    for (std::size_t i = 0; i < waveform.size(); ++i) {
        integrated[i] = static_cast<float>(cumulative);
        cumulative += waveform[i] * scale;
    }
    integrated[WavetableOscillator::kTableSize] = static_cast<float>(cumulative);

    const double drift = cumulative;
    if (std::fabs(drift) > std::numeric_limits<double>::epsilon()) {
        const double driftPerSample =
            drift / static_cast<double>(WavetableOscillator::kTableSize);
        for (std::size_t i = 0; i <= WavetableOscillator::kTableSize; ++i) {
            integrated[i] = static_cast<float>(
                static_cast<double>(integrated[i]) - driftPerSample * static_cast<double>(i));
        }
    }

    integrated[WavetableOscillator::kTableSize] = integrated[0];
    return integrated;
}

}  // namespace

WavetableOscillator::Bank WavetableOscillator::bank_ = Bank();
bool WavetableOscillator::bankInitialized_ = false;

WavetableOscillator::WavetableOscillator(double frequency)
    : waveform_(SAW),
      frequency_(frequency),
      sampleRate_(sample_rate),
      phase_(0.0),
      phaseIncrement_(0.0),
      phaseIncrementInv_(0.0),
      primaryTableIndex_(0u),
      secondaryTableIndex_(0u),
      primaryTableWeight_(1.0),
      secondaryTableWeight_(0.0),
      primaryIntegrated_(NULL),
      secondaryIntegrated_(NULL),
      blended_(false),
      previousIntegrated_(0.0) {
    ensureBank(sampleRate_);
    updatePhaseIncrement();
    updateTableIndex();
    reset();
}

void WavetableOscillator::reset() {
    phase_ = 0.0;
    updateTableIndex();
    syncIntegratedState();
}

void WavetableOscillator::setFrequency(double frequency) {
    if (frequency <= 0.0) {
        return;
    }

    if (frequency == frequency_) {
        return;
    }

    frequency_ = frequency;
    updatePhaseIncrement();
    updateTableIndex();
}

void WavetableOscillator::setSampleRate(double sampleRate) {
    if (sampleRate <= 0.0) {
        return;
    }

    sampleRate_ = sampleRate;
    ensureBank(sampleRate_);
    updatePhaseIncrement();
    updateTableIndex();
    syncIntegratedState();
}

void WavetableOscillator::setWaveform(Waveform waveform) {
    waveform_ = waveform;
    updateTableIndex();
    syncIntegratedState();
}

double WavetableOscillator::tick() {
    if (phaseIncrement_ <= 0.0) {
        return 0.0;
    }

    phase_ += phaseIncrement_;
    if (phase_ >= 1.0) {
        phase_ -= 1.0;
    }

    const double current = lookupCurrentIntegrated(phase_);
    const double output = (current - previousIntegrated_) * phaseIncrementInv_;
    previousIntegrated_ = current;

    return output;
}

void WavetableOscillator::ensureBank(double sampleRate) {
    if (bankInitialized_ && std::fabs(bank_.sampleRate - sampleRate) < 1e-9) {
        return;
    }

    bank_.sampleRate = sampleRate;
    for (std::size_t i = 0; i < bank_.tables.size(); ++i) {
        bank_.tables[i] = buildTables(static_cast<Waveform>(i), sampleRate);
    }

    bankInitialized_ = true;
}

std::vector<WavetableOscillator::Table> WavetableOscillator::buildTables(
        Waveform waveform,
        double sampleRate) {
    std::vector<Table> tables;
    const double nyquist = 0.5 * sampleRate;
    const bool oddOnly = isOddOnlyWaveform(waveform);
    const unsigned int maxHarmonic = static_cast<unsigned int>(kTableSize / 2u - 1u);
    tables.reserve(oddOnly ? (maxHarmonic + 1u) / 2u : maxHarmonic);

    std::vector<double> samples(kTableSize, 0.0);
    for (unsigned int harmonic = 1u; harmonic <= maxHarmonic; ++harmonic) {
        if (oddOnly && (harmonic % 2u == 0u)) {
            continue;
        }

        const double amplitude = [&]() {
            switch (waveform) {
                case SINE:
                    return harmonic == 1u ? 1.0 : 0.0;
                case SAW:
                    return -(2.0 / kPi) / static_cast<double>(harmonic);
                case TRIANGLE: {
                    const unsigned int oddIndex = (harmonic - 1u) / 2u;
                    const double sign = (oddIndex % 2u == 0u) ? 1.0 : -1.0;
                    const double denom =
                        static_cast<double>(harmonic) * static_cast<double>(harmonic);
                    return sign * (8.0 / (kPi * kPi)) / denom;
                }
                case SQUARE:
                    return (4.0 / kPi) / static_cast<double>(harmonic);
            }
            return 0.0;
        }();

        if (std::fabs(amplitude) <= std::numeric_limits<double>::epsilon()) {
            continue;
        }

        addHarmonic(samples, harmonic, amplitude);

        Table table;
        table.maxHarmonic = harmonic;
        table.maxFrequency = nyquist / static_cast<double>(harmonic);
        table.integrated = buildIntegratedTable(samples);
        tables.push_back(table);
    }

    return tables;
}

void WavetableOscillator::buildWaveformSamples(Waveform waveform,
                                               unsigned int maxHarmonic,
                                               std::vector<double>& waveformData) {
    std::fill(waveformData.begin(), waveformData.end(), 0.0);

    switch (waveform) {
        case SINE:
            addHarmonic(waveformData, 1u, 1.0);
            break;
        case SAW:
            for (unsigned int harmonic = 1u; harmonic <= maxHarmonic; ++harmonic) {
                addHarmonic(
                    waveformData,
                    harmonic,
                    -(2.0 / kPi) / static_cast<double>(harmonic));
            }
            break;
        case TRIANGLE:
            for (unsigned int harmonic = 1u; harmonic <= maxHarmonic; harmonic += 2u) {
                const unsigned int oddIndex = (harmonic - 1u) / 2u;
                const double sign = (oddIndex % 2u == 0u) ? 1.0 : -1.0;
                const double denom = static_cast<double>(harmonic) * static_cast<double>(harmonic);
                addHarmonic(waveformData, harmonic, sign * (8.0 / (kPi * kPi)) / denom);
            }
            break;
        case SQUARE:
            for (unsigned int harmonic = 1u; harmonic <= maxHarmonic; harmonic += 2u) {
                addHarmonic(
                    waveformData,
                    harmonic,
                    (4.0 / kPi) / static_cast<double>(harmonic));
            }
            break;
    }
}

double WavetableOscillator::lookupIntegrated(const float* table, double phase) {
    const double index = phase * static_cast<double>(kTableSize);
    const std::size_t baseIndex = static_cast<std::size_t>(index);
    const double frac = index - static_cast<double>(baseIndex);
    const double sample = static_cast<double>(table[baseIndex]);
    return sample
        + frac * (static_cast<double>(table[baseIndex + 1u]) - sample);
}

double WavetableOscillator::lookupCurrentIntegrated(double phase) {
    const double primary = lookupIntegrated(primaryIntegrated_, phase);
    if (!blended_) {
        return primary;
    }

    const double secondary = lookupIntegrated(secondaryIntegrated_, phase);
    return primary * primaryTableWeight_ + secondary * secondaryTableWeight_;
}

void WavetableOscillator::syncIntegratedState() {
    previousIntegrated_ = lookupCurrentIntegrated(phase_);
}

void WavetableOscillator::updatePhaseIncrement() {
    phaseIncrement_ = frequency_ / sampleRate_;
    phaseIncrementInv_ = phaseIncrement_ > 0.0 ? 1.0 / phaseIncrement_ : 0.0;
}

void WavetableOscillator::updateTableIndex() {
    const std::vector<Table>& tables = bank_.tables[waveform_];
    if (tables.empty()) {
        primaryTableIndex_ = 0u;
        secondaryTableIndex_ = 0u;
        primaryTableWeight_ = 1.0;
        secondaryTableWeight_ = 0.0;
        blended_ = false;
        primaryIntegrated_ = NULL;
        secondaryIntegrated_ = NULL;
        previousIntegrated_ = 0.0;
        return;
    }

    if (waveform_ == SINE || frequency_ <= 0.0) {
        primaryTableIndex_ = 0u;
        secondaryTableIndex_ = 0u;
        primaryTableWeight_ = 1.0;
        secondaryTableWeight_ = 0.0;
        blended_ = false;
        updateActiveTables();
        return;
    }

    const double nyquist = 0.5 * sampleRate_;
    unsigned int allowedHarmonic = static_cast<unsigned int>(std::floor(nyquist / frequency_));
    if (allowedHarmonic < 1u) {
        allowedHarmonic = 1u;
    }

    std::size_t richIndex = 0u;
    if (isOddOnlyWaveform(waveform_)) {
        if ((allowedHarmonic % 2u) == 0u) {
            --allowedHarmonic;
        }
        richIndex = static_cast<std::size_t>((allowedHarmonic - 1u) / 2u);
    } else {
        richIndex = static_cast<std::size_t>(allowedHarmonic - 1u);
    }

    if (richIndex >= tables.size()) {
        richIndex = tables.size() - 1u;
    }

    primaryTableIndex_ = richIndex;
    secondaryTableIndex_ = richIndex;
    primaryTableWeight_ = 1.0;
    secondaryTableWeight_ = 0.0;
    blended_ = false;

    if (richIndex == 0u) {
        updateActiveTables();
        return;
    }

    const double richMaxFrequency = tables[richIndex].maxFrequency;
    double fadeStartFrequency = 0.0;
    if (richIndex + 1u < tables.size()) {
        fadeStartFrequency = tables[richIndex + 1u].maxFrequency;
    }

    if (frequency_ <= fadeStartFrequency) {
        updateActiveTables();
        return;
    }

    secondaryTableIndex_ = richIndex - 1u;
    blended_ = true;

    if (frequency_ >= richMaxFrequency) {
        primaryTableWeight_ = 0.0;
        secondaryTableWeight_ = 1.0;
        updateActiveTables();
        return;
    }

    double t = (frequency_ - fadeStartFrequency) / (richMaxFrequency - fadeStartFrequency);
    t = std::max(0.0, std::min(1.0, t));
    t = t * t * (3.0 - 2.0 * t);
    primaryTableWeight_ = 1.0 - t;
    secondaryTableWeight_ = t;
    updateActiveTables();
}

void WavetableOscillator::updateActiveTables() {
    const std::vector<Table>& tables = bank_.tables[waveform_];
    primaryIntegrated_ = tables[primaryTableIndex_].integrated.data();
    secondaryIntegrated_ = tables[secondaryTableIndex_].integrated.data();
    if (primaryTableIndex_ == secondaryTableIndex_) {
        blended_ = false;
        secondaryTableWeight_ = 0.0;
        primaryTableWeight_ = 1.0;
    }
}

}  // namespace dsp
}  // namespace bassline
