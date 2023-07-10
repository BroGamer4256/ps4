#include "diva.h"
#include "menus.h"

namespace genericDialog {
using namespace diva;
i32 helpBaseId     = 0;
u64 *GenericDialog = (u64 *)0x14CC103A0;
bool playedOut     = false;

bool
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

	if (auto layer = compositionData->find (string ("p_help_img_01_c"))) {
		if (auto aet = aets->find (imgAetLayerArgs->id)) {
			aet.value ()->position = layer.value ()->position;
			aet.value ()->color.w  = layer.value ()->opacity;
		}
	}

	return false;
}

HOOK (void, GenericDialogPlay, 0x15ECE1320, void *a1, i32 dialogId) {
	AetLayerArgs helpBaseArgs ("AET_NSWGAM_CMN_MAIN", "cmn_win_help_base", 0x13, AetAction::IN_LOOP);
	helpBaseArgs.play (&helpBaseId);
	playedOut = false;

	originalGenericDialogPlay (a1, dialogId);
}

HOOK (bool, DestroyGenericDialog, 0x1401B0480) {
	if (*GenericDialog == 0) return false;

	if (!playedOut) {
		AetLayerArgs helpBaseArgs ("AET_NSWGAM_CMN_MAIN", "cmn_win_help_base", 0x13, AetAction::OUT_ONCE);
		helpBaseArgs.play (&helpBaseId);

		playedOut = true;
	}

	if (auto helpBase = aets->find (helpBaseId)) {
		if (helpBase.value ()->currentFrame >= helpBase.value ()->layer->endTime - 1) {
			StopAet (&helpBaseId);
			return originalDestroyGenericDialog ();
		}
	}
	return true;
}

void
init () {
	taskAddition addition;
	addition.display = GenericDialogDisplay;
	addTaskAddition ("GENERIC_DIALOG_SWITCH", addition);

	INSTALL_HOOK (GenericDialogPlay);
	INSTALL_HOOK (DestroyGenericDialog);

	WRITE_MEMORY (0x1401DEFB9, u8, 0x0F, 0xB6, 0xC0, 0x90, 0x90); // MOVAX EAX, AL
}
} // namespace genericDialog
