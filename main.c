#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define seconds				(uint8_t)10
#define freq				(uint32_t)44100
#define bps					(uint16_t)16
#define channels			(uint16_t)2
#define env_duration		0.05f
#define env_samples			(uint32_t)(env_duration * freq)
#define total_samples		(uint32_t)(seconds * freq)

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
	int16_t data[total_samples * bps * channels / 8];
} WAVE;

void write_to_file(WAVE *wav)
{
	FILE *file = fopen("audio.wav", "wb");
	if (!file)
	{
		printf("Error opening file!!!\n");
		system("pause");
		exit(0);
	}

	fwrite(wav, sizeof(WAVE), 1, file);
	fclose(file);
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

	wav->SampleRate = freq;
	wav->NumChannels = channels;
	wav->BitsPerSample = bps;
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

	float phase = 0;
	for (uint32_t i = 0; i < total_samples; i++)
	{
		phase = 300.0f * sin(1.0f * seconds * 2.0f * M_PI * i / total_samples);
		wav->data[i * 2] = 0x7ff0 * sin(400.0f * seconds * 2.0f * M_PI * i / total_samples + phase);
		wav->data[i * 2 + 1] = 0x7ff0 * sin(400.0f * seconds * 2.0f * M_PI * i / total_samples + phase);

		if (i < env_samples)
		{
			wav->data[i * 2] *= (float)i / env_samples;
			wav->data[i * 2 + 1] *= (float)i / env_samples;
		}
		if (i > total_samples - env_samples)
		{
			wav->data[i * 2] *= -(float)i / env_samples + (float)total_samples / env_samples;
			wav->data[i * 2 + 1] *= -(float)i / env_samples + (float)total_samples / env_samples;
		}
	}

	write_to_file(wav);

	free(wav);
	printf("Process completed!!!\n");
	return 0;
}
