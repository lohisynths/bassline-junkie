#include <algorithm>

#include "utils/AudioDeviceRt.h"
#include "Stk.h"
#include "BlitSaw.h"
#include "dsp/PolyBLEPOscillator/PolyBLEPOscillator.h"
#include "oscillator.h"
#include "wav_writer.h"

const size_t buffer_size = 128;
const size_t sample_rate = 96000;

double tick_stk_blit(double osc_freq) {
    static stk::BlitSaw osc;
    osc.setFrequency(osc_freq);
    return osc.tick();
}

double tick_stmlib_bleep(double osc_freq) {
    static stmlib::Oscillator osc;
    static bool init = false;
    float out;
    if (!init) {
        osc.Init();
        init = true;
    }
    osc.Render<true>(osc_freq / sample_rate,  // master
    0.5f,  // pulse-width
            0.0f,  // shape: 0.0 for saw, 1.0 for square
            &out, 1);
    return out;
}

double tick_poly_bleep(double osc_freq) {
    static PolyBLEPOscillator osc;
    static bool init = false;
    float out;
    if (!init) {
        osc.setMode(Oscillator::OSCILLATOR_MODE_SAW);
        osc.setSampleRate(sample_rate);
        init = true;
    }
    osc.setFrequency(osc_freq);
    return osc.nextSample();
}

void render_sweep(double (*f)(double), AudioDeviceRt<buffer_size> &device) {
    bool play = true;
    double freq = 12;
    std::array<double, buffer_size> output;
    while (play) {
        for (int i = 0; i < buffer_size; i++) {
            freq += 0.00005;
            double osc_freq = (stk::StkFloat) 440.0 * pow(2.0, (freq - 69.0) / 12.0);
            if (osc_freq > 20000.) {
                play = false;
            }
            output[i] = f(osc_freq);
        }
        device.play(output);
    }

    for(int i=0;i<sample_rate;i++) {
        device.mute();
    }
}

void render_sweep(double (*f)(double)) {
    stmlib::WavWriter pisacz(1, sample_rate,30);
    static int count = 0;

    std::string name("piesek");
    name = name + std::to_string(count);
    count ++;
    pisacz.Open2(name.c_str());

    bool play = true;
    double freq = 12;
    std::array<double, buffer_size> output;
    while (play) {
        for (int i = 0; i < buffer_size; i++) {
            freq += 0.00005;
            double osc_freq = (stk::StkFloat) 440.0 * pow(2.0, (freq - 69.0) / 12.0);
            if (osc_freq > 20000.) {
                play = false;
            }
            output[i] = f(osc_freq);
        }
        pisacz.Write(&output[0], buffer_size);
    }
}

int main() {
    stk::Stk::setSampleRate(sample_rate);
    stk::Stk::showWarnings(true);

    AudioDeviceRt<buffer_size> device;

    render_sweep(tick_stk_blit);
    render_sweep(tick_stmlib_bleep);
    render_sweep(tick_poly_bleep);

    return 0;
}
