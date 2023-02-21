#include "diva.h"
#include "helpers.h"
#include "menus.h"

namespace options {
void *menuTxt1Data = calloc (1, 0x1024);
void *menuTxt2Data = calloc (1, 0x1024);
void *menuTxt3Data = calloc (1, 0x1024);
void *menuTxt4Data = calloc (1, 0x1024);
void *menuTxt5Data = calloc (1, 0x1024);
void *menuTxt6Data = calloc (1, 0x1024);
i32 menuTxt1Id     = 0;
i32 menuTxt2Id     = 0;
i32 menuTxt3Id     = 0;
i32 menuTxt4Id     = 0;
i32 menuTxt5Id     = 0;
i32 menuTxt6Id     = 0;

void
playMenuTxt (i32 button, AetAction action) {
	switch (button) {
	case 0:
		CreateAetLayerData (menuTxt1Data, 0x525, "option_top_menu_txt_sound", 11, action);
		menuTxt1Id = PlayAetLayer (menuTxt1Data, menuTxt1Id);
		break;
	case 1:
		CreateAetLayerData (menuTxt2Data, 0x525, "option_top_menu_txt_display", 11, action);
		menuTxt2Id = PlayAetLayer (menuTxt2Data, menuTxt2Id);
		break;
	case 2:
		CreateAetLayerData (menuTxt3Data, 0x525, "option_top_menu_txt_graphic", 11, action);
		menuTxt3Id = PlayAetLayer (menuTxt3Data, menuTxt3Id);
		break;
	case 3:
		CreateAetLayerData (menuTxt4Data, 0x525, "option_top_menu_txt_npr", 11, action);
		menuTxt4Id = PlayAetLayer (menuTxt4Data, menuTxt4Id);
		break;
	case 4:
		CreateAetLayerData (menuTxt5Data, 0x525, "option_top_menu_txt_autosave", 11, action);
		menuTxt5Id = PlayAetLayer (menuTxt5Data, menuTxt5Id);
		break;

	case 5:
		CreateAetLayerData (menuTxt6Data, 0x525, "option_top_menu_txt_save", 11, action);
		menuTxt6Id = PlayAetLayer (menuTxt6Data, menuTxt6Id);
		break;
	}
}

void *optionMenuTopData = calloc (1, 0x1024);
i32 optionMenuTopId     = 0;
HOOK (bool, __stdcall, OptionMenuSwitchInit, 0x1406C3CB0, void *a1) {
	CreateAetLayerData (optionMenuTopData, 0x525, "option_top_menu loop", 7, AETACTION_NONE);
	optionMenuTopId = PlayAetLayer (optionMenuTopData, optionMenuTopId);
	playMenuTxt (0, AETACTION_IN_LOOP);
	return originalOptionMenuSwitchInit (a1);
}

u8 previousButton = 0;
bool wasInSubMenu = false;
HOOK (bool, __stdcall, OptionMenuSwitchLoop, 0x1406C2920, u64 a1) {
	u8 button = *(u8 *)(a1 + 0xB4);
	if (button != previousButton) {
		playMenuTxt (previousButton, AETACTION_OUT);
		playMenuTxt (button, AETACTION_IN_LOOP);
		previousButton = button;
	}
	bool inSubmenu = *(bool *)(a1 + 0xB3);
	if (inSubmenu && !wasInSubMenu) {
		playMenuTxt (button, AETACTION_OUT);
		wasInSubMenu = true;
	} else if (!inSubmenu && wasInSubMenu) {
		playMenuTxt (button, AETACTION_IN_LOOP);
		wasInSubMenu = false;
	}
	return originalOptionMenuSwitchLoop (a1);
}

HOOK (bool, __stdcall, OptionMenuSwitchDestroy, 0x1406C2CE0, void *a1) {
	StopAet (&optionMenuTopId);
	StopAet (&menuTxt1Id);
	StopAet (&menuTxt2Id);
	StopAet (&menuTxt3Id);
	StopAet (&menuTxt4Id);
	StopAet (&menuTxt5Id);
	StopAet (&menuTxt6Id);
	return originalOptionMenuSwitchDestroy (a1);
}

void
init () {
	INSTALL_HOOK (OptionMenuSwitchInit);
	INSTALL_HOOK (OptionMenuSwitchLoop);
	INSTALL_HOOK (OptionMenuSwitchDestroy);
}
} // namespace options
