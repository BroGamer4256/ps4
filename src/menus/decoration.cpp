#include "diva.h"
#include "menus.h"

namespace decoration {
using namespace diva;
AetLayerArgs **cmnbgArgs = (AetLayerArgs **)0x14CC5EEA0;
i32 bgId                 = 0;
bool decorationInited    = false;
u8 currScreen            = 1;
i32 footerButtonId       = 0;
i32 anmId                = 0;
i32 sideArrowsId         = 0;

bool
DecorationLoop (u64 This) {
	if (!decorationInited) {
		StopAet (&(*cmnbgArgs)->id);
		AetLayerArgs bgArgs ("AET_NSWGAM_DECORATION_MAIN", "bg", 5, AetAction::NONE);
		bgArgs.play (&bgId);
		decorationInited = true;
	}

	return false;
}

bool
DecorationDisplay (u64 This) {
	u8 screen = *(u8 *)(This + 0x6B);
	if (screen != currScreen) {
		currScreen = screen;

		InputType input = getInputType ();
		char buf[128];
		sprintf (buf, "footer_button_%02d_%02d", currScreen + 1, (i32)input);
		AetLayerArgs footerArgs ("AET_NSWGAM_DECORATION_MAIN", buf, 20, AetAction::NONE);
		footerArgs.play (&footerButtonId);

		if (currScreen == 1) {
			AetLayerArgs sideArrowsArgs ("AET_NSWGAM_DECORATION_MAIN", "slide_side_arrows", 19, AetAction::LOOP);
			sideArrowsArgs.frameRateControl = (void *)(This + 0x9108);
			sideArrowsArgs.play (&sideArrowsId);
		} else if (sideArrowsId != 0) {
			StopAet (&sideArrowsId);
			sideArrowsId = 0;
		}
	}

	auto args       = (AetLayerArgs *)(This + 0x8AD8);
	auto btnNoArgs  = (AetLayerArgs *)(This + 0x8CD8);
	auto btnYesArgs = (AetLayerArgs *)(This + 0x8ED0);

	AetComposition comp;
	GetComposition (&comp, args->id);
	if (btnNoArgs->id != 0) {
		if (auto layer = aets->find (btnNoArgs->id)) {
			if (auto layout = comp.find (string ("p_popup_delete_no"))) layer.value ()->color.w = layout.value ()->opacity;
		}
	}
	if (btnYesArgs->id != 0) {
		if (auto layer = aets->find (btnYesArgs->id)) {
			if (auto layout = comp.find (string ("p_popup_delete_yes"))) layer.value ()->color.w = layout.value ()->opacity;
		}
	}

	return false;
}

bool
DecorationDestroy (u64 This) {
	decorationInited = false;
	StopAet (&bgId);
	StopAet (&footerButtonId);
	StopAet (&anmId);
	return false;
}

const char *slideAnmLayers[] = {"slide_anm_in", "slide_anm_loop", "slide_anm_l", "slide_anm_r", "slide_anm_out"};

HOOK (void, UpdateSlideAnm, 0x1401F7170, void *a1, u8 newAnm, void *a3) {
	originalUpdateSlideAnm (a1, newAnm, a3);
	auto layerName = slideAnmLayers[newAnm];
	AetAction action;
	if (newAnm == 2) action = AetAction::LOOP;
	else if (newAnm == 4) action = AetAction::OUT_ONCE;
	else action = AetAction::IN_ONCE;
	AetLayerArgs args ("AET_NSWGAM_DECORATION_MAIN", layerName, 18, action);
	args.play (&anmId);
}

void
init () {
	taskAddition decoAddition;
	decoAddition.loop    = DecorationLoop;
	decoAddition.display = DecorationDisplay;
	decoAddition.destroy = DecorationDestroy;
	addTaskAddition ("DECO", decoAddition);
	INSTALL_HOOK (UpdateSlideAnm);

	WRITE_MEMORY (0x1401FA42A, i8, 21);
	WRITE_MEMORY (0x1401FA97D, i32, 22);
}

} // namespace decoration
