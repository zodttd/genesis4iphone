// (c) Copyright 2007 notaz, All rights reserved.
// Free for non-commercial use.

// For commercial use, separate licencing terms must be obtained.

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <strings.h>
#include <sys/syslimits.h>

#include "gp2x.h"
#include "menu.h"
#include "../common/menu.h"
#include "../common/emu.h"
#include "emu.h"
#include "version.h"

#include "app_iPhone.h"
#include <Pico/PicoInt.h>
#include <Pico/Patch.h>
#include <zlib/zlib.h>
#include <pthread.h>

extern char *ext_menu, *ext_state;
extern int select_exits;
extern char *PicoConfigFile;
extern char **g_argv;
extern int g_argc;
char save_filename[512];


void parse_cmd_line(int argc, char *argv[])
{
	int x, unrecognized = 0;

	for(x = 1; x < argc; x++)
	{
		if(argv[x][0] == '-')
		{
			if(strcasecmp(argv[x], "-menu") == 0) {
				if(x+1 < argc) { ++x; ext_menu = argv[x]; } /* External Frontend: Program Name */
			}
			else if(strcasecmp(argv[x], "-state") == 0) {
				if(x+1 < argc) { ++x; ext_state = argv[x]; } /* External Frontend: Arguments */
			}
			else if(strcasecmp(argv[x], "-config") == 0) {
				if(x+1 < argc) { ++x; PicoConfigFile = argv[x]; }
			}
			else if(strcasecmp(argv[x], "-selectexit") == 0) {
				select_exits = 1;
			}
			else {
				unrecognized = 1;
				break;
			}
		} else {
			/* External Frontend: ROM Name */
			FILE *f;
			strncpy(romFileName, argv[x], PATH_MAX);
			romFileName[PATH_MAX-1] = 0;
			f = fopen(romFileName, "rb");
			if (f) fclose(f);
			else unrecognized = 1;
			engineState = PGS_ReloadRom;
			break;
		}
	}

	if (unrecognized) {
		printf("\n\n\nPicoDrive v" VERSION " (c) notaz, 2006-2007\n");
		printf("usage: %s [options] [romfile]\n", argv[0]);
		printf( "options:\n"
				"-menu <menu_path> launch a custom program on exit instead of default gp2xmenu\n"
				"-state <param>    pass '-state param' to the menu program\n"
				"-config <file>    use specified config file instead of default 'picoconfig.bin'\n"
				"                  see currentConfig_t structure in emu.h for the file format\n"
				"-selectexit       pressing SELECT will exit the emu and start 'menu_path'\n");
	}
}

void r9testfunc(void);

int iphone_main(char* filename)
{
	FILE *f;
	emu_ReadConfig(0, 0);
	gp2x_init();
	emu_Init();
	menu_init();

	engineState = PGS_Menu;

	//if (argc > 1)
	//	parse_cmd_line(g_argc, g_argv);
	strncpy(romFileName, filename, PATH_MAX);
	romFileName[PATH_MAX-1] = 0;
	
	if( (!strcasecmp(romFileName + (strlen(romFileName)-7), ".mds.gz")) )
	{
		unsigned long pos;
		sprintf(save_filename, "%s", romFileName);
		pos = strlen(romFileName)-21;
		romFileName[pos] = '\0';
	}
	
	f = fopen(romFileName, "rb");
	if (f) 
	{
		engineState = PGS_ReloadRom;
		fclose(f);
	}

	for (;;)
	{
		switch (engineState)
		{
			case PGS_Menu:
				menu_loop();
				break;

			case PGS_ReloadRom:
				if (emu_ReloadRom())
				{
					engineState = PGS_Running;
					if (strlen(save_filename) > 0) 
    				{
						void *PmovFile = NULL;
						if( (PmovFile = gzopen(save_filename, "rb")) ) {
								emu_setSaveStateCbs(1);
						}
						
						if(PmovFile) {
							PmovState(6, PmovFile);
							areaClose(PmovFile);
							PmovFile = 0;
							Pico.m.dirtyPal=1;
						}
						save_filename[0] = '\0';
    				}
				} 
				else 
				{
					printf("PGS_ReloadRom == 0\n");
					engineState = PGS_Menu;
				}
				break;

			case PGS_RestartRun:
				engineState = PGS_Running;

			case PGS_Running:
				emu_Loop();
				break;

			case PGS_Quit:
				goto endloop;

			default:
				printf("engine got into unknown state (%i), exitting\n", engineState);
				goto endloop;
		}
	}

	endloop:

	//if( !__emulation_run ) 
	{
		char buffer[512];
		char tmpfilename[512];
		time_t curtime;
		struct tm *loctime;
		void *PmovFile = NULL;

		curtime = time (NULL);
		loctime = localtime (&curtime);
		strftime (buffer, 260, "%y%m%d-%I%M%p", loctime);
		sprintf(tmpfilename, "%s-%s.mds.gz", romFileName, buffer);

		if( (PmovFile = gzopen(tmpfilename, "wb")) ) {
				emu_setSaveStateCbs(1);
				gzsetparams(PmovFile, 9, Z_DEFAULT_STRATEGY);
		}
		if(PmovFile) {
			PmovState(5, PmovFile);
			areaClose(PmovFile);
			PmovFile = 0;
#ifndef NO_SYNC
			sync();
#endif
		}
		app_SetSvsFile(tmpfilename);
	}
			
	emu_Deinit();
	gp2x_deinit();

	//if( !__emulation_run ) 
	{
		pthread_exit(NULL);
	}
	return 0;
}
