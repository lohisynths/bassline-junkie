//
//  Oscillator.cpp
//  SpaceBass
//
//  Created by Martin on 08.04.14.
//
//

#include "Oscillator.h"

stk::StkFloat Oscillator::mSampleRate = 48000.0;

void Oscillator::setMode(OscillatorMode mode) {
    mOscillatorMode = mode;
}

void Oscillator::setFrequency(stk::StkFloat frequency) {
    mFrequency = frequency;
    updateIncrement();
}

void Oscillator::setSampleRate(stk::StkFloat sampleRate) {
    mSampleRate = sampleRate;
    updateIncrement();
}

void Oscillator::generate(stk::StkFloat* buffer, int nFrames) {
    const stk::StkFloat twoPI = 2 * mPI;
    switch (mOscillatorMode) {
        case OSCILLATOR_MODE_SINE:
            for (int i = 0; i < nFrames; i++) {
                buffer[i] = stk::math::sin(mPhase);
                mPhase += mPhaseIncrement;
                while (mPhase >= twoPI) {
                    mPhase -= twoPI;
                }
            }
            break;
        case OSCILLATOR_MODE_SAW:
            for (int i = 0; i < nFrames; i++) {
                buffer[i] = 1.0 - (2.0 * mPhase / twoPI);
                mPhase += mPhaseIncrement;
                while (mPhase >= twoPI) {
                    mPhase -= twoPI;
                }
            }
            break;
        case OSCILLATOR_MODE_SQUARE:
            for (int i = 0; i < nFrames; i++) {
                if (mPhase <= mPI) {
                    buffer[i] = 1.0;
                } else {
                    buffer[i] = -1.0;
                }
                mPhase += mPhaseIncrement;
                while (mPhase >= twoPI) {
                    mPhase -= twoPI;
                }
            }
            break;
        case OSCILLATOR_MODE_TRIANGLE:
            for (int i = 0; i < nFrames; i++) {
            	stk::StkFloat value = -1.0 + (2.0 * mPhase / twoPI);
                buffer[i] = 2.0 * (stk::math::fabs(value) - 0.5);
                mPhase += mPhaseIncrement;
                while (mPhase >= twoPI) {
                    mPhase -= twoPI;
                }
            }
            break;
        default:
        break;


    }
}

stk::StkFloat Oscillator::nextSample() {
	stk::StkFloat value = naiveWaveformForMode(mOscillatorMode);
    mPhase += mPhaseIncrement;
    while (mPhase >= twoPI) {
        mPhase -= twoPI;
    }
    return value;
}

void Oscillator::setPitchMod(stk::StkFloat amount) {
    mPitchMod = amount;
    updateIncrement();
}



void Oscillator::updateIncrement() {
	stk::StkFloat pitchModAsFrequency = stk::math::pow(2.0, stk::math::fabs(mPitchMod) * 14.0) - 1;
    if (mPitchMod < 0) {
        pitchModAsFrequency = -pitchModAsFrequency;
    }
    stk::StkFloat calculatedFrequency = fmin(fmax(mFrequency + pitchModAsFrequency, 0), mSampleRate/2.0);
    mPhaseIncrement = calculatedFrequency * 2 * mPI / mSampleRate;
}

stk::StkFloat Oscillator::naiveWaveformForMode(OscillatorMode mode) {
	stk::StkFloat value=0;
    switch (mode) {
        case OSCILLATOR_MODE_SINE:
            value = stk::math::sin(static_cast<stk::StkFloat>(mPhase));
            break;
        case OSCILLATOR_MODE_SAW:
            value = (2.0 * mPhase / twoPI) - 1.0;
            break;
        case OSCILLATOR_MODE_SQUARE:
            if (mPhase < mPI) {
                value = 1.0;
            } else {
                value = -1.0;
            }
            break;
        case OSCILLATOR_MODE_TRIANGLE:
            value = -1.0 + (2.0 * mPhase / twoPI);
            value = 2.0 * (stk::math::fabs(value) - 0.5);
            break;
        default:
            break;
    }
    return value;
}
