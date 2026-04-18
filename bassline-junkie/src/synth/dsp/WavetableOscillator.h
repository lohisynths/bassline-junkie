#ifndef BASSLINE_JUNKIE_SRC_SYNTH_DSP_WAVETABLEOSCILLATOR_H_
#define BASSLINE_JUNKIE_SRC_SYNTH_DSP_WAVETABLEOSCILLATOR_H_

#include <array>
#include <cstddef>
#include <vector>

namespace bassline {
namespace dsp {

// Shared, band-limited wavetable oscillator for classic waveforms.
// Each waveform uses a bank of tables with different harmonic limits and
// reads them through an integrated-wavetable lookup to reduce resampling
// aliasing during playback.
class WavetableOscillator {
public:
    enum Waveform {
        SINE = 0,
        SAW,
        TRIANGLE,
        SQUARE
    };

    explicit WavetableOscillator(double frequency = 220.0);

    void reset();
    void setFrequency(double frequency);
    void setSampleRate(double sampleRate);
    void setWaveform(Waveform waveform);
    double tick();

    static const std::size_t kTableSize = 4096;

private:
    struct Table {
        unsigned int maxHarmonic;
        double maxFrequency;
        std::vector<float> integrated;
    };

    struct Bank {
        double sampleRate;
        std::array<std::vector<Table>, 4> tables;
    };

    static void ensureBank(double sampleRate);
    static std::vector<Table> buildTables(Waveform waveform, double sampleRate);
    static void buildWaveformSamples(Waveform waveformType,
                                     unsigned int maxHarmonic,
                                     std::vector<double>& waveformData);
    static double lookupIntegrated(const float* table, double phase);
    double lookupCurrentIntegrated(double phase);
    void syncIntegratedState();

    void updatePhaseIncrement();
    void updateTableIndex();
    void updateActiveTables();

    static Bank bank_;
    static bool bankInitialized_;

    Waveform waveform_;
    double frequency_;
    double sampleRate_;
    double phase_;
    double phaseIncrement_;
    double phaseIncrementInv_;
    std::size_t primaryTableIndex_;
    std::size_t secondaryTableIndex_;
    double primaryTableWeight_;
    double secondaryTableWeight_;
    const float* primaryIntegrated_;
    const float* secondaryIntegrated_;
    bool blended_;
    double previousIntegrated_;
};

}  // namespace dsp
}  // namespace bassline

#endif  // BASSLINE_JUNKIE_SRC_SYNTH_DSP_WAVETABLEOSCILLATOR_H_
