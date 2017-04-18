/*
 * AudioDevice.cpp
 *
 *  Created on: Feb 7, 2017
 *      Author: alax
 */

#include "AudioDevice.h"

AudioDevice::AudioDevice()
{
	handle = NULL;
	samples = NULL;
	areas = NULL;

	//device = "hw:HDMI,3"; /* playback device */
	//device = "default"; /* playback device */

#ifndef __arm__
	device = "hw:PCH,0"; /* playback device */
#else
	device = "default"; /* playback device */
#endif



	format = SND_PCM_FORMAT_S32; /* sample format */
	rate = 44100; /* stream rate */
	channels = 2; /* count of channels */
	//desired_buffer_size = 1024; /* desired buffer size */
	//desired_period_count = 2;
	verbose = 1; /* verbose flag */
	resample = 0; /* enable alsa-lib resampling */
	period_event = 0; /* produce poll event after each period */
	period_size = 512;
	buffer_size = period_size * 2;

	output = NULL;

	setup();
}

AudioDevice::~AudioDevice()
{
	this->close();
}

int AudioDevice::setup()
{
	int err;
	snd_pcm_hw_params_t *hwparams;
	snd_pcm_sw_params_t *swparams;
	unsigned int chn;
	snd_pcm_hw_params_alloca(&hwparams);
	snd_pcm_sw_params_alloca(&swparams);

	err = snd_output_stdio_attach(&output, stdout, 0);
	if (err < 0)
	{
		printf("Output failed: %s\n", snd_strerror(err));
		return 0;
	}
	printf("Playback device is %s\n", device.c_str());
	printf("Stream parameters are %iHz, %s, %i channels\n", rate,
			snd_pcm_format_name(format), channels);
	printf("Using transfer method: %s\n", "SND_PCM_ACCESS_MMAP_INTERLEAVED");
	if ((err = snd_pcm_open(&handle, device.c_str(), SND_PCM_STREAM_PLAYBACK, 0))
			< 0)
	{
		printf("Playback open error: %s\n", snd_strerror(err));
		return 0;
	}

	//SND_PCM_ACCESS_MMAP_NONINTERLEAVED
	if ((err = set_hwparams(handle, hwparams, SND_PCM_ACCESS_MMAP_INTERLEAVED))
			< 0)
	{
		printf("Setting of hwparams failed: %s\n", snd_strerror(err));
		exit(EXIT_FAILURE);
	}
	if ((err = set_swparams(handle, swparams)) < 0)
	{
		printf("Setting of swparams failed: %s\n", snd_strerror(err));
		exit(EXIT_FAILURE);
	}
	if (verbose > 0)
		snd_pcm_dump(handle, output);
	samples = static_cast<signed short *>(malloc(
			(period_size * channels * snd_pcm_format_physical_width(format))
					/ 8));
	if (samples == NULL)
	{
		printf("No enough memory\n");
		exit(EXIT_FAILURE);
	}

	areas = static_cast<snd_pcm_channel_area_t*>(calloc(channels,
			sizeof(snd_pcm_channel_area_t)));
	if (areas == NULL)
	{
		printf("No enough memory\n");
		exit(EXIT_FAILURE);
	}
	for (chn = 0; chn < channels; chn++)
	{
		areas[chn].addr = samples;
		areas[chn].first = chn * snd_pcm_format_physical_width(format);
		areas[chn].step = channels * snd_pcm_format_physical_width(format);
	}
	return 1;
}

void AudioDevice::close()
{
	free(areas);
	free(samples);
	snd_pcm_close(handle);
}

int AudioDevice::set_hwparams(snd_pcm_t *handle, snd_pcm_hw_params_t *params,
		snd_pcm_access_t access)
{
	unsigned int rrate;
	snd_pcm_uframes_t size;
	int err, dir;
	/* choose all parameters */
	err = snd_pcm_hw_params_any(handle, params);
	if (err < 0)
	{
		printf(
				"Broken configuration for playback: no configurations available: %s\n",
				snd_strerror(err));
		return err;
	}
	/* set hardware resampling */
	err = snd_pcm_hw_params_set_rate_resample(handle, params, resample);
	if (err < 0)
	{
		printf("Resampling setup failed for playback: %s\n", snd_strerror(err));
		return err;
	}
	/* set the interleaved read/write format */
	err = snd_pcm_hw_params_set_access(handle, params, access);
	if (err < 0)
	{
		printf("Access type not available for playback: %s\n",
				snd_strerror(err));
		return err;
	}
	/* set the sample format */
	err = snd_pcm_hw_params_set_format(handle, params, format);
	if (err < 0)
	{
		printf("Sample format not available for playback: %s\n",
				snd_strerror(err));
		return err;
	}
	/* set the count of channels */
	err = snd_pcm_hw_params_set_channels(handle, params, channels);
	if (err < 0)
	{
		printf("Channels count (%i) not available for playbacks: %s\n",
				channels, snd_strerror(err));
		return err;
	}
	/* set the stream rate */
	rrate = rate;
	err = snd_pcm_hw_params_set_rate_near(handle, params, &rrate, 0);
	if (err < 0)
	{
		printf("Rate %iHz not available for playback: %s\n", rate,
				snd_strerror(err));
		return err;
	}
	if (rrate != rate)
	{
		printf("Rate doesn't match (requested %iHz, get %iHz)\n", rate, err);
		return -EINVAL;
	}
	/* set the buffer time */

	size = buffer_size;
	//    err = snd_pcm_hw_params_set_buffer_time_near(handle, params, &buffer_time, &dir);
	err = snd_pcm_hw_params_set_buffer_size(handle, params, size);
	if (err < 0)
	{
		printf("Unable to set buffer time %ld for playback: %s\n", size,
				snd_strerror(err));
		return err;
	}
	err = snd_pcm_hw_params_get_buffer_size(params, &size);
	if (err < 0)
	{
		printf("Unable to get buffer size for playback: %s\n",
				snd_strerror(err));
		return err;
	}
	if (size != (snd_pcm_uframes_t) buffer_size)
	{
		printf("Unable to get buffer size for playback: %s\n",
				snd_strerror(err));
		return err;
	}

	// buffer_size = size;

	size = period_size;
	err = snd_pcm_hw_params_set_period_size(handle, params, size, 0);
	if (err < 0)
	{
		printf("Unable to set period size %ld for playback: %s\n", period_size,
				snd_strerror(err));
		return err;
	}
	if (size != (snd_pcm_uframes_t) period_size)
	{
		printf("Unable to get buffer size for playback: %s\n",
				snd_strerror(err));
		return err;
	}

//    /* set the period time */
//    err = snd_pcm_hw_params_set_period_time_near(handle, params, &period_time, &dir);
//    if (err < 0) {
//            printf("Unable to set period time %i for playback: %s\n", period_time, snd_strerror(err));
//            return err;
//    }
	err = snd_pcm_hw_params_get_period_size(params, &size, &dir);
	if (err < 0)
	{
		printf("Unable to get period size for playback: %s\n",
				snd_strerror(err));
		return err;
	}
	period_size = size;
	/* write the parameters to device */
	err = snd_pcm_hw_params(handle, params);
	if (err < 0)
	{
		printf("Unable to set hw params for playback: %s\n", snd_strerror(err));
		return err;
	}
	return 0;
}

int AudioDevice::set_swparams(snd_pcm_t *handle, snd_pcm_sw_params_t *swparams)
{
	int err;
	/* get the current swparams */
	err = snd_pcm_sw_params_current(handle, swparams);
	if (err < 0)
	{
		printf("Unable to determine current swparams for playback: %s\n",
				snd_strerror(err));
		return err;
	}
	/* start the transfer when the buffer is almost full: */
	/* (buffer_size / avail_min) * avail_min */
	err = snd_pcm_sw_params_set_start_threshold(handle, swparams,
			(buffer_size / period_size) * period_size);
	if (err < 0)
	{
		printf("Unable to set start threshold mode for playback: %s\n",
				snd_strerror(err));
		return err;
	}
	/* allow the transfer when at least period_size samples can be processed */
	/* or disable this mechanism when period event is enabled (aka interrupt like style processing) */
	err = snd_pcm_sw_params_set_avail_min(handle, swparams,
			period_event ? buffer_size : period_size);
	if (err < 0)
	{
		printf("Unable to set avail min for playback: %s\n", snd_strerror(err));
		return err;
	}
	/* enable period events when requested */
	if (period_event)
	{
		err = snd_pcm_sw_params_set_period_event(handle, swparams, 1);
		if (err < 0)
		{
			printf("Unable to set period event: %s\n", snd_strerror(err));
			return err;
		}
	}
	/* write the parameters to the playback device */
	err = snd_pcm_sw_params(handle, swparams);
	if (err < 0)
	{
		printf("Unable to set sw params for playback: %s\n", snd_strerror(err));
		return err;
	}
	return 0;
}

snd_pcm_t *AudioDevice::get_handle()
{
	return handle;
}

signed short *AudioDevice::get_samples()
{
	return samples;
}

snd_pcm_channel_area_t *AudioDevice::get_areas()
{
	return areas;
}

int AudioDevice::get_period_size()
{
	return period_size;
}

snd_pcm_format_t *AudioDevice::get_format()
{
	return &format;
}

unsigned int AudioDevice::get_channels()
{
	return channels;
}

unsigned int AudioDevice::get_sample_rate()
{
	return rate;
}

int xrun_recovery(snd_pcm_t *handle, int err)
{
	printf("stream recovery\n");
	if (err == -EPIPE)
	{ /* under-run */
		err = snd_pcm_prepare(handle);
		if (err < 0)
			printf("Can't recovery from underrun, prepare failed: %s\n",
					snd_strerror(err));
		return 0;
	}
	else if (err == -ESTRPIPE)
	{
		while ((err = snd_pcm_resume(handle)) == -EAGAIN)
			sleep(1); /* wait until the suspend flag is released */
		if (err < 0)
		{
			err = snd_pcm_prepare(handle);
			if (err < 0)
				printf("Can't recovery from suspend, prepare failed: %s\n",
						snd_strerror(err));
		}
		return 0;
	}
	return err;
}

static int err, first = 1;

void AudioDevice::play(std::array<int32_t, 512> &arr)
{
	snd_pcm_uframes_t size = period_size;

	while (size > 0)
	{
		snd_pcm_uframes_t frames = size;
		snd_pcm_uframes_t offset;
		const snd_pcm_channel_area_t *my_areas;

		int err = snd_pcm_mmap_begin(handle, &my_areas, &offset, &frames);

		if (err < 0)
		{
			printf("snd_pcm_mmap_begin\n");

			if ((err = xrun_recovery(handle, err)) < 0)
			{
				printf("MMAP begin avail error: %s\n", snd_strerror(err));
				exit(EXIT_FAILURE);
			}
			first = 1;
		}

		int32_t *data[channels];

		for (unsigned int chn = 0; chn < channels; chn++)
		{
			auto adr = chn + offset * channels;
			data[chn] = (((int32_t *) my_areas[chn].addr) + adr);
		}

		for (unsigned int i = 0; i < size; i++)
		{
			int32_t res = arr[i];
			for (unsigned int chn = 0; chn < channels; chn++)
			{
				auto out = data[chn];
				if (chn == 0)
				{
					*out = res;
				}
				else
				{
					*out = res;
				}
				data[chn] += channels;
			}
		}

		snd_pcm_sframes_t commitres = snd_pcm_mmap_commit(handle, offset,
				frames);
		if (commitres < 0 || (snd_pcm_uframes_t) commitres != frames)
		{
			printf("snd_pcm_mmap_commit\n");
			if ((err = xrun_recovery(handle,
					commitres >= 0 ? -EPIPE : commitres)) < 0)
			{
				printf("MMAP commit error: %s\n", snd_strerror(err));
				exit(EXIT_FAILURE);
			}
			first = 1;
		}
		size -= frames;
		//if (size != 0)
		//	std::cout << " size -= frames beware -_- " << size << std::endl;
	}
}

int AudioDevice::aval()
{
	snd_pcm_state_t state = snd_pcm_state(handle);
	if (state == SND_PCM_STATE_XRUN)
	{
		printf("SND_PCM_STATE_XRUN\n");

		err = xrun_recovery(handle, -EPIPE);
		if (err < 0)
		{
			printf("XRUN recovery failed: %s\n", snd_strerror(err));
			return err;
		}
		first = 1;
	}
	else if (state == SND_PCM_STATE_SUSPENDED)
	{
		printf("SND_PCM_STATE_SUSPENDED\n");

		err = xrun_recovery(handle, -ESTRPIPE);
		if (err < 0)
		{
			printf("SUSPEND recovery failed: %s\n", snd_strerror(err));
			return err;
		}
	}

	snd_pcm_sframes_t avail = snd_pcm_avail_update(handle);
	if (avail < 0)
	{
		printf("avail = snd_pcm_avail_update(handle);\n");

		err = xrun_recovery(handle, avail);
		if (err < 0)
		{
			printf("avail update failed: %s\n", snd_strerror(err));
			return err;
		}
		first = 1;
		return 0;
	}
	if (avail < period_size)
	{
		if (first)
		{
			first = 0;
			err = snd_pcm_start(handle);
			if (err < 0)
			{
				printf("Start error: %s\n", snd_strerror(err));
				exit(EXIT_FAILURE);
			}
		}
		else
		{
			err = snd_pcm_wait(handle, -1);
			if (err < 0)
			{
				if ((err = xrun_recovery(handle, err)) < 0)
				{
					printf("snd_pcm_wait error: %s\n", snd_strerror(err));
					exit(EXIT_FAILURE);
				}
				first = 1;
			}
		}
		return 0;
	}

	return 1;

}

