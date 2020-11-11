#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define seconds				(uint32_t)10
#define env_duration		0.2f

typedef struct WAVE
{
	char ChunkID[4];
	uint32_t ChunkSize;
	char Format[4];
	char Subchunk1ID[4];
	uint32_t Subchunk1Size;
	uint16_t AudioFormat;
	uint16_t NumChannels;
	uint32_t SampleRate;
	uint32_t ByteRate;
	uint16_t BlockAlign;
	uint16_t BitsPerSample;
	char Subchunk2ID[4];
	uint32_t Subchunk2Size;
} WAVE;

void write_to_file(WAVE *wav, int16_t *data)
{
	FILE *file = fopen("audio.wav", "wb");
	if (!file)
	{
		printf("Error opening file!!!\n");
		system("pause");
		exit(0);
	}

	fwrite(wav, sizeof(WAVE), 1, file);
	fwrite(data, wav->Subchunk2Size, 1, file);
	fclose(file);
}

void start_linear_envelope(int16_t *data, uint32_t env_samples)
{
	for (uint32_t i = 0; i < env_samples; i++)
	{
		data[i] = (int16_t)((float)data[i] * i / (env_samples));
	}
}

void stop_linear_envelope(int16_t *data, uint32_t env_samples, uint32_t total_samples)
{
	for (uint32_t i = total_samples - env_samples; i < total_samples; i++)
	{
		data[i] = (int16_t)((float)data[i] * ((float)i / (-env_samples) + (total_samples) / (env_samples)));
	}
}

int main(int argc, char** argv)
{
	WAVE *wav = malloc(sizeof(WAVE));
	if (!wav)
	{
		printf("Error allocating memory for WAVE struct!!!\n");
		system("pause");
		return 0;
	}

	wav->SampleRate = 22050;
	wav->NumChannels = 1;
	wav->BitsPerSample = 16;
	wav->AudioFormat = 1;
	wav->Subchunk1Size = 16;
	wav->Subchunk2Size = wav->SampleRate * seconds * wav->NumChannels * wav->BitsPerSample / 8;
	wav->BlockAlign = wav->NumChannels * wav->BitsPerSample / 8;
	wav->ByteRate = wav->SampleRate * wav->NumChannels * wav->BitsPerSample / 8;
	wav->ChunkSize = 4 + (8 + wav->Subchunk1Size) + (8 + wav->Subchunk2Size);
	strncpy(wav->ChunkID, "RIFF", 4);
	strncpy(wav->Format, "WAVE", 4);
	strncpy(wav->Subchunk1ID, "fmt ", 4);
	strncpy(wav->Subchunk2ID, "data", 4);

	int16_t *data = malloc(wav->Subchunk2Size);
	if (!data)
	{
		printf("Error allocating memory for data!!!\n");
		system("pause");
		return 0;
	}

	float phase = 0;
	for (uint32_t i = 0; i < wav->SampleRate * seconds; i++)
	{
		phase = 300.0f * sin(1.0f * seconds * 2.0f * M_PI * i / (wav->SampleRate * seconds));
		data[i] = 0x7ff0 * sin(400.0f * seconds * 2.0f * M_PI * i / (wav->SampleRate * seconds) + phase);
	}

	start_linear_envelope(data, env_duration * wav->SampleRate);
	stop_linear_envelope(data, env_duration * wav->SampleRate, wav->SampleRate * seconds);

	write_to_file(wav, data);

	free(data);
	free(wav);
	printf("Process completed!!!\n");
	return 0;
}
