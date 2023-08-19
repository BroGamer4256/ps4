#include "diva.h"
#include "menus.h"

namespace result {
using namespace diva;
i32 fsRank;
i32 ctRank;
i32 fsPoints;
i32 ctPoints;
i32 gainedPoints;

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

HOOK (i32, GetPoints, 0x15DE80210, i32 a1, i32 a2, i32 a3) {
	gainedPoints = originalGetPoints (a1, a2, a3);
	return gainedPoints;
}

HOOK (i32 *, GetRankData, 0x1401E7C50) {
	PvDbEntry **pvData;
	asm ("mov %0, qword ptr [rbx + 0x100]" : "=g"(pvData));
	i32 ps4DlcType = (*pvData)->pack;

	bool isFutureSound = ps4DlcType == 1;
	bool isPs4Dlc      = ps4DlcType != 0;

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

	if (isPs4Dlc) {
		data[0]  = gainedPoints;             // Gained points
		data[1]  = points;                   // Current points
		data[2]  = rank;                     // Current rank
		data[4]  = pointsRequired[rank - 1]; // Points required for current rank
		data[5]  = pointsRequired[rank];     // Points required for next rank
		data[14] = data[0] > 0;              // Has gained points
	} else {
		data[0]  = 0;
		data[1]  = 0;
		data[2]  = rank;
		data[4]  = 0;
		data[5]  = 1;
		data[14] = 0;
	}

	return data;
}

HOOK (void, PlayRankGauge, 0x1402377E0, u64 a1) {
	auto pvData = **(PvDbEntry ***)(a1 + 0x100);
	if (pvData->pack == 0) return;

	originalPlayRankGauge (a1);
}

HOOK (void, PlayRankGaugeLoop, 0x140236F80, u64 a1) {
	auto pvData = **(PvDbEntry ***)(a1 + 0x100);
	if (pvData->pack == 0) return;

	originalPlayRankGaugeLoop (a1);
}

void
init () {
	INSTALL_HOOK (GetStageResultSwitch);
	INSTALL_HOOK (StageResultSwitchFinished);
	INSTALL_HOOK (StageResultSwitchLoaded);
	INSTALL_HOOK (GetPoints);
	INSTALL_HOOK (GetRankData);
	INSTALL_HOOK (PlayRankGauge);
	INSTALL_HOOK (PlayRankGaugeLoop);

	diva::taskAddition gameResultAddition;
	gameResultAddition.loop = GameResultLoop;
	diva::addTaskAddition ("GameResultTask", gameResultAddition);

	diva::taskAddition pvGameAddition;
	pvGameAddition.init = PVGameInit;
	diva::addTaskAddition ("PVGAME", pvGameAddition);
}
} // namespace result
