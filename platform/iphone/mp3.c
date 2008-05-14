// (c) Copyright 2007 notaz, All rights reserved.
// Free for non-commercial use.

// For commercial use, separate licencing terms must be obtained.

#include <stdio.h>
#include <string.h>

// might be called before initialization
int mp3_get_bitrate(FILE *f, int size)
{
	return 128;
}

void mp3_start_play(FILE *f, int pos)
{
}


void mp3_update(int *buffer, int length, int stereo)
{
}


int mp3_get_offset(void) // 0-1023
{
	return 0;
}


