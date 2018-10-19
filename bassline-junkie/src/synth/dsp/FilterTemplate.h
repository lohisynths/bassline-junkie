/*
 * FilterTemplate.h
 *
 *  Created on: Sep 16, 2017
 *      Author: alax
 */

#ifndef SRC_SYNTH_DSP_FILTERTEMPLATE_H_
#define SRC_SYNTH_DSP_FILTERTEMPLATE_H_

#include "Stk.h"

class Filter
{
public:
    enum class FilterType {LOW_PASS, HI_PASS, LOW_PASS}; // enum class

    virtual ~Filter();

    virtual double process(double x)=0;
	virtual void setCutoff(double c)=0;
	virtual void setRes(double c)=0;
    virtual void setType(FilterType type)=0;

};

#endif /* SRC_SYNTH_DSP_FILTERTEMPLATE_H_ */
