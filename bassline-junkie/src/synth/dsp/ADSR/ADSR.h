//
//  ADRS.h
//
//  Created by Nigel Redmon on 12/18/12.
//  EarLevel Engineering: earlevel.com
//  Copyright 2012 Nigel Redmon
//
//  For a complete explanation of the ADSR envelope generator and code,
//  read the series of articles by the author, starting here:
//  http://www.earlevel.com/main/2013/06/01/envelope-generators/
//
//  License:
//
//  This source code is provided as is, without warranty.
//  You may copy and distribute verbatim copies of this document.
//  You may modify and use this source code to create binary code for your own purposes, free or commercial.
//

#ifndef ADRS_h
#define ADRS_h

#include <Stk.h>
class ADSR {
public:
	ADSR(void);
	~ADSR(void);
	stk::StkFloat process(void);
    stk::StkFloat getOutput(void);
    int getState(void);
	void gate(int on);
    void setAttackRate(stk::StkFloat rate);
    void setDecayRate(stk::StkFloat rate);
    void setReleaseRate(stk::StkFloat rate);
	void setSustainLevel(stk::StkFloat level);
    void setTargetRatioA(stk::StkFloat targetRatio);
    void setTargetRatioDR(stk::StkFloat targetRatio);
    void reset(void);
    void setLoopOn(bool enabled) {
        loop = enabled;
        if(!loop) {
            state = env_release;
        }
    }
    
    enum envState {
        env_idle = 0,
        env_attack,
        env_decay,
        env_sustain,
        env_release
    };

protected:
    stk::StkFloat sample_rate;
	stk::StkFloat max_time;
	int state;
	stk::StkFloat output;
	stk::StkFloat attackRate;
	stk::StkFloat decayRate;
	stk::StkFloat releaseRate;
	stk::StkFloat attackCoef;
	stk::StkFloat decayCoef;
	stk::StkFloat releaseCoef;
	stk::StkFloat sustainLevel;
    stk::StkFloat targetRatioA;
    stk::StkFloat targetRatioDR;
    stk::StkFloat attackBase;
    stk::StkFloat decayBase;
    stk::StkFloat releaseBase;
    bool loop;
 
    stk::StkFloat calcCoef(stk::StkFloat rate, stk::StkFloat targetRatio);
};

inline stk::StkFloat ADSR::process() {
	switch (state) {
        case env_idle:
            break;
        case env_attack:
            output = attackBase + output * attackCoef;
            if (output >= 1.0) {
                output = 1.0;
                state = env_decay;
            }
            break;
        case env_decay:
            output = decayBase + output * decayCoef;
            if(!loop) {
                if (output <= sustainLevel) {
                    output = sustainLevel;
                    state = env_sustain;
                }
            } else {
                if (output <= sustainLevel) {
                    state = env_attack;
                }
            }
            break;
        case env_sustain:
            break;
        case env_release:
            output = releaseBase + output * releaseCoef;
            if (output <= 0.0) {
                output = 0.0;
                if(loop) {
                    state = env_attack;
                } else {
                    state = env_idle;
                }
            }
	}
	return output;
}

inline void ADSR::gate(int gate) {
	if (gate) {
        state = env_attack;
	}
	else if (state != env_idle) {
	    if(!loop) {
	        state = env_release;
	    }
	}
}

inline int ADSR::getState() {
    return state;
}

inline void ADSR::reset() {
    state = env_idle;
    output = 0.0;
}

inline stk::StkFloat ADSR::getOutput() {
	return output;
}

#endif
