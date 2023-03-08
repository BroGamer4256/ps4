#include "diva.h"
#include "menus.h"

namespace options {
i32 menuTxt1Id = 0;
i32 menuTxt2Id = 0;
i32 menuTxt3Id = 0;
i32 menuTxt4Id = 0;
i32 menuTxt5Id = 0;
i32 menuTxt6Id = 0;
i32 footerId;
char *footerName = (char *)calloc (32, sizeof (char));
char *topLoopName;

using diva::AetAction;
using diva::aetLayer;
using diva::InputType;

void
playMenuTxt (u8 button, u8 subMenu, AetAction action) {
	const char *name;
	aetLayer menuTxtData;
	switch (button) {
	case 0:
		switch (subMenu) {
		case 0: name = "option_top_menu_txt_sound"; break;
		case 1: name = "option_menu_sound_txt_audio_device"; break;
		case 2: name = "option_menu_display_txt_target"; break;
		case 3: name = "option_menu_graphic_txt_graphic"; break;
		default: name = ""; break;
		}
		menuTxtData.with_data (0x525, name, 13, action);
		menuTxtData.play (&menuTxt1Id);
		break;
	case 1:
		switch (subMenu) {
		case 0: name = "option_top_menu_txt_display"; break;
		case 1: name = "option_menu_sound_txt_theme"; break;
		case 2: name = "option_menu_display_txt_mode"; break;
		case 3: name = "option_menu_graphic_txt_image"; break;
		default: name = ""; break;
		}
		menuTxtData.with_data (0x525, name, 13, action);
		menuTxtData.play (&menuTxt2Id);
		break;
	case 2:
		switch (subMenu) {
		case 0: name = "option_top_menu_txt_graphic"; break;
		case 1: name = "option_menu_sound_txt_bgm"; break;
		case 2: name = "option_menu_display_txt_resolution"; break;
		case 3: name = "option_menu_graphic_txt_aa"; break;
		default: name = ""; break;
		}
		menuTxtData.with_data (0x525, name, 13, action);
		menuTxtData.play (&menuTxt3Id);
		break;
	case 3:
		switch (subMenu) {
		case 0: name = "option_top_menu_txt_npr"; break;
		case 1: name = "option_menu_sound_txt_btn"; break;
		case 3: name = "option_menu_graphic_txt_shadow"; break;
		default: name = ""; break;
		}
		menuTxtData.with_data (0x525, name, 13, action);
		menuTxtData.play (&menuTxt4Id);
		break;
	case 4:
		switch (subMenu) {
		case 0: name = "option_top_menu_txt_autosave"; break;
		case 1: name = "option_menu_sound_txt_se"; break;
		case 3: name = "option_menu_graphic_txt_reflection"; break;
		default: name = ""; break;
		}
		menuTxtData.with_data (0x525, name, 13, action);
		menuTxtData.play (&menuTxt5Id);
		break;
	case 5:
		switch (subMenu) {
		case 0: name = "option_top_menu_txt_save"; break;
		default: name = ""; break;
		}
		menuTxtData.with_data (0x525, name, 13, action);
		menuTxtData.play (&menuTxt6Id);
		break;
	}
}

void *optionMenuTopData = calloc (1, 0x1024);
i32 optionMenuTopId     = 0;
InputType previousInput = InputType::UNKNOWN;
HOOK (bool, __stdcall, OptionMenuSwitchInit, 0x1406C3CB0, void *a1) {
	aetLayer optionMenuTopData (0x525, topLoopName, 7, AetAction::NONE);
	optionMenuTopData.play (&optionMenuTopId);

	InputType input = diva::getInputType ();
	previousInput   = input;
	sprintf (footerName, "footer_button_01_%02d", (i32)input);

	aetLayer footerData (0x525, footerName, 13, AetAction::NONE);
	footerData.play (&footerId);

	playMenuTxt (0, 0, AetAction::IN_LOOP);
	return originalOptionMenuSwitchInit (a1);
}

u8 previousButton  = 0;
u8 previousSubMenu = 0;
HOOK (bool, __stdcall, OptionMenuSwitchLoop, 0x1406C2920, u64 a1) {
	u8 subMenu      = *(u8 *)(a1 + 0xB3);
	subMenu         = subMenu > 1 ? subMenu - 1 : subMenu;
	u8 button       = *(u8 *)(a1 + 0xB4 + subMenu);
	InputType input = diva::getInputType ();
	if (input != previousInput || previousSubMenu != subMenu) {
		sprintf (footerName, "footer_button_%02d_%02d", (bool)subMenu + 1, (i32)input);
		aetLayer footerData (0x525, footerName, 13, AetAction::NONE);
		footerData.play (&footerId);
		previousInput = input;
	}
	if (button != previousButton || previousSubMenu != subMenu) {
		playMenuTxt (previousButton, previousSubMenu, AetAction::OUT_ONCE);
		playMenuTxt (button, subMenu, AetAction::IN_LOOP);
		previousButton  = button;
		previousSubMenu = subMenu;
	}
	return originalOptionMenuSwitchLoop (a1);
}

HOOK (bool, __stdcall, OptionMenuSwitchDestroy, 0x1406C2CE0, void *a1) {
	diva::StopAet (&optionMenuTopId);
	diva::StopAet (&menuTxt1Id);
	diva::StopAet (&menuTxt2Id);
	diva::StopAet (&menuTxt3Id);
	diva::StopAet (&menuTxt4Id);
	diva::StopAet (&menuTxt5Id);
	diva::StopAet (&menuTxt6Id);
	diva::StopAet (&footerId);
	return originalOptionMenuSwitchDestroy (a1);
}

void
init () {
	topLoopName = diva::appendTheme ("option_top_menu loop");

	INSTALL_HOOK (OptionMenuSwitchInit);
	INSTALL_HOOK (OptionMenuSwitchLoop);
	INSTALL_HOOK (OptionMenuSwitchDestroy);
}
} // namespace options
