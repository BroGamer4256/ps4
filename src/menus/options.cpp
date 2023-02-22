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
char *topLoopName;

void
playMenuTxt (u8 button, u8 subMenu, AetAction action) {
	const char *name;
	switch (button) {
	case 0:
		switch (subMenu) {
		case 0: name = "option_top_menu_txt_sound"; break;
		case 1: name = "option_menu_sound_txt_audio_device"; break;
		case 3: name = "option_menu_display_txt_target"; break;
		case 4: name = "option_menu_graphic_txt_graphic"; break;
		default: name = ""; break;
		}
		CreateAetLayerData (menuTxt1Data, 0x525, name, 13, action);
		menuTxt1Id = PlayAetLayer (menuTxt1Data, menuTxt1Id);
		break;
	case 1:
		switch (subMenu) {
		case 0: name = "option_top_menu_txt_display"; break;
		case 1: name = "option_menu_sound_txt_theme"; break;
		case 3: name = "option_menu_display_txt_mode"; break;
		case 4: name = "option_menu_graphic_txt_image"; break;
		default: name = ""; break;
		}
		CreateAetLayerData (menuTxt2Data, 0x525, name, 13, action);
		menuTxt2Id = PlayAetLayer (menuTxt2Data, menuTxt2Id);
		break;
	case 2:
		switch (subMenu) {
		case 0: name = "option_top_menu_txt_graphic"; break;
		case 1: name = "option_menu_sound_txt_bgm"; break;
		case 3: name = "option_menu_display_txt_resolution"; break;
		case 4: name = "option_menu_graphic_txt_aa"; break;
		default: name = ""; break;
		}
		CreateAetLayerData (menuTxt3Data, 0x525, name, 13, action);
		menuTxt3Id = PlayAetLayer (menuTxt3Data, menuTxt3Id);
		break;
	case 3:
		switch (subMenu) {
		case 0: name = "option_top_menu_txt_npr"; break;
		case 1: name = "option_menu_sound_txt_btn"; break;
		case 4: name = "option_menu_graphic_txt_shadow"; break;
		default: name = ""; break;
		}
		CreateAetLayerData (menuTxt4Data, 0x525, name, 13, action);
		menuTxt4Id = PlayAetLayer (menuTxt4Data, menuTxt4Id);
		break;
	case 4:
		switch (subMenu) {
		case 0: name = "option_top_menu_txt_autosave"; break;
		case 1: name = "option_menu_sound_txt_se"; break;
		case 4: name = "option_menu_graphic_txt_reflection"; break;
		default: name = ""; break;
		}
		CreateAetLayerData (menuTxt5Data, 0x525, name, 13, action);
		menuTxt5Id = PlayAetLayer (menuTxt5Data, menuTxt5Id);
		break;
	case 5:
		switch (subMenu) {
		case 0: name = "option_top_menu_txt_save"; break;
		default: name = ""; break;
		}
		CreateAetLayerData (menuTxt6Data, 0x525, name, 13, action);
		menuTxt6Id = PlayAetLayer (menuTxt6Data, menuTxt6Id);
		break;
	}
}

void *optionMenuTopData = calloc (1, 0x1024);
i32 optionMenuTopId     = 0;
HOOK (bool, __stdcall, OptionMenuSwitchInit, 0x1406C3CB0, void *a1) {
	CreateAetLayerData (optionMenuTopData, 0x525, topLoopName, 7, AETACTION_NONE);
	optionMenuTopId = PlayAetLayer (optionMenuTopData, optionMenuTopId);
	playMenuTxt (0, 0, AETACTION_IN_LOOP);
	return originalOptionMenuSwitchInit (a1);
}

u8 previousButton  = 0;
u8 previousSubMenu = 0;
HOOK (bool, __stdcall, OptionMenuSwitchLoop, 0x1406C2920, u64 a1) {
	u8 subMenu = *(u8 *)(a1 + 0xB3);
	u8 button  = *(u8 *)(a1 + 0xB4 + (subMenu > 1 ? subMenu - 1 : subMenu));
	if (button != previousButton || previousSubMenu != subMenu) {
		playMenuTxt (previousButton, previousSubMenu, AETACTION_OUT);
		playMenuTxt (button, subMenu, AETACTION_IN_LOOP);
		previousButton  = button;
		previousSubMenu = subMenu;
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
	topLoopName = appendTheme ("option_top_menu loop");

	INSTALL_HOOK (OptionMenuSwitchInit);
	INSTALL_HOOK (OptionMenuSwitchLoop);
	INSTALL_HOOK (OptionMenuSwitchDestroy);
}
} // namespace options
