#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

#define header_size			(uint32_t)44
#define seconds				(uint32_t)10
#define sample_rate			(uint32_t)22050
#define channels			(uint32_t)1
#define bytes_per_sample	(uint32_t)2
#define env_duration		0.2f
#define samples 			(uint32_t)(sample_rate * seconds)
#define data_size			(uint32_t)(samples * bytes_per_sample * channels)
#define total_size			(uint32_t)(data_size + header_size)

void write_to_file(int16_t *data)
{
	uint8_t header[header_size] =
	{
		'R', 'I', 'F', 'F',
		(uint32_t)(total_size - 8) & 0xff,	// total size of the file - 8
		((uint32_t)(total_size - 8) >> 8) & 0xff,
		((uint32_t)(total_size - 8) >> 16) & 0xff,
		((uint32_t)(total_size - 8) >> 24) & 0xff,
		'W', 'A', 'V', 'E', 'f', 'm', 't', ' ',
		16 & 0xff,	//the rest of the fmt subchunk
		(16 >> 8) & 0xff,
		(16 >> 16) & 0xff,
		(16 >> 24) & 0xff,
		1 & 0xff,	//1 for PCM
		(1 >> 8) & 0xff,
		((uint16_t)channels) & 0xff,	//number of channels
		((uint16_t)channels >> 8) & 0xff,
		(uint32_t)(sample_rate) & 0xff,	// sample rate
		((uint32_t)(sample_rate) >> 8) & 0xff,
		((uint32_t)(sample_rate) >> 16) & 0xff,
		((uint32_t)(sample_rate) >> 24) & 0xff,
		(uint32_t)(sample_rate * channels * bytes_per_sample) & 0xff,	// byte rate
		((uint32_t)(sample_rate * channels * bytes_per_sample) >> 8) & 0xff,
		((uint32_t)(sample_rate * channels * bytes_per_sample) >> 16) & 0xff,
		((uint32_t)(sample_rate * channels * bytes_per_sample) >> 24) & 0xff,
		(uint16_t)(channels * bytes_per_sample) & 0xff,	//block align
		((uint16_t)(channels * bytes_per_sample) >> 8) & 0xff,
		(uint16_t)(8 * bytes_per_sample) & 0xff,	//bits per sample
		((uint16_t)(8 * bytes_per_sample) >> 8) & 0xff,
		'd', 'a', 't', 'a',
		(uint32_t)(data_size) & 0xff,	// actual data size
		((uint32_t)(data_size) >> 8) & 0xff,
		((uint32_t)(data_size) >> 16) & 0xff,
		((uint32_t)(data_size) >> 24) & 0xff
	};

	FILE *file = fopen("audio.wav", "wb");
	fwrite(&header[0], header_size, 1, file);
	fwrite(data, data_size, 1, file);
	fclose(file);
}

int main(int argc, char** argv)
{
	int16_t *data = malloc(data_size);

	for (uint32_t i = 0; i < samples; i++)
	{
		data[i] = 0x7ff0 * sin(600.0f * seconds * 2.0f * M_PI * i / samples + (sin(1.0f * seconds * 2.0f * M_PI * i / samples) * 100.0f));
	}

	for (uint32_t i = 0; i < env_duration * sample_rate; i++)
	{
		data[i] = (int16_t)((float)data[i] * i / (env_duration * sample_rate));
	}

	for (uint32_t i = samples - env_duration * sample_rate; i < samples; i++)
	{
		data[i] = (int16_t)((float)data[i] * ((float)i / (-env_duration * sample_rate) + (seconds * sample_rate) / (env_duration * sample_rate)));
	}

	write_to_file(data);

	free(data);

	//system("pause");
	return 0;
}
