/*
 * array_writer.cpp
 *
 *  Created on: Jun 1, 2017
 *      Author: alax
 */

#include "array_writer.h"
#include <fstream>

ArrayWriter::ArrayWriter() {

}

ArrayWriter::~ArrayWriter() {

}


void ArrayWriter::writeArray(std::array<double, 512> input)
{
	auto data = reinterpret_cast<char *>(input.data());
	auto data_size = sizeof(input.at(0)) * input.size();
	std::ofstream outfile_m("output_float.bin", std::ios::binary);
	outfile_m.write(data, data_size);
}


void ArrayWriter::writeArray(std::array<double, 44100> input)
{
	auto data = reinterpret_cast<char *>(input.data());
	auto data_size = sizeof(input.at(0)) * input.size();
	std::ofstream outfile_m("output_float.bin", std::ios::binary);
	outfile_m.write(data, data_size);
}

void ArrayWriter::process(double input)
{
	if(!done)
	{
		i++;
		if(i>start_threshold)
		{
			output_float[i-start_threshold]=input;
			if(i>output_float.size())
			{
				writeArray(output_float);
				done=true;
				i=0;
			}
		}
	}
}
