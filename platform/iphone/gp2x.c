/**
 * All this is mostly based on rlyeh's minimal library.
 * Copied here to review all his code and understand what's going on.
**/

/*

  GP2X minimal library v0.A by rlyeh, (c) 2005. emulnation.info@rlyeh (swap it!)

  Thanks to Squidge, Robster, snaff, Reesy and NK, for the help & previous work! :-)

  License
  =======

  Free for non-commercial projects (it would be nice receiving a mail from you).
  Other cases, ask me first.

  GamePark Holdings is not allowed to use this library and/or use parts from it.

*/


#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include "gp2x.h"
#include "../common/arm_utils.h"

#include "app_iPhone.h"

void *gp2x_screen = NULL;
short __attribute__((aligned(4))) sndOutBuffer[2*44100/50];
short* sndOutPtr = NULL;
static int lastlen = 0;

/* video stuff */
void gp2x_video_flip(void)
{
#if 0
	if(preferences.landscape)
	{
		int x, y;
		unsigned short *screen_ptr = (unsigned short *)gp2x_screen;
		unsigned short *current_scanline_ptr = screen_ptr;
		unsigned short *current_target_ptr = (unsigned short *)BaseAddress;
		for( x = 0; x < 320; x++ )
		{
			for( y = 0; y < 240; y++ )
			{
				*current_target_ptr++ = current_scanline_ptr[(320 * (240-y)) + (x)];
			}
		}
	}
	else
#endif
	{
		memcpy(BaseAddress, gp2x_screen, 320*240*2);
	}

	updateScreen();
}

/* doulblebuffered flip */
void gp2x_video_flip2(void)
{
#if 0
	if(preferences.landscape)
	{
		int x, y;
		unsigned short *screen_ptr = (unsigned short *)gp2x_screen;
		unsigned short *current_scanline_ptr = screen_ptr;
		unsigned short *current_target_ptr = (unsigned short *)BaseAddress;
		for( x = 0; x < 320; x++ )
		{
			for( y = 0; y < 240; y++ )
			{
				*current_target_ptr++ = current_scanline_ptr[(320 * (240-y)) + (x)];
			}
		}
	}
	else
#endif
	{
		memcpy(BaseAddress, gp2x_screen, 320*240*2);
	}

	updateScreen();
}


void gp2x_video_changemode2(int bpp)
{
}


void gp2x_video_changemode(int bpp)
{
}


void gp2x_video_setpalette(int *pal, int len)
{
}


// TV Compatible function //
void gp2x_video_RGB_setscaling(int ln_offs, int W, int H)
{
}


void gp2x_video_wait_vsync(void)
{
}


void gp2x_video_flush_cache(void)
{
}


void gp2x_memcpy_buffers(int buffers, void *data, int offset, int len)
{
}


void gp2x_memcpy_all_buffers(void *data, int offset, int len)
{
}


void gp2x_memset_all_buffers(int offset, int byte, int len)
{
}


void gp2x_pd_clone_buffer2(void)
{
}


unsigned long gp2x_joystick_read(int allow_usb_joy)
{
	return joystick_read();
}

static int s_oldrate = 0, s_oldbits = 0, s_oldstereo = 0;

void gp2x_stop_sound()
{
	app_MuteSound();
}

void gp2x_start_sound()
{
	lastlen = 0;
	sndOutPtr = sndOutBuffer;
	app_DemuteSound();
#if 0
	int frag = 0, bsize, buffers;

	// if no settings change, we don't need to do anything
	if (rate == s_oldrate && s_oldbits == bits && s_oldstereo == stereo) return;

	if (sounddev > 0) close(sounddev);
	sounddev = open("/dev/dsp", O_WRONLY|O_ASYNC);
	if (sounddev == -1)
		printf("open(\"/dev/dsp\") failed with %i\n", errno);

	ioctl(sounddev, SNDCTL_DSP_SETFMT, &bits);
	ioctl(sounddev, SNDCTL_DSP_SPEED,  &rate);
	ioctl(sounddev, SNDCTL_DSP_STEREO, &stereo);
	// calculate buffer size
	buffers = 16;
	bsize = rate / 32;
	if (rate > 22050) { bsize*=4; buffers*=2; } // 44k mode seems to be very demanding
	while ((bsize>>=1)) frag++;
	frag |= buffers<<16; // 16 buffers
	ioctl(sounddev, SNDCTL_DSP_SETFRAGMENT, &frag);
	usleep(192*1024);

	printf("gp2x_set_sound: %i/%ibit/%s, %i buffers of %i bytes\n",
		rate, bits, stereo?"stereo":"mono", frag>>16, 1<<(frag&0xffff));

	s_oldrate = rate; s_oldbits = bits; s_oldstereo = stereo;
#endif
}


void gp2x_sound_write(void *buff, int len)
{	
	if( (unsigned long)sndOutPtr + lastlen + len <= (unsigned long)sndOutBuffer + ((2*44100/50)*2) )
	{
		sndOutPtr += lastlen>>1;
		memcpy(sndOutPtr, buff, len);
	}
	else
	{
		sndOutPtr = sndOutBuffer;
		memcpy(sndOutPtr, buff, len);
	}
	
	lastlen = len;	
//	write(sounddev, buff, len);
}

void gp2x_sound_sync(void)
{
//	ioctl(sounddev, SOUND_PCM_SYNC, 0);
}

void gp2x_sound_volume(int l, int r)
{
}

/* common */
void gp2x_init(void)
{
	setpriority(PRIO_PROCESS, 0, -20);
	gp2x_screen = (unsigned short*)malloc(320*240*2);
}

char *ext_menu = 0, *ext_state = 0;

void gp2x_deinit(void)
{
	if(gp2x_screen != NULL) free(gp2x_screen);

	gp2x_screen = NULL;
	app_MuteSound();
}

/* lprintf */
void lprintf(const char *fmt, ...)
{
	va_list vl;

	va_start(vl, fmt);
	vprintf(fmt, vl);
	va_end(vl);
}

