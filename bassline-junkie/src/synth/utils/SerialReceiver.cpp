/*
 * SerialReceiver.cpp
 *
 *  Created on: Jun 20, 2017
 *      Author: alax
 */

#include "SerialReceiver.h"

#include "concurency_helpers.h"


SerialReceiver::SerialReceiver() {

	n=0;
	const char *portname = "/dev/ttyUSB0";

	fd = open(portname, O_RDWR | O_NOCTTY | O_SYNC);
	if (fd < 0) {
		printf("error %d opening %s", errno, portname);
		return;
	}

	set_interface_attribs(fd, B115200, 0); // set speed to 115,200 bps, 8n1 (no parity)
	set_blocking(fd, 0);                // set no blocking

	t = new std::thread([&]()
		{
		stick_this_thread_to_core(0);
			while(1)
			{
				if(n==0)
					n =read(fd, buf, sizeof buf); // read up to 100 characters if ready to read
				//sleep(1);
			}
		}
	);


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


void SerialReceiver::writeBytes(const char *data, size_t size)
{
	write(fd, data, size);           // send 7 character greeting
}


int SerialReceiver::parse(char input)
{
	if (input == 0xb0 || input == 0xb9)
	{
		msg.m_type = MidiMessage::Type::CC;
		msg.count++;
	}
	else if (input == 0x90 || input == 0x99)
	{
		msg.m_type = MidiMessage::Type::NOTE_ON;
		msg.count++;
	}
	else if (input == 0x80)
	{
		msg.m_type = MidiMessage::Type::NOTE_OFF;
		msg.count++;
	}
	else if (msg.m_type != MidiMessage::Type::NO_MESSAGE)
	{
		if (msg.count == 1)
		{
			msg.m_val_1 = input;
			msg.count++;
		}
		else if (msg.count == 2)
		{
			msg.m_val_2 = input;
			return 1;
		}
		else
		{
			msg.reset();
			std::cout << "wrong message" << std::endl;
		}
	}
	else
	{
		msg.reset();
		std::cout << "unknown message" << std::endl;
	}
	return 0;
}


MidiMessage* SerialReceiver::getMessage()
{
	MidiMessage* output= nullptr;

	//TODO: IMPORTANT better parsing
	if(n!=0)
	{
		for(size_t i=0;i<n;i++)
		{
			if (parse(buf[i]))
			{
				//std::cout << std::endl;
				msg_out = msg;
				output = &msg_out;
				msg.reset();
				//std::cout << "    get ";
				//output->print();
			}
		}
		n=0;
	}

	return output;
}

