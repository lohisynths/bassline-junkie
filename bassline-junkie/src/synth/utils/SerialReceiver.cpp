/*
 * SerialReceiver.cpp
 *
 *  Created on: Jun 20, 2017
 *      Author: alax
 */

#include "SerialReceiver.h"

//TODO!!: IMPORTANT no memory allocations after lauynch!!

SerialReceiver::SerialReceiver() : running(true)
{
	logger = spdlog::get("serial").get();
	if(!logger)
	{
		std::cerr << "failed to get serial logger.\n";
		exit(1);
	}

	std::shared_ptr<spdlog::logger> logger=nullptr;


	const char *portname = "/dev/ttyACM0";

	fd = open(portname, O_RDWR | O_NOCTTY | O_SYNC);
	if (fd < 0) {
		std::ostringstream error_message;
		error_message << "error opening " << portname << ". " << strerror(errno) << ". \n Exit" ;
		//LOG_ERROR(logger, error_message.str());
		exit(1);
		return;
	}

	set_interface_attribs(fd, B115200, 0); // set speed to 115,200 bps, 8n1 (no parity)
	set_blocking(fd, 0);                // set no blocking

	start();


}

SerialReceiver::~SerialReceiver()
{
	//LOG_DEBUG(logger, "break worker thread while loop");
	stop();
	//LOG_DEBUG(logger, "t->join()");
    t->join();
	//LOG_DEBUG(logger, "t joinined");
	//LOG_DEBUG(logger, "delete t");
	delete t;
}


void SerialReceiver::worker_thread()
{
	stick_this_thread_to_core(0);

	uint8_t buf[100];

	while(running)
	{
		std::unique_lock<std::mutex> lock(m, std::defer_lock);

		int n = read(fd, &buf, sizeof(buf));// read up to 100 characters if ready to read
		if(n>0)
		{
			uint8_t *tmp_buff = buf;
			while(n--)
			{
				//std::cout << "pushed  " << *tmp_buff << std::endl;
				vector_char.push_back(*tmp_buff);
				tmp_buff++;
			}
		}
		else if( n == -1)
		{
			std::string error("error ");
			error += strerror(errno);
			//LOG_DEBUG(logger, error);
		}
	}
	//LOG_DEBUG(logger, "worker_thread exit");

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
    std::unique_lock<std::mutex> lock(m, std::defer_lock);
    running = false;
}

void SerialReceiver::writeBytes(const uint8_t *data, size_t size)
{
	write(fd, data, size);           // send 7 character greeting
}

std::deque<uint8_t> &SerialReceiver::getBuffer()
{
	return vector_char;
}

MidiMessage* SerialReceiver::getMessage()
{
	MidiMessage* output= nullptr;

	//TODO: IMPORTANT better parsing
	// conditional insteadf of lock
    std::unique_lock<std::mutex> lock(m, std::defer_lock);

    midi_parser.midiHandler(getBuffer());
    output = midi_parser.getMessage();

	return output;
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
