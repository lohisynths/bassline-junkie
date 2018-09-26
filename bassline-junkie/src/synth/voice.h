/*
 * synth.h
 *
 *  Created on: Mar 14, 2017
 *      Author: alax
 */

#ifndef SYNTH_H_
#define SYNTH_H_

#include <array>
#include <BlitSaw.h>
#include <BlitSquare.h>
#include <SineWave.h>

#include "dsp/ADSR/ADSR.h"
#include "dsp/Lfo.h"
#include "dsp/MoogFilter.h"
#include "dsp/Osc.h"

#include "dsp/VAStateVariableFilter.h"

#include "modifiers.h"

#include "utils/MidiReceiver.h"
#include "utils/clipping.h"

const stk::StkFloat divider = 1. / 127.;
const stk::StkFloat env_range_in_notes = 12 * 4;
const stk::StkFloat lfo_range_in_notes = 12 * 2;

template<size_t buffer_size>
class Voice
{
public:

    void set_voice_index(int index) {
        m_modifiers.set_voice_index(index);
    }

	Voice()
	{
		m_modifiers.flt_mod_matrix.frequency = 64+32;
		m_modifiers.flt_mod_matrix.resonance = 0;

		m_modifiers.amp_mod_matrix.velocity= 0;

		for (unsigned int i=0; i < osc.size();i++)
		{
			m_modifiers.osc_m[i].octave = 0.5;
			m_modifiers.osc_m[i].freq = 64;

			m_modifiers.updateOsc(osc[i],i);
		}
		std::fill(std::begin(array), std::end(array), 0);
	}

	~Voice(){};

	std::array<stk::StkFloat, buffer_size>& get_array()
	{
		return array;
	}

	void cc(stk::StkFloat param, stk::StkFloat val)
	{
		m_modifiers.controlCange(param, val);
	}

	void noteOn(stk::StkFloat note, stk::StkFloat vel)
	{
		m_modifiers.noteOn(note, vel);
	}

	void noteOff(stk::StkFloat note, stk::StkFloat vel)
	{
		m_modifiers.noteOff();
	}

	void process()
	{
		for (auto &sample : array)
		{
			for (auto &adsr : m_modifiers.env)
			{
				adsr.process();
			}
			for (auto &lfo : m_modifiers.lfo)
			{
				lfo.tick();
			}

			///////////////////////////// OSCILLATORS
			for(unsigned int i=0;i<osc.size();i++)
			{
				m_modifiers.updateOsc(osc[i],i);
			}
			/////////////////////////////////////////////////////////////////////////////

			///////////////////////////// FILTERS
			m_modifiers.updateFilter(&filter);
			///////////////////////////// FILTERS

			double output;
            //sralinka output;
			output = 0;

			for(auto &wave : osc) {
				output = tanh(output + wave.tick());
			}

            output *= m_modifiers.amp_mod_matrix.velocity; // velocity
            output *= m_modifiers.env[2].getOutput();
			output = filter.process( output );

			sample = output;
			//writer.process(adsr_tick);
		}
	}

private:

	//MoogFilter filter;
	VAStateVariableFilter filter;
	std::array<Osc, 3> osc;
	modifiers m_modifiers;
	std::array<stk::StkFloat, buffer_size> array;


};

#endif /* SYNTH_H_ */
