#ifndef SRC_SYNTH_DSP_SOFTCLIPPER_TANHSOFTCLIPPER_H_
#define SRC_SYNTH_DSP_SOFTCLIPPER_TANHSOFTCLIPPER_H_

#include <cmath>

#include "HalfBandIir2x.h"
#include "Oversampler.h"

namespace bassline {
namespace dsp {

template <size_t OversamplingFactor = 8, typename Stage = HalfBandIir2x>
class TanhSoftClipper {
public:
    void reset(double value = 0.0)
    {
        m_oversampler.reset(value);
    }

    double process(double input, double drive)
    {
        return m_oversampler.process(input, [drive](double sample) {
            return std::tanh(sample * drive) / std::tanh(drive);
        });
    }

private:
    Oversampler<OversamplingFactor, Stage> m_oversampler;
};

} // namespace dsp
} // namespace bassline

#endif /* SRC_SYNTH_DSP_SOFTCLIPPER_TANHSOFTCLIPPER_H_ */
