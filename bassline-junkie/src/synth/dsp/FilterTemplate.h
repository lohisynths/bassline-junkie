/*
 * FilterTemplate.h
 *
 *  Created on: Sep 16, 2017
 *      Author: alax
 */

#ifndef SRC_SYNTH_DSP_FILTERTEMPLATE_H_
#define SRC_SYNTH_DSP_FILTERTEMPLATE_H_

#include "Stk.h"
#include "../modifiers.h"
#include "../utils/smod_mat.h"

class Filter
{
public:
	virtual double process(double x)=0;
	virtual void setCutoff(double c)=0;
	virtual void setRes(double c)=0;
};

#endif /* SRC_SYNTH_DSP_FILTERTEMPLATE_H_ */
