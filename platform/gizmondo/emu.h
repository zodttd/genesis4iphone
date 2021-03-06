// (c) Copyright 2006-2007 notaz, All rights reserved.
// Free for non-commercial use.

// For commercial use, separate licencing terms must be obtained.



// engine states
enum TPicoGameState {
	PGS_Paused = 1,
	PGS_Running,
	PGS_Quit,
	PGS_KeyConfig,
	PGS_ReloadRom,
	PGS_Menu,
	PGS_RestartRun,
};

extern unsigned char gfx_buffer[321*240*2*2];
extern char romFileName[];
extern int engineState;


void emu_Init(void);
void emu_Deinit(void);
void emu_Loop(void);
void emu_ResetGame(void);
void emu_forcedFrame(void);

void emu_stateCb(const char *str);

