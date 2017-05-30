#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include "ConditionalVar.h"


#include "Engine.h"


const unsigned int format_bits = 32; //snd_pcm_format_width(*m_format);
const unsigned int maxval = (1U << (format_bits - 1U)) - 1U;

class ConditionalVarEngine : public Engine {
public:
	ConditionalVarEngine(std::array<Voice, voices_count> &voices) : m_voices(voices)
	{
		// ConditionalVar(vector of voices, first cpu, number of first voice, number of voices to process
		cores.push_back( new ConditionalVar(m_voices, 2,0,3) );
		cores.push_back( new ConditionalVar(m_voices, 3,3,3) );

		free_cores.push_back(0);
		free_cores.push_back(1);
		free_cores.push_back(2);
		free_cores.push_back(3);
		free_cores.push_back(4);
		free_cores.push_back(5);
	}

	std::array<uint32_t, 512> &process(std::array<Voice, voices_count> &voices)
	{
		for(auto core : cores)
			core->request();
		for(auto core : cores)
			core->wait();


		for(int i = 0; i < 100; i++)
			updateMessages(voices);

		// reset buffer
		std::fill(std::begin(output), std::end(output), 0);
		// fill output buffer with data
		for (auto &voice : voices)
		{
			auto &voice_data = voice.get_array();

			std::transform(voice_data.begin(), voice_data.end(),
					voice_data.begin(),
					std::bind1st(std::multiplies<double>(),maxval*0.6));
			std::transform(output.begin(), output.end(), voice_data.begin(),
					output.begin(), std::plus<int>());
		}
		return output;
	}


private:
	std::vector<ConditionalVar*> cores;
	std::array<Voice, voices_count> &m_voices;
	std::array<uint32_t, 512> output;

};
