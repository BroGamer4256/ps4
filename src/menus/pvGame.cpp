#include "diva.h"
#include "menus.h"

namespace pvGame {
using namespace diva;
i32 prcInfoId = 0;
InputType previousInputType;

FUNCTION_PTR (bool, isPractice, 0x1401E7B90);

bool
PVGameInit (u64 a1) {
	if (isPractice ()) {
		InputType input   = diva::getInputType ();
		previousInputType = input;
		char buf[128];
		sprintf (buf, "prc_info_%02d", (i32)input);
		diva::AetLayerArgs layer ("AET_NSWGAM_GAME_MAIN", buf, 0xD, AetAction::IN_LOOP);
		layer.play (&prcInfoId);
	}
	return false;
}

bool
PVGameLoop (u64 a1) {
	InputType input = diva::getInputType ();
	if (isPractice () && input != previousInputType) {
		StopAet (&prcInfoId);
		previousInputType = input;
		char buf[128];
		sprintf (buf, "prc_info_%02d", (i32)input);
		diva::AetLayerArgs layer ("AET_NSWGAM_GAME_MAIN", buf, 0xD, AetAction::LOOP);
		layer.play (&prcInfoId);
	}
	return false;
}

bool
PVGameDestroy (u64 a1) {
	StopAet (&prcInfoId);
	return false;
}

void
init () {
	taskAddition addition;
	addition.init    = PVGameInit;
	addition.loop    = PVGameLoop;
	addition.destroy = PVGameDestroy;
	addTaskAddition ("PVGAME", addition);
}
} // namespace pvGame
