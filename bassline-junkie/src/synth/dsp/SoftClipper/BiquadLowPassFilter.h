#ifndef SRC_SYNTH_DSP_SOFTCLIPPER_BIQUADLOWPASSFILTER_H_
#define SRC_SYNTH_DSP_SOFTCLIPPER_BIQUADLOWPASSFILTER_H_

#include <cmath>

namespace bassline {
namespace dsp {

class BiquadLowPassFilter {
public:
    BiquadLowPassFilter()
    {
        updateCoefficients();
        reset();
    }

    void setSampleRate(double sampleRate)
    {
        if (!std::isfinite(sampleRate) || sampleRate <= 0.0) {
            sampleRate = 48000.0;
        }

        m_sample_rate = sampleRate;
        updateCoefficients();
    }

    void setCutoff(double cutoffHz)
    {
        m_cutoff = cutoffHz;
        updateCoefficients();
    }

    void reset(double value = 0.0)
    {
        m_x1 = value;
        m_x2 = value;
        m_y1 = value;
        m_y2 = value;
    }

    double process(double input)
    {
        const double output = (m_b0 * input) + (m_b1 * m_x1) + (m_b2 * m_x2)
            - (m_a1 * m_y1) - (m_a2 * m_y2);

        m_x2 = m_x1;
        m_x1 = input;
        m_y2 = m_y1;
        m_y1 = output;

        return output;
    }

private:
    void updateCoefficients()
    {
        if (!std::isfinite(m_cutoff)) {
            m_cutoff = 1000.0;
        }

        const double min_cutoff = 1.0;
        const double max_cutoff = (m_sample_rate * 0.5) * 0.98;

        if (m_cutoff < min_cutoff) {
            m_cutoff = min_cutoff;
        } else if (m_cutoff > max_cutoff) {
            m_cutoff = max_cutoff;
        }

        const double pi = 3.14159265358979323846;
        const double omega = 2.0 * pi * m_cutoff / m_sample_rate;
        const double cos_omega = std::cos(omega);
        const double sin_omega = std::sin(omega);
        const double alpha = sin_omega / (2.0 * m_q);

        const double a0 = 1.0 + alpha;
        const double b0 = (1.0 - cos_omega) * 0.5;
        const double b1 = 1.0 - cos_omega;
        const double b2 = (1.0 - cos_omega) * 0.5;
        const double a1 = -2.0 * cos_omega;
        const double a2 = 1.0 - alpha;

        m_b0 = b0 / a0;
        m_b1 = b1 / a0;
        m_b2 = b2 / a0;
        m_a1 = a1 / a0;
        m_a2 = a2 / a0;
    }

    double m_sample_rate = 48000.0;
    double m_cutoff = 1000.0;
    const double m_q = 0.70710678;

    double m_b0 = 1.0;
    double m_b1 = 0.0;
    double m_b2 = 0.0;
    double m_a1 = 0.0;
    double m_a2 = 0.0;

    double m_x1 = 0.0;
    double m_x2 = 0.0;
    double m_y1 = 0.0;
    double m_y2 = 0.0;
};

} // namespace dsp
} // namespace bassline

#endif /* SRC_SYNTH_DSP_SOFTCLIPPER_BIQUADLOWPASSFILTER_H_ */
