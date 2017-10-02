#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <array>

#include <Stk.h>

#include "../config.h"
#include "../utils/SerialReceiver.h"
#include "../utils/MidiReceiver.h"
#include "../utils/MidiReceiverRt.h"
#include "../voice.h"
#include "thread.h"

#include "PipeReceiver.h"

template<size_t voices_count,size_t buffer_size>
class Engine {

private:
	std::array<thread<buffer_size>, max_cores> cores;
	std::array<stk::StkFloat, buffer_size> output_float;
	std::array<Voice<buffer_size>, voices_count> m_voices;

	SerialReceiver messager;
	//MidiReceiverRt messager;
	std::vector<std::pair<MidiMessage, int>> notes;
	std::vector<int> free_voices;
	std::shared_ptr<spdlog::logger> engine_logger=nullptr;

public:
	Engine()
	{
		engine_logger = spdlog::get("engine");
		if(!engine_logger)
		{
			std::cerr << "failed to get egine logger.\n";
			exit(1);
		}

		if(voices_count > max_cores*max_voices_per_core)
		{
			std::cerr << "more voice requested than possible to get from max_cores*max_voices_per_core."
					<< std::endl << "edit config.h"<< std::endl;
			exit(1);
		}

		int voices_left=voices_count;
		size_t core_count=0;

		while(voices_left)
		{
			core_count++;
			int core_nr = max_cores - core_count + first_cpu;
		
			std::ostringstream string;
			string << "new thread created on core " << core_nr;
			LOG_DEBUG(engine_logger, string.str());

			for(size_t j=0; j < max_voices_per_core ; j++)
			{
				int voice_nr = voices_count - voices_left;
				cores[core_count-1].add_voice(&m_voices[voice_nr]);
				free_voices.push_back(voice_nr);

				std::ostringstream string;
				string << "core " << core_nr << " voice " << voice_nr << " pushed";
				LOG_DEBUG(engine_logger, string.str());

				voices_left--;
				if(voices_left==0)
					break;
			}
			cores[core_count-1].set_cpu_affinity(core_nr);

		}

		for(auto &core: cores)
		{
			std::ostringstream string("core started");
			LOG_DEBUG(engine_logger, string.str());
			core.start_thread();
		}
	}

	std::array<stk::StkFloat, buffer_size> &process()
	{
		for(auto &core : cores)
			core.request();
		for(auto &core : cores)
			core.wait();

		for(int i=0;i<50;i++)
		{
			updateMessages();
		}

		// reset buffer
		std::fill(std::begin(output_float), std::end(output_float), 0);
		// fill output buffer with data
		for (auto &voice : m_voices)
		{
			auto &voice_data = voice.get_array();
			auto ciabej = [](stk::StkFloat &output, stk::StkFloat &input){ output += input; return output;};
			std::transform(output_float.begin(), output_float.end(), voice_data.begin(),output_float.begin(),ciabej);
		}
		return output_float;
	}

	void noteOff(MidiMessage* msg)
	{
		for (size_t i = 0; i < notes.size(); i++)
		{
			auto &note = notes[i].first;
			auto core = notes[i].second;

			if (note.m_val_1 == msg->m_val_1)
			{
				m_voices[core].noteOn(msg->m_val_1,msg->m_val_2);
				notes.erase(notes.begin() + i);
				free_voices.push_back(core);
			}
		}
	}

	void noteOn(MidiMessage* msg)
	{

		if (msg->m_val_2 != 0)
		{
			if (notes.size() < voices_count)
			{
				auto core = free_voices.back();
				free_voices.pop_back();

				notes.push_back(std::make_pair(*msg, core));

				auto note = &notes.back().first;
				auto core_nr = notes.back().second;

				m_voices[core_nr].noteOn(note->m_val_1,note->m_val_2);
			}
		}
		else
		{
			noteOff(msg);
		}
	}

	void updateMessages()
	{

		size_t lolo = stk::Stk::getPipeAvaliable();


		uint8_t buffer[1024]={};
		if(lolo > 0)
		{
			std::cout << "getPipeAvaliable " << lolo << std::endl;

			size_t size;
			stk::Stk::readFromPipe(buffer, size);
			if(size>0)
			{
				std::cout << "jes" << std::endl;

				std::cout << buffer << std::endl << std::endl;
			}
			else if (size > sizeof(buffer))
			{
				std::cout << "za maly bufor" << std::endl;
			}
			else
			{
				std::cout << "nie ma" << std::endl;

			}
		}



		auto msg = messager.getMessage();
		if (msg)
		{
			std::ostringstream data;
			msg->print(data);
			LOG_INFO(engine_logger, data.str());

			if (msg->m_type != MidiMessage::NO_MESSAGE)
			{
				switch (msg->m_type)
				{

				case MidiMessage::Type::NOTE_ON:
				{
					if(msg->m_val_2!=0)
					{
						noteOn(msg);
						break;
					}
					else
						msg->m_type=MidiMessage::Type::NOTE_OFF;
				} // ignore warning NoteOn with vel 0 = NoteOff

				case MidiMessage::Type::NOTE_OFF:
				{
					noteOff(msg);

					break;       // and exits the switchNOTE_ON
				}
				case MidiMessage::Type::CC:
				{
					for(auto &voice : m_voices)
						voice.cc(msg->m_val_1,msg->m_val_2);

					break;       // and exits the switchNOTE_ON
				}
				case MidiMessage::Type::NO_MESSAGE:
					break;       // and exits the switchNOTE_ON
				};
			}
		}
	}
};
