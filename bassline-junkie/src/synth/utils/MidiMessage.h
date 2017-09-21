#ifndef MIDIMESSAGE_H_
#define MIDIMESSAGE_H_
#include <iostream>

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

	void print()
	{
		if (m_type != MidiMessage::NO_MESSAGE)
		{
			std::cout << "Message ";

			switch (m_type)
			{
			case MidiMessage::Type::CC:
				std::cout << "MidiMessage::Type::CC"; // prints "1"
				break;       // and exits the switchNOTE_ON
			case MidiMessage::Type::NOTE_ON:
				if (m_val_2 == 0)
					std::cout << "MidiMessage::Type::NOTE_OFF"; // prints "1"
				else
					std::cout << "MidiMessage::Type::NOTE_ON"; // prints "1"
				break;
			case MidiMessage::Type::NOTE_OFF:
				std::cout << "MidiMessage::Type::NOTE_OFF"; // prints "1"
				break;
			case MidiMessage::Type::NO_MESSAGE:
				std::cout << "MidiMessage::Type::NO_MESSAGE"; // prints "1"
				break;       // and exits the switchNOTE_ON
			}

			std::cout << " param " << +m_val_1 << " val "
					<< +m_val_2 << std::endl;
		}
	}

};

#endif /* MIDIMESSAGE_H_ */
