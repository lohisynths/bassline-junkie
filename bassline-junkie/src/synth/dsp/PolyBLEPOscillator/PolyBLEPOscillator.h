//
//  PolyBLEPOscillator.h
//  SpaceBass
//
//  Created by Martin on 08.04.14.
//
//

#ifndef __SpaceBass__PolyBLEPOscillator__
#define __SpaceBass__PolyBLEPOscillator__

#include "Oscillator.h"

class PolyBLEPOscillator: public Oscillator {
public:
    PolyBLEPOscillator() : lastOutput(0.0) { updateIncrement(); };
    stk::StkFloat nextSample();
    stk::StkFloat getSample();

private:
    stk::StkFloat poly_blep(stk::StkFloat t);
    stk::StkFloat lastOutput;
    stk::StkFloat outputOutput;
};

#endif /* defined(__SpaceBass__PolyBLEPOscillator__) */
