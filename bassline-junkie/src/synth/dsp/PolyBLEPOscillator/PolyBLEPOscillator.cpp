//
//  PolyBLEPOscillator.cpp
//  SpaceBass
//
//  Created by Martin on 08.04.14.
//
//

#include "PolyBLEPOscillator.h"

// PolyBLEP by Tale
// (slightly modified)
// http://www.kvraudio.com/forum/viewtopic.php?t=375517
// http://www.martin-finke.de/blog/articles/audio-plugins-018-polyblep-oscillator/
stk::StkFloat PolyBLEPOscillator::poly_blep(stk::StkFloat t)
{
	stk::StkFloat dt = mPhaseIncrement / twoPI;
    // 0 <= t < 1
    if (t < dt) {
        t /= dt;
        return t+t - t*t - 1.0;
    }
    // -1 < t < 0
    else if (t > 1.0 - dt) {
        t = (t - 1.0) / dt;
        return t*t + t+t + 1.0;
    }
    // 0 otherwise
    else return 0.0;
}

stk::StkFloat PolyBLEPOscillator::getSample() {
	return outputOutput;
}

stk::StkFloat PolyBLEPOscillator::nextSample() {
	stk::StkFloat value = 0.0;
	stk::StkFloat t = mPhase / twoPI;
    
    if (mOscillatorMode == OSCILLATOR_MODE_SINE) {
        value = naiveWaveformForMode(OSCILLATOR_MODE_SINE);
    } else if (mOscillatorMode == OSCILLATOR_MODE_SAW) {
        value = naiveWaveformForMode(OSCILLATOR_MODE_SAW);
        value -= poly_blep(t);
        value *= -1.;
    } else {
        value = naiveWaveformForMode(OSCILLATOR_MODE_SQUARE);
        value += poly_blep(t);
        value -= poly_blep(stk::math::fmod(t + 0.5, 1.0));
        if (mOscillatorMode == OSCILLATOR_MODE_TRIANGLE) {
            // Leaky integrator: y[n] = A * x[n] + (1 - A) * y[n-1]
            value = mPhaseIncrement * value + (1 - mPhaseIncrement) * lastOutput;
            lastOutput = value;
        }
    }
    
    mPhase += mPhaseIncrement;
    while (mPhase >= twoPI) {
        mPhase -= twoPI;
    }
    outputOutput = value;
    return value;
}
