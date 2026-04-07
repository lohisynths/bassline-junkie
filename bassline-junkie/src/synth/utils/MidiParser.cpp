/*
 * MidiParser.cpp
 *
 *  Created on: Sep 19, 2017
 *      Author: alax
 */

#include "MidiParser.h"

MidiParser::MidiParser()
{
	// TODO Auto-generated constructor stub

}

MidiParser::~MidiParser()
{
	// TODO Auto-generated destructor stub
}


size_t MidiParser::getChannelMessageSize(uint8_t status) const
{
	uint8_t type = status & 0xF0;
	if (type == 0xC0 || type == 0xD0)
	{
		return 2;
	}

	return 3;
}


void MidiParser::parsePendingBytes()
{
	while (!pending_bytes.empty())
	{
		uint8_t status = pending_bytes.front();
		bool use_running_status = false;

		if ((status & 0x80) == 0)
		{
			if (running_status == 0)
			{
				pending_bytes.pop_front();
				continue;
			}

			status = running_status;
			use_running_status = true;
		}
		else if (status >= 0xF8)
		{
			// Realtime bytes can appear between channel messages.
			pending_bytes.pop_front();
			continue;
		}
		else if (status >= 0xF0)
		{
			// System messages are not used by the synth. Consume them so they
			// do not break channel-message parsing and clear running status.
			running_status = 0;

			if (status == 0xF0)
			{
				pending_bytes.pop_front();
				while (!pending_bytes.empty())
				{
					uint8_t byte = pending_bytes.front();
					pending_bytes.pop_front();
					if (byte == 0xF7)
					{
						break;
					}
				}
				continue;
			}

			size_t message_size = 1;
			if (status == 0xF1 || status == 0xF3)
			{
				message_size = 2;
			}
			else if (status == 0xF2)
			{
				message_size = 3;
			}

			if (pending_bytes.size() < message_size)
			{
				return;
			}

			for (size_t i = 0; i < message_size; ++i)
			{
				pending_bytes.pop_front();
			}
			continue;
		}

		size_t message_size = getChannelMessageSize(status);
		size_t data_bytes = message_size - 1;
		size_t available = pending_bytes.size() - (use_running_status ? 0 : 1);

		if (available < data_bytes)
		{
			return;
		}

		bool invalid_data = false;
		for (size_t i = 0; i < data_bytes; ++i)
		{
			size_t index = use_running_status ? i : i + 1;
			if (pending_bytes[index] & 0x80)
			{
				invalid_data = true;
				break;
			}
		}

		if (invalid_data)
		{
			if (use_running_status)
			{
				running_status = 0;
				pending_bytes.pop_front();
			}
			else
			{
				pending_bytes.pop_front();
			}
			continue;
		}

		running_status = status;
		if (!use_running_status)
		{
			pending_bytes.pop_front();
		}

		uint8_t intValue0 = pending_bytes.front();
		pending_bytes.pop_front();

		uint8_t intValue1 = 0;
		if (data_bytes == 2)
		{
			intValue1 = pending_bytes.front();
			pending_bytes.pop_front();
		}

		uint8_t type = status & 0xF0;
		uint8_t channel = status & 0x0F;

		if (type == 0xB0)
		{
			build_tmp_.m_type = MidiMessage::CC;
			build_tmp_.m_val_1 = intValue0;
			build_tmp_.m_val_2 = intValue1;
			build_tmp_.channel = channel;
			msg.push_back(build_tmp_);
		}
		else if (type == 0x90)
		{
			if (intValue1 != 0)
			{
				build_tmp_.m_type = MidiMessage::NOTE_ON;
			}
			else
			{
				build_tmp_.m_type = MidiMessage::NOTE_OFF;
			}

			build_tmp_.m_val_1 = intValue0;
			build_tmp_.m_val_2 = intValue1;
			build_tmp_.channel = channel;
			msg.push_back(build_tmp_);
		}
		else if (type == 0x80)
		{
			build_tmp_.m_type = MidiMessage::NOTE_OFF;
			build_tmp_.m_val_1 = intValue0;
			build_tmp_.m_val_2 = intValue1;
			build_tmp_.channel = channel;
			msg.push_back(build_tmp_);
		}
	}
}



void MidiParser::midiHandler(std::deque<uint8_t> &bytes)
{
	while (!bytes.empty())
	{
		pending_bytes.push_back(bytes.front());
		bytes.pop_front();
	}

	parsePendingBytes();
}


MidiMessage MidiParser::getMessage()
{
	MidiMessage output;

	if(msg.size() > 0)
	{
		output = msg.front();
		msg.pop_front();
	}

	return output;
}
