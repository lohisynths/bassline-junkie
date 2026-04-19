/*
 * SerialReceiver.cpp
 *
 *  Created on: Jun 20, 2017
 *      Author: alax
 */

#include "SerialReceiver.h"

#include "concurency_helpers.h"

//TODO!!: IMPORTANT no memory allocations after lauynch!!
#define SERIAL_PRINT_FUNCTION std::cout << "[SerialReceiver] "<< __PRETTY_FUNCTION__ << std::endl;
#define SERIAL_PRINT(a) std::cout << "[SerialReceiver] \t"<< __func__ << "\t\t " << (a) << std::endl;

SerialReceiver::SerialReceiver() : running(false), fd(-1)
{
	const char *portname = "/dev/ttyAMA0";

	fd = open(portname, O_RDWR | O_NOCTTY | O_SYNC);
	if (fd < 0) {
		printf("SerialReceiver  warning: error %d opening %s (serial MIDI unavailable)\n", errno, portname);
		return;
	}

	running.store(true);
    set_interface_attribs(fd, B1000000, 0); // set speed to 1000000 bps, 8n1 (no parity)
	set_blocking(fd, 0);                // set no blocking

	start();
}

SerialReceiver::~SerialReceiver()
{
	SERIAL_PRINT_FUNCTION;
	SERIAL_PRINT("break worker thread while loop");
	stop();
	if (t) {
		SERIAL_PRINT("t->join()");
		t->join();
		SERIAL_PRINT("t joinined");
		SERIAL_PRINT("delete t");
		delete t;
		t = nullptr;
	}
	if (fd >= 0) {
		::close(fd);
		fd = -1;
	}
}


void SerialReceiver::worker_thread()
{
	stick_this_thread_to_core(1);
    set_pthread_params();

	uint8_t buf[256];

	while(running.load())
	{
		int n = read(fd, &buf, sizeof(buf));// read up to 100 characters if ready to read
		if(n>0)
		{
			std::deque<uint8_t> local_buffer;
			uint8_t *tmp_buff = buf;
			while(n--)
			{
				local_buffer.push_back(static_cast<uint8_t>(*tmp_buff));
				tmp_buff++;
			}

			// Parse bytes on the worker thread so the audio thread only pops
			// already-decoded MIDI messages.
			std::lock_guard<std::mutex> lock(m);
			midi_parser.midiHandler(local_buffer);
		}
		else if( n == -1)
		{
			SERIAL_PRINT("error");
			SERIAL_PRINT(errno);
		}
	}
	SERIAL_PRINT_FUNCTION;
	SERIAL_PRINT("worker_thread exit");

}


void SerialReceiver::start()
{
	t = new std::thread([&]()
	{
		worker_thread();
	});
}


void SerialReceiver::stop()
{
	running.store(false);
}

void SerialReceiver::writeBytes(const uint8_t *data, size_t size)
{
    size_t ret = write(fd, data, size);
    if(ret != size) {
        printf("ret != size\n");

    }
}

std::deque<uint8_t> &SerialReceiver::getBuffer()
{
	return vector_char;
}

MidiMessage SerialReceiver::getMessage()
{
	if (fd < 0) {
		return MidiMessage();
	}
	std::lock_guard<std::mutex> lock(m);
	return midi_parser.getMessage();
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
