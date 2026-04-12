#include <array>
#include <cmath>
#include <string>

#include "dsp/fast_trig.h"
#include "dsp/BlitSaw.h"
#include "dsp/BlitSquare.h"
#include "dsp/MinBLEPOscillator.h"
#include "dsp/stmlib_polybleep.h"
#include "dsp/PolyBLEPOscillator/PolyBLEPOscillator.h"
#include "dsp/PolyBLEP/PolyBLEP.h"
#include "wav_writer.h"

const size_t buffer_size = 128;
const size_t sample_rate = 44100;

double tick_stk_blit_saw(double osc_freq) {
    static bassline::dsp::BlitSaw osc;
    static bool init = false;
    if (!init) {
        osc.setSampleRate(sample_rate);
        init = true;
    }
    osc.setFrequency(osc_freq);
    return osc.tick();
}


double tick_stk_blit_square(double osc_freq) {
    static bassline::dsp::BlitSquare osc;
    static bool init = false;
    if (!init) {
        osc.setSampleRate(sample_rate);
        init = true;
    }
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
    osc.Render<true>(
            osc_freq / sample_rate,  // master
            0.5f,                    // pulse-width
            0.0f,                    // shape: 0.0 for saw, 1.0 for square
            &out, 1);
    return out;
}

double tick_PolyBLEPOscillator(double osc_freq) {
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

double tick_minBLEP_saw(double osc_freq) {
    static bassline::dsp::MinBLEPOscillator osc;
    static bool init = false;
    if (!init) {
        osc.setSampleRate(sample_rate);
        init = true;
    }
    osc.setFrequency(osc_freq);
    return osc.tick() * 0.8;
}

double tick_minBLEP_square(double osc_freq) {
    static bassline::dsp::MinBLEPOscillator osc;
    static bool init = false;
    if (!init) {
        osc.setSampleRate(sample_rate);
        osc.setWaveform(bassline::dsp::MinBLEPOscillator::SQUARE);
        osc.setPulseWidth(0.5);
        init = true;
    }
    osc.setFrequency(osc_freq);
    return osc.tick() * 0.8;
}

double tick_PolyBleep(double osc_freq) {
    static PolyBLEP osc(sample_rate, PolyBLEP::SAWTOOTH, 440);
    osc.setFrequency(osc_freq);
    return osc.getAndInc();
}

void render_sweep(double (*f)(double), std::string name) {
    stmlib::WavWriter pisacz(1, sample_rate,30);

    name = name + std::string(".wav");
    pisacz.Open2(name.c_str());

    bool play = true;
    double freq = 12;
    std::array<double, buffer_size> output;
    while (play) {
        for (int i = 0; i < buffer_size; i++) {
            freq += 0.00005;
            double osc_freq = (double) 440.0 * bassline::math::pow(2.0, (freq - 69.0) / 12.0);
            if (osc_freq > 20000.) {
                play = false;
            }
            output[i] = f(osc_freq);
        }
        pisacz.Write(&output[0], buffer_size);
    }
}

int main() {
    //AudioDeviceRt<buffer_size> device;
    render_sweep(tick_stk_blit_saw, "stk_blit_saw");
    render_sweep(tick_stk_blit_square, "stk_blit_square");
    render_sweep(tick_stmlib_bleep, "stmlib_bleep");
    render_sweep(tick_PolyBleep, "PolyBleep");
    render_sweep(tick_PolyBLEPOscillator, "PolyBLEPOscillator");
    render_sweep(tick_minBLEP_saw, "minBLEP_saw");
    render_sweep(tick_minBLEP_square, "minBLEP_square");

    return 0;
}
