//
//  Oscillator.h
//  SpaceBass
//
//  Created by Martin on 08.04.14.
//
//

#ifndef __SpaceBass__Oscillator__
#define __SpaceBass__Oscillator__

#include "Stk.h"

class Oscillator {
public:
    enum OscillatorMode {
        OSCILLATOR_MODE_SINE = 0,
        OSCILLATOR_MODE_SAW,
        OSCILLATOR_MODE_SQUARE,
        OSCILLATOR_MODE_TRIANGLE,
        kNumOscillatorModes
    };
    void setMode(OscillatorMode mode);
    void setFrequency(stk::StkFloat frequency);
    void setSampleRate(stk::StkFloat sampleRate);
    void generate(stk::StkFloat* buffer, int nFrames);
    inline void setMuted(bool muted) { isMuted = muted; }
    virtual stk::StkFloat nextSample()=0;
    Oscillator() :
    mOscillatorMode(OSCILLATOR_MODE_SINE),
    mPI(2*acos(0.0)),
    twoPI(2 * mPI),
    isMuted(true),
    mFrequency(440.0),
    mPitchMod(0.0),
    mPhase(0.0) { updateIncrement(); };
    virtual ~Oscillator();

    void reset() { mPhase = 0.0; }
    void setPitchMod(stk::StkFloat amount);
protected:
    stk::StkFloat naiveWaveformForMode(OscillatorMode mode);
    OscillatorMode mOscillatorMode;
    const stk::StkFloat mPI;
    const stk::StkFloat twoPI;
    bool isMuted;
    stk::StkFloat mFrequency;
    stk::StkFloat mPitchMod;
    stk::StkFloat mPhase;
    static stk::StkFloat mSampleRate;
    stk::StkFloat mPhaseIncrement;
    void updateIncrement();
    
};

#endif /* defined(__SpaceBass__Oscillator__) */
