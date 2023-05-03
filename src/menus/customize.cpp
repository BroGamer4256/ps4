#include "diva.h"
#include "menus.h"

namespace customize {
using namespace diva;
i32 footerButtonId = 0;
i32 optionTxt0Id   = 0;
i32 optionTxt1Id   = 0;
i32 optionTxt2Id   = 0;
i32 optionTxt3Id   = 0;
i32 optionTxt4Id   = 0;
i32 optionTxt5Id   = 0;
i32 optionTxt6Id   = 0;
i32 optionTxt7Id   = 0;
i32 optionTxt8Id   = 0;
i32 soundListInId  = 0;
i32 currentMenu    = -1;
i32 previousOption = -1;
InputType previousInputType;

struct PlayCustomizeSelFooterArgs {
	diva::string footerName;
	i32 screen;
};

// Fixes the header/footer being present on customize
HOOK (bool, CustomizeSelInit, 0x140687D10, u64 This) {
	diva::CmnMenuDestroy (0x14114C370);
	pvSel::hide ();
	return originalCustomizeSelInit (This);
}

// Fixes switching to customize from playlists
HOOK (bool, CustomizeSelIsLoaded, 0x140687CD0) {
	if (*(i32 *)0x14CC6F118 == 1) {
		if (implOfCustomizeSelInit (0x14CC6F100)) {
			*(i32 *)0x14CC6F118 = 2;
			*(i32 *)0x14CC6F124 = 2;
		}
	}

	return originalCustomizeSelIsLoaded ();
}

bool
CustomizeSelLoop (u64 task) {
	InputType input = diva::getInputType ();
	if (currentMenu != -1 && input != previousInputType) {
		previousInputType = input;
		char buf[128];
		sprintf (buf, "footer_button_%02d_%02d", currentMenu + 1, (i32)input);
		diva::AetLayerArgs layer ("AET_NSWGAM_CUSTOM_MAIN", buf, 0x11, AetAction::NONE);
		layer.play (&footerButtonId);
	}
	return false;
}

bool
CustomizeSelDestroy (u64 task) {
	StopAet (&footerButtonId);
	currentMenu = -1;
	return false;
}

HOOK (void, PlayCustomizeSelFooter, 0x15F9811D0, void *a1, PlayCustomizeSelFooterArgs *args) {
	char buf[128];
	sprintf (buf, "footer_button_%02d_%02d", args->screen + 1, (i32)diva::getInputType ());
	diva::AetLayerArgs layer ("AET_NSWGAM_CUSTOM_MAIN", buf, 0x11, AetAction::NONE);
	layer.play (&footerButtonId);
	currentMenu       = args->screen;
	previousInputType = diva::getInputType ();
	originalPlayCustomizeSelFooter (a1, args);
}

void
playOptionText (i32 option, AetAction action) {
	const char *txtLayer = "";
	i32 *txtId;
	switch (option) {
	case 0:
		txtLayer = "game_menu_txt_button_config";
		txtId    = &optionTxt0Id;
		break;
	case 1:
		txtLayer = "game_menu_txt_multipress";
		txtId    = &optionTxt1Id;
		break;
	case 2:
		txtLayer = "game_menu_txt_ac_controller";
		txtId    = &optionTxt2Id;
		break;
	case 3:
		txtLayer = "game_menu_txt_vibration";
		txtId    = &optionTxt3Id;
		break;
	case 4:
		txtLayer = "game_menu_txt_vocal";
		txtId    = &optionTxt4Id;
		break;
	case 5:
		txtLayer = "game_menu_txt_icon";
		txtId    = &optionTxt5Id;
		break;
	case 6:
		txtLayer = "game_menu_txt_threshold";
		txtId    = &optionTxt6Id;
		break;
	case 7:
		txtLayer = "game_menu_txt_lag_config";
		txtId    = &optionTxt7Id;
		break;
	case 8:
		txtLayer = "game_menu_txt_shared_config";
		txtId    = &optionTxt8Id;
		break;
	}
	AetLayerArgs txtArgs ("AET_NSWGAM_CUSTOM_MAIN", txtLayer, 0xD, action);
	txtArgs.play (txtId);
}

HOOK (void *, GameOptionsLoop, 0x14066E0E0, u64 a1, i32 a2, bool a3) {
	if (a2 == 0) {
		StopAet (&optionTxt0Id);
		StopAet (&optionTxt1Id);
		StopAet (&optionTxt2Id);
		StopAet (&optionTxt3Id);
		StopAet (&optionTxt4Id);
		StopAet (&optionTxt5Id);
		StopAet (&optionTxt6Id);
		StopAet (&optionTxt7Id);
		StopAet (&optionTxt8Id);
		previousOption = -1;
	} else {
		i32 selectedOption = *(i32 *)(a1 + 0x60);
		if (selectedOption != previousOption) {
			if (previousOption != -1) playOptionText (previousOption, AetAction::OUT_ONCE);
			playOptionText (selectedOption, AetAction::IN_LOOP);
			previousOption = selectedOption;
		}
	}
	return originalGameOptionsLoop (a1, a2, a3);
}

HOOK (void, ButtonFxListIn, 0x1406985B0, u64 a1) {
	AetLayerArgs soundListInArgs ("AET_NSWGAM_CUSTOM_MAIN", "sound_list_in", 0xD, AetAction::NONE);
	soundListInArgs.play (&soundListInId);
	originalButtonFxListIn (a1);
}

HOOK (void, ButtonFxUnload, 0x1406996C0, u64 a1) {
	StopAet (&soundListInId);
	originalButtonFxUnload (a1);
}

void
init () {
	INSTALL_HOOK (CustomizeSelInit);
	INSTALL_HOOK (CustomizeSelIsLoaded);
	INSTALL_HOOK (PlayCustomizeSelFooter);
	INSTALL_HOOK (GameOptionsLoop);
	INSTALL_HOOK (ButtonFxListIn);
	INSTALL_HOOK (ButtonFxUnload);

	taskAddition addition;
	addition.loop    = CustomizeSelLoop;
	addition.destroy = CustomizeSelDestroy;
	addTaskAddition ("CustomizeSel", addition);
}
} // namespace customize
