#include <array>
#include <string>

#include "dsp/SineWave.h"
#include "dsp/SoftClipper/TanhSoftClipper.h"
#include "dsp/fast_trig.h"
#include "wav_writer.h"

const size_t render_buffer_size = 128;
const size_t render_sample_rate = 48000;
const double soft_clip_drive = 8.0;

double tick_sine_soft_clipper(double osc_freq)
{
    static bassline::dsp::SineWave osc;
    static bassline::dsp::TanhSoftClipper<4, bassline::dsp::BiquadLowPassFilter> soft_clipper;
    static bool init = false;

    if (!init) {
        osc.setSampleRate(render_sample_rate);
        init = true;
    }

    osc.setFrequency(osc_freq);
    return soft_clipper.process(osc.tick(), soft_clip_drive);
}

void render_sweep(double (*f)(double), std::string name)
{
    stmlib::WavWriter writer(1, render_sample_rate, 30);

    name = name + std::string(".wav");
    writer.Open2(name.c_str());

    bool play = true;
    double freq = 12;
    std::array<double, render_buffer_size> output;
    while (play) {
        for (size_t i = 0; i < render_buffer_size; i++) {
            freq += 0.00005;
            const double osc_freq = 440.0 * bassline::math::exp2((freq - 69.0) / 12.0);
            if (osc_freq > 20000.) {
                play = false;
            }
            output[i] = f(osc_freq) * 0.8;
        }
        writer.Write(&output[0], render_buffer_size);
    }
}

int main()
{
    render_sweep(tick_sine_soft_clipper, "sine_soft_clipper_drive_8");
    return 0;
}
