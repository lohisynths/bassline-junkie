#ifndef SRC_SYNTH_DSP_SOFTCLIPPER_OVERSAMPLER_H_
#define SRC_SYNTH_DSP_SOFTCLIPPER_OVERSAMPLER_H_

#include <array>
#include <cstddef>
#include <cmath>

#include "HalfBandIir2x.h"
#include "../../config.h"

namespace bassline {
namespace dsp {

template <size_t Factor>
class Oversampler {
    static constexpr bool isPowerOfTwo(size_t value)
    {
        return value != 0 && ((value & (value - 1)) == 0);
    }

    static constexpr size_t stageCount(size_t value)
    {
        return value <= 1 ? 0 : 1 + stageCount(value / 2);
    }

    static_assert(Factor >= 2, "Oversampling factor must be at least 2");
    static_assert(isPowerOfTwo(Factor), "Oversampling factor must be a power of two");

public:
    explicit Oversampler(double baseSampleRate = sample_rate)
    {
        setSampleRate(baseSampleRate);
    }

    void setSampleRate(double baseSampleRate)
    {
        if (!std::isfinite(baseSampleRate) || baseSampleRate <= 0.0) {
            baseSampleRate = sample_rate;
        }

        m_base_sample_rate = baseSampleRate;
    }

    void reset(double value = 0.0)
    {
        for (auto& stage : m_up_stages) {
            stage.reset(value);
        }
        for (auto& stage : m_down_stages) {
            stage.reset(value);
        }
    }

    template <typename Processor>
    double process(double input, Processor processor)
    {
        std::array<double, Factor> samples;
        std::array<double, Factor> scratch;
        samples[0] = input;

        size_t sample_count = 1;
        for (size_t stage = 0; stage < kStageCount; ++stage) {
            for (size_t i = 0; i < sample_count; ++i) {
                double even;
                double odd;
                m_up_stages[stage].processUpsample(samples[i], even, odd);
                scratch[i * 2] = even;
                scratch[(i * 2) + 1] = odd;
            }
            sample_count *= 2;
            for (size_t i = 0; i < sample_count; ++i) {
                samples[i] = scratch[i];
            }
        }

        for (size_t i = 0; i < Factor; ++i) {
            samples[i] = processor(samples[i]);
        }

        for (size_t stage = kStageCount; stage > 0; --stage) {
            HalfBandIir2x& down_stage = m_down_stages[stage - 1];
            const size_t next_sample_count = sample_count / 2;
            for (size_t i = 0; i < next_sample_count; ++i) {
                samples[i] = down_stage.processDownsample(samples[i * 2], samples[(i * 2) + 1]);
            }
            sample_count = next_sample_count;
        }

        return samples[0];
    }

private:
    static const size_t kStageCount = stageCount(Factor);

    double m_base_sample_rate = sample_rate;
    std::array<HalfBandIir2x, kStageCount> m_up_stages;
    std::array<HalfBandIir2x, kStageCount> m_down_stages;
};

} // namespace dsp
} // namespace bassline

#endif /* SRC_SYNTH_DSP_SOFTCLIPPER_OVERSAMPLER_H_ */
