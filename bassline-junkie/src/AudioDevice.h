/*
 * AudioDevice.h
 *
 *  Created on: Feb 7, 2017
 *      Author: alax
 */

#ifndef AUDIODEVICE_H_
#define AUDIODEVICE_H_

#include <alsa/asoundlib.h>
#include <string>
#include <array>






//
const unsigned int format_bits = 32; //snd_pcm_format_width(*m_format);
const unsigned int maxval = (1U << (format_bits - 1U)) - 1U;


extern void tick(int32_t **data, unsigned int channels, unsigned int count);

class AudioDevice
{
public:
	AudioDevice();
	virtual ~AudioDevice();

	int setup();

	int set_hwparams(snd_pcm_t *handle, snd_pcm_hw_params_t *params,
			snd_pcm_access_t access);

	int set_swparams(snd_pcm_t *handle, snd_pcm_sw_params_t *swparams);

	void close();

	snd_pcm_t* get_handle();
	signed short* get_samples();
	snd_pcm_channel_area_t* get_areas();
	int get_period_size();
	snd_pcm_format_t *get_format();
	unsigned int get_channels();
	unsigned int get_sample_rate();


	int prepare_mmap();

	int aval();
	void play(std::array<int32_t, 512> &arr);


	snd_pcm_t *handle;
	signed short *samples;
	snd_pcm_channel_area_t *areas;

private:



	std::string device;
	snd_pcm_format_t format;
	unsigned int rate;
	unsigned int channels;
	int verbose;
	int resample;
	int period_event;

	snd_pcm_sframes_t buffer_size;
	snd_pcm_sframes_t period_size;

	snd_output_t *output;

};

#endif /* AUDIODEVICE_H_ */
