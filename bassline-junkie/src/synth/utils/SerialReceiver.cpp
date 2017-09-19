/*
 * SerialReceiver.cpp
 *
 *  Created on: Jun 20, 2017
 *      Author: alax
 */

#include "SerialReceiver.h"

#include "concurency_helpers.h"

//TODO!!: IMPORTANT no memory allocations after lauynch!!

SerialReceiver::SerialReceiver()
{
	const char *portname = "/dev/ttyACM0";

	fd = open(portname, O_RDWR | O_NOCTTY | O_SYNC);
	if (fd < 0) {
		printf("error %d opening %s", errno, portname);
		exit(1);
		return;
	}

	set_interface_attribs(fd, B115200, 0); // set speed to 115,200 bps, 8n1 (no parity)
	set_blocking(fd, 0);                // set no blocking

	t = new std::thread([&]()
	{
		stick_this_thread_to_core(0);

		uint8_t buf[100];

		while(1)
		{
			std::unique_lock<std::mutex> lock(m, std::defer_lock);

			int n = read(fd, &buf, sizeof(buf));// read up to 100 characters if ready to read
			if(n>0)
			{
				uint8_t *tmp_buff = buf;
				while(n--)
				{
					vector_char.push_back(*tmp_buff);
					tmp_buff++;
				}
			}
			else if( n == -1)
			{
				std::cout << "error " << errno << std::endl;
			}
		}
	});


}

SerialReceiver::~SerialReceiver()
{

}


int SerialReceiver::set_interface_attribs(int fd, int speed, int parity) {
	struct termios tty;
	memset(&tty, 0, sizeof tty);
	if (tcgetattr(fd, &tty) != 0) {
		printf("error %d from tcgetattr", errno);
		return -1;
	}

	cfsetospeed(&tty, speed);
	cfsetispeed(&tty, speed);

	tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
	// disable IGNBRK for mismatched speed tests; otherwise receive break
	// as \000 chars
	tty.c_iflag &= ~IGNBRK;         // disable break processing
	tty.c_lflag = 0;                // no signaling chars, no echo,
									// no canonical processing
	tty.c_oflag = 0;                // no remapping, no delays
	tty.c_cc[VMIN] = 0;            // read doesn't block
	tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

	tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

	tty.c_cflag |= (CLOCAL | CREAD); // ignore modem controls,
									 // enable reading
	tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
	tty.c_cflag |= parity;
	tty.c_cflag &= ~CSTOPB;
	tty.c_cflag &= ~CRTSCTS;

	if (tcsetattr(fd, TCSANOW, &tty) != 0) {
		printf("error %d from tcsetattr", errno);
		return -1;
	}
	return 0;
}

void SerialReceiver::set_blocking(int fd, int should_block) {
	struct termios tty;
	memset(&tty, 0, sizeof tty);
	if (tcgetattr(fd, &tty) != 0) {
		printf("error %d from tggetattr", errno);
		return;
	}

	tty.c_cc[VMIN] = should_block ? 1 : 0;
	tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

	if (tcsetattr(fd, TCSANOW, &tty) != 0)
		printf("error %d setting term attributes", errno);
}


void SerialReceiver::writeBytes(const uint8_t *data, size_t size)
{
	write(fd, data, size);           // send 7 character greeting
}

MidiMessage* SerialReceiver::midiHandler(std::deque<uint8_t> &bytes)
{
	MidiMessage* output = nullptr;
	size_t size = bytes.size();

	// Parse the MIDI bytes ... only keep MIDI channel messages.
	uint8_t byte = bytes.front();
	bytes.pop_front();
	//std::cout << "midiHandler " << +byte << " popped " << +bytes.size() <<  "\n";

	if (byte > 239)
		return nullptr;


	uint8_t type = byte & 0xF0;
	uint8_t channel = byte & 0x0F;

	byte = bytes.front();
	bytes.pop_front();
	//std::cout << "midiHandler " << +byte << " popped " << +bytes.size() <<  "\n";

	uint8_t intValue0 = byte;

	if ((type != 0xC0) && (type != 0xD0))
	{
		if (size < 3)
			return nullptr;

		byte = bytes.front();
		bytes.pop_front();
		//std::cout << "midiHandler " << +byte << " popped " << +bytes.size() <<  "\n";

		uint8_t intValue1 = byte;

		if (type == 0xb0)
		{
			tmp.m_type = MidiMessage::CC;
			tmp.m_val_1 = intValue0;
			tmp.m_val_2 = intValue1;
			//std::cout << "midi CC message on channel " << +channel << " "
			//		<< +intValue0 << " " << +intValue1 << " received\n";
			msg.push_back(tmp);
		}
		else if (type == 0x90)
		{
			if(intValue1!=0)
			{
				tmp.m_type = MidiMessage::NOTE_ON;
			}
			else
			{
				tmp.m_type = MidiMessage::NOTE_OFF;
			}

			tmp.m_val_2 = intValue1;
			tmp.m_val_1 = intValue0;

			//std::cout << "midi NOTE ON message on channel " << +channel << " "
			//		<< +intValue0 << " " << +intValue1 << " received\n";

			msg.push_back(tmp);
		}
		else
			tmp.reset();
	}

	if(msg.size() > 0)
	{
		tmp = msg.front();
		msg.pop_front();

		output = &tmp;
	}


	return output;

}

MidiMessage* SerialReceiver::getMessage()
{
	MidiMessage* output= nullptr;

	//TODO: IMPORTANT better parsing
	// conditional insteadf of lock
    std::unique_lock<std::mutex> lock(m, std::defer_lock);

    if(vector_char.size() > 2)
    {
    	output = midiHandler(vector_char);
	}

	return output;
}

