#include "diva.h"
#include "menus.h"

namespace pause {
typedef enum PauseAction : i32 {
	Load         = 0,
	Unload       = 1,
	ButtonChange = 2,
	ButtonSelect = 3,
} PauseAction;

using diva::AetAction;
using diva::aetLayer;

i32 pauseMenuBackgroundId = 0;
bool playedOut            = 0;
HOOK (void, __stdcall, LoadPauseBackground, 0x1406570E0, u64 a1, bool playOut) {
	if (playOut && !playedOut) {
		playedOut = true;
		aetLayer pauseMenuBackground (0x51C, "pause_win_add_base", 0x12, AetAction::OUT_ONCE);
		pauseMenuBackground.frameRateControl = (void *)(a1 + 0xB0);
		pauseMenuBackground.play (&pauseMenuBackgroundId);
	} else {
		playedOut = false;
		aetLayer pauseMenuBackground (0x51C, "pause_win_add_base", 0x12, AetAction::IN_LOOP);
		pauseMenuBackground.frameRateControl = (void *)(a1 + 0xB0);
		pauseMenuBackground.play (&pauseMenuBackgroundId);
	}
	originalLoadPauseBackground (a1, playOut);
}

HOOK (void, __stdcall, PauseExit, 0x14065B810, u64 a1) {
	if (!playedOut) {
		playedOut = true;
		aetLayer pauseMenuBackground (0x51C, "pause_win_add_base", 0x12, AetAction::OUT_ONCE);
		pauseMenuBackground.frameRateControl = (void *)(a1 + 0xB0);
		pauseMenuBackground.play (&pauseMenuBackgroundId);
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
