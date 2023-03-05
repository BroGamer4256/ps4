#include "diva.h"
#include "menus.h"

namespace pause {
typedef enum PauseAction : i32 {
	Load         = 0,
	Unload       = 1,
	ButtonChange = 2,
	ButtonSelect = 3,
} PauseAction;

void *pauseMenuBackground = calloc (1, 0x1024);
i32 pauseMenuBackgroundId = 0;
bool playedOut            = 0;
HOOK (void, __stdcall, LoadPauseBackground, 0x1406570E0, u64 a1, bool playOut) {
	if (playOut && !playedOut) {
		playedOut = true;
		CreateAetLayerData (pauseMenuBackground, 0x51C, "pause_win_add_base", 0x12, AETACTION_OUT);
		*(u64 *)((u64)pauseMenuBackground + 0x148) = a1 + 0xB0;
		pauseMenuBackgroundId                      = PlayAetLayer (pauseMenuBackground, pauseMenuBackgroundId);
	} else {
		playedOut = false;
		CreateAetLayerData (pauseMenuBackground, 0x51C, "pause_win_add_base", 0x12, AETACTION_IN_LOOP);
		*(u64 *)((u64)pauseMenuBackground + 0x148) = a1 + 0xB0;
		pauseMenuBackgroundId                      = PlayAetLayer (pauseMenuBackground, pauseMenuBackgroundId);
	}
	originalLoadPauseBackground (a1, playOut);
}

HOOK (void, __stdcall, PauseExit, 0x14065B810, u64 a1) {
	if (!playedOut) {
		playedOut = true;
		CreateAetLayerData (pauseMenuBackground, 0x51C, "pause_win_add_base", 0x12, AETACTION_OUT);
		*(u64 *)((u64)pauseMenuBackground + 0x148) = a1 + 0xB0;
		pauseMenuBackgroundId                      = PlayAetLayer (pauseMenuBackground, pauseMenuBackgroundId);
	}
	originalPauseExit (a1);
}

HOOK (void, __stdcall, PauseDestroy, 0x14065B100, u64 a1) {
	StopAet (&pauseMenuBackgroundId);
	originalPauseDestroy (a1);
}

void
init () {
	INSTALL_HOOK (LoadPauseBackground);
	INSTALL_HOOK (PauseExit);
	INSTALL_HOOK (PauseDestroy);
}
} // namespace pause
