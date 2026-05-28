#ifndef SRC_SYNTH_DSP_SOFTCLIPPER_TANHADAASOFTCLIPPER_H_
#define SRC_SYNTH_DSP_SOFTCLIPPER_TANHADAASOFTCLIPPER_H_

#include <cmath>

#include "Oversampler.h"

namespace bassline {
namespace dsp {

template <size_t OversamplingFactor = 8>
class TanhAdaaSoftClipper {
public:
    void reset(double value = 0.0)
    {
        m_previous_input = value;
        m_oversampler.reset(value);
    }

    double process(double input, double drive)
    {
        return m_oversampler.process(input, [this, drive](double sample) {
            return processAdaa(sample, drive);
        });
    }

private:
    static double tanhNormalized(double input, double drive)
    {
        return std::tanh(input * drive) / std::tanh(drive);
    }

    static double logCosh(double input)
    {
        const double abs_input = std::fabs(input);
        const double log_two = 0.69314718055994530942;
        return abs_input + std::log1p(std::exp(-2.0 * abs_input)) - log_two;
    }

    static double tanhAntiderivative(double input, double drive)
    {
        return logCosh(input * drive) / (drive * std::tanh(drive));
    }

    double processAdaa(double input, double drive)
    {
        const double diff = input - m_previous_input;
        double output;

        if (std::fabs(diff) < 1.0e-9) {
            output = tanhNormalized(input, drive);
        } else {
            output = (tanhAntiderivative(input, drive)
                - tanhAntiderivative(m_previous_input, drive)) / diff;
        }

        m_previous_input = input;
        return output;
    }

    double m_previous_input = 0.0;
    Oversampler<OversamplingFactor> m_oversampler;
};

} // namespace dsp
} // namespace bassline

#endif /* SRC_SYNTH_DSP_SOFTCLIPPER_TANHADAASOFTCLIPPER_H_ */
