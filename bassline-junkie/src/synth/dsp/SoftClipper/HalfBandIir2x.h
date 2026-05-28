#ifndef SRC_SYNTH_DSP_SOFTCLIPPER_HALFBANDIIR2X_H_
#define SRC_SYNTH_DSP_SOFTCLIPPER_HALFBANDIIR2X_H_

#include <array>
#include <cstddef>

namespace bassline {
namespace dsp {

class HalfBandIir2x {
public:
    void reset(double value = 0.0)
    {
        m_upsampler.reset(value);
        m_downsampler.reset(value);
    }

    void processUpsample(double input, double& even, double& odd)
    {
        m_upsampler.process(input, input, even, odd);
    }

    double processDownsample(double even, double odd)
    {
        double filtered_even;
        double filtered_odd;
        m_downsampler.process(even, odd, filtered_even, filtered_odd);
        return (filtered_even + filtered_odd) * 0.5;
    }

private:
    static const size_t kSectionCount = 6;

    class PolyphaseEven {
    public:
        void reset(double value)
        {
            for (auto& state : m_state) {
                state[0] = value;
                state[1] = value;
            }
            m_state_last[0] = value;
            m_state_last[1] = value;
        }

        void process(double input_0, double input_1, double& output_0, double& output_1)
        {
            double signal[2] = {input_1, input_0};

            for (size_t i = 0; i < kSectionCount - 1; ++i) {
                const double tmp_0 = ((signal[0] - m_state[i + 1][0]) * coefficient(i, 0))
                    + m_state[i][0];
                const double tmp_1 = ((signal[1] - m_state[i + 1][1]) * coefficient(i, 1))
                    + m_state[i][1];

                m_state[i][0] = signal[0];
                m_state[i][1] = signal[1];
                signal[0] = tmp_0;
                signal[1] = tmp_1;
            }

            const size_t i = kSectionCount - 1;
            const double tmp_0 = ((signal[0] - m_state_last[0]) * coefficient(i, 0))
                + m_state[i][0];
            const double tmp_1 = ((signal[1] - m_state_last[1]) * coefficient(i, 1))
                + m_state[i][1];

            m_state[i][0] = signal[0];
            m_state[i][1] = signal[1];
            signal[0] = tmp_0;
            signal[1] = tmp_1;
            m_state_last[0] = signal[0];
            m_state_last[1] = signal[1];

            output_0 = signal[0];
            output_1 = signal[1];
        }

    private:
        static double coefficient(size_t section, size_t branch)
        {
            static const double coefficients[kSectionCount][2] = {
                {0.036681502163648017, 0.13654762463195794},
                {0.27463175937945444, 0.42313861743656711},
                {0.56109869787919531, 0.67754004997416184},
                {0.76974183386322703, 0.83988962484963892},
                {0.89226081800387902, 0.93154195996318390},
                {0.96209454837808417, 0.98781637073289585}
            };
            return coefficients[section][branch];
        }

        std::array<std::array<double, 2>, kSectionCount> m_state = {};
        std::array<double, 2> m_state_last = {};
    };

    PolyphaseEven m_upsampler;
    PolyphaseEven m_downsampler;
};

} // namespace dsp
} // namespace bassline

#endif /* SRC_SYNTH_DSP_SOFTCLIPPER_HALFBANDIIR2X_H_ */
