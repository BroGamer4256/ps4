#include "diva.h"
#include "menus.h"

namespace genericDialog {
using namespace diva;
i32 helpBaseId = 0;
bool playedOut = false;

void
GenericDialogDisplay (u64 This) {
	auto dialogAetLayerArgs = (AetLayerArgs *)(This + 0x78);
	auto pageNo             = *(u8 *)(This + 0x479);
	auto imgAetLayerArgs    = (AetLayerArgs *)(*(u64 *)(This + 0x498) + (0x1F8 * pageNo));
	auto compositionData    = (AetComposition *)(This + 0x468);

	compositionData->~AetComposition ();
	u8 buf[sizeof (AetComposition)];
	auto comp = new (buf) AetComposition;
	GetComposition (comp, dialogAetLayerArgs->id);
	*compositionData = *comp;

	if (auto layer = compositionData->find (string ("p_help_img_01_c")))
		if (auto aet = aets->find (imgAetLayerArgs->id)) aet.value ()->position = layer.value ()->position;
}

bool
GenericDialogDestory (u64 This) {
	if (!playedOut) {
		AetLayerArgs helpBaseArgs ("AET_NSWGAM_CMN_MAIN", "cmn_win_help_base", 0x13, AetAction::OUT_ONCE);
		helpBaseArgs.play (&helpBaseId);

		auto dialogAetLayerArgs = (AetLayerArgs *)(This + 0x78);
		*dialogAetLayerArgs     = AetLayerArgs ("AET_NSWGAM_CMN_MAIN", "cmn_win_help", 0x13, AetAction::OUT_ONCE);
		dialogAetLayerArgs->play (&dialogAetLayerArgs->id);

		playedOut = true;
	}

	return false;
}

HOOK (void, GenericDialogPlay, 0x15ECE1320, void *a1, i32 dialogId) {
	AetLayerArgs helpBaseArgs ("AET_NSWGAM_CMN_MAIN", "cmn_win_help_base", 0x13, AetAction::IN_LOOP);
	helpBaseArgs.play (&helpBaseId);
	playedOut = false;

	originalGenericDialogPlay (a1, dialogId);
}

void
init () {
	taskAddition addition;
	addition.destroy = GenericDialogDestory;
	addition.display = GenericDialogDisplay;
	addTaskAddition ("GENERIC_DIALOG_SWITCH", addition);

	INSTALL_HOOK (GenericDialogPlay);
}
} // namespace genericDialog
