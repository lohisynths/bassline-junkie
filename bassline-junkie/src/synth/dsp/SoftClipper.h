#ifndef SRC_SYNTH_DSP_SOFTCLIPPER_H_
#define SRC_SYNTH_DSP_SOFTCLIPPER_H_

#include <array>
#include <cstddef>
#include <cstdint>

class SoftClipper {
public:
    SoftClipper();

    void setVolumeMidi(uint8_t value);
    double process(double input);
    void reset();

private:
    static constexpr std::size_t kOversampleFactor = 4;
    static constexpr std::size_t kFirTaps = 32;
    static constexpr std::size_t kPhaseTaps = kFirTaps / kOversampleFactor;
    static constexpr double kCutoffCycles = 0.11;
    static constexpr double kDriveSmoothing = 1.0 / (64.0 * kOversampleFactor);

    static double calculateTargetOutputGain(double drive);
    void pushInputSample(double input);
    void pushShapedSample(double input);
    double interpolatePhase(std::size_t phase) const;
    double decimate() const;
    void initialiseFilters();

    bool enabled_ = false;
    double current_drive_ = 1.0;
    double target_drive_ = 1.0;
    double current_output_gain_ = 1.0;
    double target_output_gain_ = 1.0;
    double current_level_gain_ = 1.0;
    double dry_level_ = 0.0;
    double wet_level_ = 0.0;
    double previous_input_ = 0.0;
    uint8_t last_volume_midi_ = 0;

    std::array<double, kPhaseTaps> input_history_{};
    std::array<double, kFirTaps> shaped_history_{};
    std::array<std::array<double, kPhaseTaps>, kOversampleFactor> upsample_taps_{};
    std::array<double, kFirTaps> decimate_taps_{};
};

#endif /* SRC_SYNTH_DSP_SOFTCLIPPER_H_ */
