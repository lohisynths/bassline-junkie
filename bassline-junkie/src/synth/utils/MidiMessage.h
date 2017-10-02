#ifndef MIDIMESSAGE_H_
#define MIDIMESSAGE_H_
#include <iostream>
#include <sstream>

class MidiMessage
{
public:
	enum Type{
		NOTE_ON,
		NOTE_OFF,
		CC,
		NO_MESSAGE
	};

	Type m_type=NO_MESSAGE;
	uint8_t m_val_1=0;
	uint8_t m_val_2=0;

	uint8_t count=0;
	uint8_t time=0;

	void reset()
	{
		m_type=NO_MESSAGE;
		m_val_1=0;
		m_val_2=0;
		count=0;
	}

	void print(std::ostringstream &buf)
	{
		if (m_type != MidiMessage::NO_MESSAGE)
		{
			buf << "Message ";

			switch (m_type)
			{
			case MidiMessage::Type::CC:
				buf << "MidiMessage::Type::CC"; // prints "1"
				break;       // and exits the switchNOTE_ON
			case MidiMessage::Type::NOTE_ON:
				if (m_val_2 == 0)
					buf << "MidiMessage::Type::NOTE_OFF"; // prints "1"
				else
					buf << "MidiMessage::Type::NOTE_ON"; // prints "1"
				break;
			case MidiMessage::Type::NOTE_OFF:
				buf << "MidiMessage::Type::NOTE_OFF"; // prints "1"
				break;
			case MidiMessage::Type::NO_MESSAGE:
				buf << "MidiMessage::Type::NO_MESSAGE"; // prints "1"
				break;       // and exits the switchNOTE_ON
			}

			buf << " param " << +m_val_1 << " val "	<< +m_val_2;

		}
	}

};

#endif /* MIDIMESSAGE_H_ */
