//
//  ADSR_.cpp
//
//  Created by Nigel Redmon on 12/18/12.
//  EarLevel Engineering: earlevel.com
//  Copyright 2012 Nigel Redmon
//
//  For a complete explanation of the ADSR_ envelope generator and code,
//  read the series of articles by the author, starting here:
//  http://www.earlevel.com/main/2013/06/01/envelope-generators/
//
//  License:
//
//  This source code is provided as is, without warranty.
//  You may copy and distribute verbatim copies of this document.
//  You may modify and use this source code to create binary code for your own purposes, free or commercial.
//
//  1.01  2016-01-02  njr   added calcCoefs to SetTargetRatio functions that were in the ADSR_ widget but missing in this code
//

#include "ADSR_.h"
#include <limits>

ADSR_::ADSR_() {
	m_sampleRate=44100;
	m_maxTime=5;

    reset();
    setAttackTime( 0.001 );
    setDecayTime(0.1);
    setReleaseTime( 0.001 );
    setSustainLevel(1.0);
    setTargetRatioA(0.3);
    setTargetRatioDR(0.001);
}

ADSR_::~ADSR_(void) {
}


void ADSR_::setAttackTime(stk::StkFloat rate) {
	rate=rate*m_sampleRate*m_maxTime;

    attackRate = rate;
    attackCoef = calcCoef(rate, targetRatioA);
    attackBase = (1.0 + targetRatioA) * (1.0 - attackCoef);
}

void ADSR_::setDecayTime(stk::StkFloat rate) {
	rate=rate*m_sampleRate*m_maxTime;

    decayRate = rate*m_sampleRate*m_maxTime;
    decayCoef = calcCoef(rate, targetRatioDR);
    decayBase = (sustainLevel - targetRatioDR) * (1.0 - decayCoef);
}

void ADSR_::setReleaseTime(stk::StkFloat rate) {
	rate=rate*m_sampleRate*m_maxTime;

    releaseRate = rate*m_sampleRate*m_maxTime;
    releaseCoef = calcCoef(rate, targetRatioDR);
    releaseBase = -targetRatioDR * (1.0 - releaseCoef);
}

stk::StkFloat ADSR_::calcCoef(stk::StkFloat rate, stk::StkFloat targetRatio) {
    return stk::math::exp(-stk::math::log((1.0 + targetRatio) / targetRatio) / rate);
}

void ADSR_::setSustainLevel(stk::StkFloat level) {
    sustainLevel = level;
    decayBase = (sustainLevel - targetRatioDR) * (1.0 - decayCoef);
}

void ADSR_::setTargetRatioA(stk::StkFloat targetRatio) {
    if (targetRatio < 0.000000001)
        targetRatio = 0.000000001;  // -180 dB
    targetRatioA = targetRatio;
    attackCoef = calcCoef(attackRate, targetRatioA);
    attackBase = (1.0 + targetRatioA) * (1.0 - attackCoef);
}

void ADSR_::setTargetRatioDR(stk::StkFloat targetRatio) {
    if (targetRatio < 0.000000001)
        targetRatio = 0.000000001;  // -180 dB
    targetRatioDR = targetRatio;
    decayCoef = calcCoef(decayRate, targetRatioDR);
    releaseCoef = calcCoef(releaseRate, targetRatioDR);
    decayBase = (sustainLevel - targetRatioDR) * (1.0 - decayCoef);
    releaseBase = -targetRatioDR * (1.0 - releaseCoef);
}
