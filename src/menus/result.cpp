#include "diva.h"
#include "menus.h"

namespace result {
using namespace diva;
i32 fsRank;
i32 ctRank;
i32 fsPoints;
i32 ctPoints;

HOOK (u64, GetStageResultSwitch, 0x14064BF50) { return 0x1412C1F00; }
HOOK (bool, StageResultSwitchFinished, 0x14064C0D0, u64 task) { return *(i32 *)(task + 0x68) == 0x5A; }
HOOK (bool, StageResultSwitchLoaded, 0x1401E8060) { return *(i32 *)(implOfGetStageResultSwitch () + 0x68) != 0; }

bool
GameResultLoop (u64 task) {
	if (IsSurvival () && (LifeGauge () == 0 || SurvivalCleared ()) && *(i32 *)(task + 0x68) < 8) {
		*(i32 *)(task + 0x68) = 8;
		*(i32 *)(task + 0x88) = 1;
	}
	return false;
}

bool
PVGameInit (u64 task) {
	GetFSCTRankData (&fsRank, &ctRank, &fsPoints, &ctPoints);
	return false;
}

HOOK (i32 *, GetRankData, 0x1401E7C50) {
	bool isFutureSound;
	asm ("mov %0, byte ptr [rbx + 0x1FF]" : "=g"(isFutureSound));
	i32 rank;
	i32 points;
	i32 *pointsRequired = (i32 *)0x1412B6828;
	pointsRequired += (39 * isFutureSound);
	if (isFutureSound) {
		rank   = fsRank;
		points = fsPoints;
	} else {
		rank   = ctRank;
		points = ctPoints;
	}

	i32 *data = originalGetRankData ();

	data[0]  = *(i32 *)0x14CC08DD0; // Gained points
	data[1]  = points;
	data[2]  = rank;
	data[4]  = pointsRequired[rank - 1];
	data[5]  = pointsRequired[rank];
	data[14] = 1;

	return data;
}
void
init () {
	INSTALL_HOOK (GetStageResultSwitch);
	INSTALL_HOOK (StageResultSwitchFinished);
	INSTALL_HOOK (StageResultSwitchLoaded);
	INSTALL_HOOK (GetRankData);

	diva::taskAddition gameResultAddition;
	gameResultAddition.loop = GameResultLoop;
	diva::addTaskAddition ("GameResultTask", gameResultAddition);

	diva::taskAddition pvGameAddition;
	pvGameAddition.init = PVGameInit;
	diva::addTaskAddition ("PVGAME", pvGameAddition);
}
} // namespace result
