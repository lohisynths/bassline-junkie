#ifndef SRC_SYNTH_DSP_SOFTCLIPPER_OVERSAMPLER_H_
#define SRC_SYNTH_DSP_SOFTCLIPPER_OVERSAMPLER_H_

#include <cstddef>
#include <cmath>

#include "../../config.h"

namespace bassline {
namespace dsp {

template <size_t Factor, typename Filter>
class Oversampler {
    static_assert(Factor > 0, "Oversampling factor must be greater than zero");

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
        const double oversampled_rate = m_base_sample_rate * static_cast<double>(Factor);
        const double cutoff = m_base_sample_rate * 0.45;

        m_pre_filter.setSampleRate(oversampled_rate);
        m_pre_filter.setCutoff(cutoff);
        m_post_filter.setSampleRate(oversampled_rate);
        m_post_filter.setCutoff(cutoff);
    }

    void reset(double value = 0.0)
    {
        m_previous_input = value;
        m_pre_filter.reset(value);
        m_post_filter.reset(value);
    }

    template <typename Processor>
    double process(double input, Processor processor)
    {
        double output = 0.0;

        for (size_t i = 0; i < Factor; ++i) {
            const double t = static_cast<double>(i + 1) / static_cast<double>(Factor);
            const double interpolated = m_previous_input + ((input - m_previous_input) * t);
            const double filtered_input = m_pre_filter.process(interpolated);
            output = m_post_filter.process(processor(filtered_input));
        }

        m_previous_input = input;
        return output;
    }

private:
    double m_base_sample_rate = sample_rate;
    double m_previous_input = 0.0;
    Filter m_pre_filter;
    Filter m_post_filter;
};

} // namespace dsp
} // namespace bassline

#endif /* SRC_SYNTH_DSP_SOFTCLIPPER_OVERSAMPLER_H_ */
