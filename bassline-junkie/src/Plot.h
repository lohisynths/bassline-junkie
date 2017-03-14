/*
 * Plot.h
 *
 *  Created on: Mar 9, 2017
 *      Author: root
 */

#ifndef PLOT_H_
#define PLOT_H_
#define GNUPLOT_ENABLE_PTY

#include <thread>
#include <mutex>

#include <atomic>
#include <deque>

#include "gnuplot-iostream.h"
#include "concurency_helpers.h"

#define maximum_count 512

class Plot
{
public:
	enum class transfer
	{
		block, scroll
	};

	Plot(char const * _name, double _scale_y_min, double _scale_y_max,
			transfer _metod) :
			name(_name), scale_y_min(_scale_y_min), scale_y_max(_scale_y_max), metod(
					_metod)
	{
		for (size_t i = 0; i < maximum_count; i++)
		{
			plot.push_front(std::make_pair(1 - (i / maximum_count), 0));
		}

		if (metod == transfer::scroll)
		{
			// Don't forget to put "\n" at the end of each line!
			gp << "set xrange [0:1]\n";
			//gp << "set yrange [0:1]\n";

			gp << "set yrange [";
			gp << scale_y_min;

			gp << ":";
			gp << scale_y_max;

			gp << "]\n";

			t =
					new std::thread(
							[&]()
							{
								stick_this_thread_to_core(1);
								//set_pthread_params();
								getinfo();

								PrintThread
								{}<< " worked function\n";

								while(1)
								{
									proces();
									std::this_thread::sleep_for( std::chrono::duration<double, std::milli>(30) );
								}
							});
		}
		else
		{
			gp << "set xrange [0:";
			gp << maximum_count;
			gp << "]\n";
			gp << "set yrange [0:1]\n";

			gp << "set yrange [";
			gp << scale_y_min;

			gp << ":";
			gp << scale_y_max;

			gp << "]\n";

		}
	}

	bool update(double input)
	{
		value.store(input);
	}

	bool upload(const std::array<double, 512> &input)
	{
		// Don't forget to put "\n" at the end of each line!

		gp << "plot '-' with lines title '";
		gp << name;
		gp << "'\n";

		gp.send1d(input);

		gp.flush();
	}

	bool proces()
	{
		//, '-' with lines title 'pts_B'\n";
		// '-' means read from stdin.  The send1d() function sends data to gnuplot's stdin.
		//gp << "plot '-' with lines title 'ala_X'\n";
		gp << "plot";
		gp << "'-' with lines title '";
		gp << name;
		gp << "'\n";

		if (plot.size() > maximum_count)
		{
			plot.pop_back();
			for (size_t i = 0; i < plot.size(); i++)
			{
				plot.at(i).first -= 1.0 / maximum_count;
			}
		}

		plot.push_front(std::make_pair(1, value.load()));

		gp.send1d(plot);

		gp.flush();

//		gp.getMouse(mx, my, mb, "Left click to aim arrows, right click to exit.");
//		printf("You pressed mouse button %d at x=%f y=%f\n", mb, mx, my);
//		if(mb < 0) printf("The gnuplot window was closed.\n");

		return mb < 0;

	}

private:
	transfer metod;
	std::mutex m;

	std::thread *t;
	char const *name;
	double scale_y_min, scale_y_max;
	std::atomic<double> value;

	bool mb = true;
	Gnuplot gp;
	std::deque<std::pair<double, double> > plot;

};

#endif /* PLOT_H_ */
