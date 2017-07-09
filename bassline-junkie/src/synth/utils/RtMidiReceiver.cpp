/*
 * RtMidiReceiver.cpp
 *
 *  Created on: Jul 9, 2017
 *      Author: alax
 */

#include "RtMidiReceiver.h"

RtMidiReceiver::RtMidiReceiver()
{
	  probe();

	  // Check available ports.
	  unsigned int nPorts = midiin.getPortCount();
	  if ( nPorts == 0 ) {
	    std::cout << "No ports available!\n";
	    exit(1);
	  }
	  midiin.openPort( 1 );
	  // Don't ignore sysex, timing, or active sensing messages.
	  midiin.ignoreTypes( false, false, false );


}

RtMidiReceiver::~RtMidiReceiver()
{

}




MidiMessage* RtMidiReceiver::getMessage() {

	MidiMessage* output= nullptr;

	double stamp = midiin.getMessage( &message );
	int nBytes = message.size();

	if ( nBytes > 0 )
    {
//        for (int i=0; i<nBytes; i++ )
//          std::cout << "Byte " << i << " = " << (int)message[i] << ", ";
//
//        std::cout << "stamp = " << stamp << std::endl;
        for (int i=0; i<nBytes; i++ )
		if (parse(message[i]))
		{
			//std::cout << std::endl;
			msg_out = msg;
			output = &msg_out;
			msg.reset();
			//std::cout << "    get ";
			//output->print();
		}
    }
   	return output;
}

void RtMidiReceiver::probe(){
	RtMidiOut *midiout = 0;

	// Check inputs.
	unsigned int nPorts = midiin.getPortCount();
	std::cout << "\nThere are " << nPorts << " MIDI input sources available.\n";
	std::string portName;
	for (unsigned int i = 0; i < nPorts; i++) {
		try {
			portName = midiin.getPortName(i);
		} catch (RtMidiError &error) {
			error.printMessage();
			goto cleanup;
		}
		std::cout << "  Input Port #" << i + 1 << ": " << portName << '\n';
	}
	// RtMidiOut constructor
	try {
		midiout = new RtMidiOut();
	} catch (RtMidiError &error) {
		error.printMessage();
		exit( EXIT_FAILURE);
	}
	// Check outputs.
	nPorts = midiout->getPortCount();
	std::cout << "\nThere are " << nPorts << " MIDI output ports available.\n";
	for (unsigned int i = 0; i < nPorts; i++) {
		try {
			portName = midiout->getPortName(i);
		} catch (RtMidiError &error) {
			error.printMessage();
			goto cleanup;
		}
		std::cout << "  Output Port #" << i + 1 << ": " << portName << '\n';
	}
	std::cout << '\n';
	// Clean up
	cleanup:
	delete midiout;
}
