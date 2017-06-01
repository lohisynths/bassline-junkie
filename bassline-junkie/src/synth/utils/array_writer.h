/*
 * array_writer.h
 *
 *  Created on: Jun 1, 2017
 *      Author: alax
 */

#ifndef SRC_SYNTH_UTILS_ARRAY_WRITER_H_
#define SRC_SYNTH_UTILS_ARRAY_WRITER_H_

#include <array>

class ArrayWriter {
public:
	ArrayWriter();
	virtual ~ArrayWriter();

	void process(double input);
	void writeArray(std::array<double, 44100> input);
	void writeArray(std::array<double, 512> input);


	bool done=false;
	size_t i=0;
	size_t start_threshold = 44100;

	std::array<double, 44100> output_float;
};

#endif /* SRC_SYNTH_UTILS_ARRAY_WRITER_H_ */
