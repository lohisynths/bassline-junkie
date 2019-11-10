/*
 * clipping.h
 *
 *  Created on: May 31, 2017
 *      Author: alax
 */

#ifndef SRC_SYNTH_CLIPPING_H_
#define SRC_SYNTH_CLIPPING_H_

#include <algorithm>


inline void check_clipping(double input, const char* function, int line)
{
	if(input > 1)
		std::cout << function << ":" << line << "\tCLIP >  1\n";
	else if(input < -1)
		std::cout << function << ":" << line << "\tCLIP < -1\n";
}
inline void check_clipping(std::array<double, 512> input, const char* function, int line)
{
	std::for_each(std::begin(input), std::end(input),
	[&] (double &element)
	{
		check_clipping(element, function, line);
	});
}

struct debug_double
{
    operator double()
    {
		check_clipping(data, __FILE__, __LINE__);
    	return data;
    }

	void operator<<(double in)
	{
		data=in;
		check_clipping(data, __FILE__, __LINE__);
	}
	 double operator=(double in)
	{
		data=in;
		check_clipping(data, __FILE__, __LINE__);
		return data;
	}

	void operator+=(double in)
	{
		data+=in;
		check_clipping(data, __FILE__, __LINE__);
	}

	void operator*=(double in)
	{
		data*=in;
		check_clipping(data, __FILE__, __LINE__);
	}

	double data;
};




#endif /* SRC_SYNTH_CLIPPING_H_ */
