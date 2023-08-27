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
i32 gameOptionsArrowsUpId   = 0;
i32 gameOptionsArrowsDownId = 0;

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
		diva::AetLayerArgs layer ("AET_NSWGAM_CUSTOM_MAIN", buf, 21, AetAction::NONE);
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
	diva::AetLayerArgs layer ("AET_NSWGAM_CUSTOM_MAIN", buf, 21, AetAction::NONE);
	layer.play (&footerButtonId);
	currentMenu       = args->screen;
	previousInputType = diva::getInputType ();
	originalPlayCustomizeSelFooter (a1, args);
}

HOOK (void, StopCustomizeSelFooter, 0x140684A00, void *a1) {
	StopAet (&footerButtonId);
	originalStopCustomizeSelFooter (a1);
}

HOOK (void, PlayTshirtEditFooter, 0x140710680, void *a1, i32 index) {
	i32 realIndex = 0;
	if (index == 0) realIndex = 3;
	else if (index == 1) realIndex = 4;
	else if (index == 2) realIndex = 7;

	char buf[128];
	sprintf (buf, "footer_button_%02d_%02d", realIndex + 1, (i32)diva::getInputType ());
	diva::AetLayerArgs layer ("AET_NSWGAM_CUSTOM_MAIN", buf, 0x11, AetAction::NONE);
	layer.play (&footerButtonId);
	currentMenu       = realIndex;
	previousInputType = diva::getInputType ();

	originalPlayTshirtEditFooter (a1, index);
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
		StopAet (&gameOptionsArrowsUpId);
		StopAet (&gameOptionsArrowsDownId);
		previousOption = -1;
	} else if (a2 == 1) {
		diva::AetLayerArgs args ("AET_NSWGAM_CUSTOM_MAIN", "setting_menu_bg_arrow_up", 0x10, AetAction::IN_LOOP);
		args.play (&gameOptionsArrowsUpId);
		diva::AetLayerArgs bottomArgs ("AET_NSWGAM_CUSTOM_MAIN", "setting_menu_bg_arrow_down", 0x10, AetAction::IN_LOOP);
		bottomArgs.play (&gameOptionsArrowsDownId);
	} else {
		i32 selectedOption = *(i32 *)(a1 + 0x60);
		if (selectedOption != previousOption) {
			if (previousOption != -1) playOptionText (previousOption, AetAction::OUT_ONCE);
			playOptionText (selectedOption, AetAction::IN_LOOP);
			previousOption = selectedOption;

			if (previousOption != -1) {
				if (selectedOption == 0) {
					if (auto layer = aets->find (gameOptionsArrowsUpId)) {
						diva::AetLayerArgs topArgs ("AET_NSWGAM_CUSTOM_MAIN", "setting_menu_bg_arrow_up", 0x10, AetAction::IN_ONCE);
						topArgs.play (&gameOptionsArrowsUpId);
						diva::AetLayerArgs bottomArgs ("AET_NSWGAM_CUSTOM_MAIN", "setting_menu_bg_arrow_down", 0x10, AetAction::SPECIAL_LOOP);
						bottomArgs.play (&gameOptionsArrowsDownId);
					}
				} else if (selectedOption == 8) {
					if (auto layer = aets->find (gameOptionsArrowsDownId)) {
						diva::AetLayerArgs topArgs ("AET_NSWGAM_CUSTOM_MAIN", "setting_menu_bg_arrow_up", 0x10, AetAction::SPECIAL_LOOP);
						topArgs.play (&gameOptionsArrowsUpId);
						diva::AetLayerArgs bottomArgs ("AET_NSWGAM_CUSTOM_MAIN", "setting_menu_bg_arrow_down", 0x10, AetAction::IN_ONCE);
						bottomArgs.play (&gameOptionsArrowsDownId);
					}
				} else {
					diva::AetLayerArgs topArgs ("AET_NSWGAM_CUSTOM_MAIN", "setting_menu_bg_arrow_up", 0x10, AetAction::SPECIAL_LOOP);
					topArgs.play (&gameOptionsArrowsUpId);
					diva::AetLayerArgs bottomArgs ("AET_NSWGAM_CUSTOM_MAIN", "setting_menu_bg_arrow_down", 0x10, AetAction::SPECIAL_LOOP);
					bottomArgs.play (&gameOptionsArrowsDownId);
				}
			}
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

HOOK (u32 *, SetCursorColor, 0x14065E410, void *a1, u32 *rgbaColor) {
	originalSetCursorColor (a1, rgbaColor);
	*rgbaColor |= 0xFF;
	return rgbaColor;
}

i32 choiceListPackId[16] = {0};

extern "C" {
void
LoadChoiceListPlayTxt (AetLayoutData *layout, ModuleAttr attr, i32 index) {
	if ((attr & (ModuleAttr::FutureSound | ModuleAttr::ColorfulTone)) == (ModuleAttr::FutureSound | ModuleAttr::ColorfulTone)) {
		StopAet (&choiceListPackId[index]);
	} else if (attr & (ModuleAttr::FutureSound | ModuleAttr::ColorfulTone)) {
		AetLayerArgs args;
		auto priority = index;
		if (index > 5) {
			priority = -priority;
			priority += 10;
		}
		priority *= 2;
		priority += 10;

		if (attr & ModuleAttr::FutureSound) args.create ("AET_NSWGAM_CUSTOM_MAIN", "choice_list_pack_f", priority, AetAction::NONE);
		else args.create ("AET_NSWGAM_CUSTOM_MAIN", "choice_list_pack_t", priority, AetAction::NONE);
		args.position = layout->position;
		args.color.w  = layout->opacity;
		if (index == 5) {
			args.scale = Vec3{1.2, 1.2, 1.2};
			args.position.y -= 5;
		} else if (index == 11) {
			args.color.w = 0.0;
		}
		args.play (&choiceListPackId[index]);
	} else {
		StopAet (&choiceListPackId[index]);
	}
}

HOOK (void, LoadModuleChoiceList, 0x140691D47);
const char *
realLoadModuleChoiceList (u64 This, i32 moduleId, i32 index) {
	auto modules   = (vector<ModuleData *> *)(This + 0x70);
	auto moduleOpt = modules->at (moduleId);
	if (!moduleOpt.has_value ()) return "choice_list_mdl_base_etc_sel";
	auto module = **moduleOpt;
	if (module == 0) return "choice_list_mdl_base_etc_sel";

	auto layouts = *(u64 *)(This + 0x1C8);
	if (layouts == 0) return "choice_list_mdl_base_etc_sel";
	auto offset = *(i32 *)(This + 0x1AC);
	auto layout = *(AetLayoutData **)(layouts + ((index + offset) * 16));
	if (layout == 0) return "choice_list_mdl_base_etc_sel";

	LoadChoiceListPlayTxt (layout, module->attr, index);

	if ((module->attr & (ModuleAttr::FutureSound | ModuleAttr::ColorfulTone)) == (ModuleAttr::FutureSound | ModuleAttr::ColorfulTone)) return "choice_list_mdl_base_etc_sel";
	else if (module->attr & ModuleAttr::FutureSound) return "choice_list_mdl_base_f_sel";
	else if (module->attr & ModuleAttr::ColorfulTone) return "choice_list_mdl_base_t_sel";
	else return "choice_list_mdl_base_etc_sel";
}

HOOK (void, LoadHairstyleChoiceList, 0x1406892F8);
const char *
realLoadHairstyleChoiceList (u64 This, i32 hairstyleId, i32 index) {
	auto taskData = *(u64 *)0x14CC6F178;
	auto modules  = (vector<ModuleData> *)(taskData + 0x1A0);

	auto hairstyles   = (vector<CustomizeItemData *> *)(This + 0x108);
	auto hairstyleOpt = hairstyles->at (hairstyleId);
	if (!hairstyleOpt.has_value ()) {
		printf ("Hairstyle with offset %d lookup failed\n", hairstyleId);
		return "choice_list_mdl_base_etc_sel";
	}
	auto hairstyle = **hairstyleOpt;
	if (hairstyle == 0) {
		printf ("Hairstyle with offset %d is NULL\n", hairstyleId);
		return "choice_list_mdl_base_etc_sel";
	}
	if (hairstyle->bind_module == -1) return "choice_list_mdl_base_etc_sel";
	ModuleData *module = 0;
	for (auto it = modules->begin (); it != modules->end (); it++) {
		if (it == 0) continue;
		if (it->id == hairstyle->bind_module) {
			module = it;
			break;
		}
	}
	if (module == 0) {
		printf ("Failed to find module %d\n", hairstyle->bind_module);
		return "choice_list_mdl_base_etc_sel";
	}

	auto layouts = *(u64 *)(This + 0x1F0);
	if (layouts == 0) return "choice_list_mdl_base_etc_sel";
	auto offset = *(i32 *)(This + 0x1D4);
	auto layout = *(AetLayoutData **)(layouts + ((index + offset) * 16));
	if (layout == 0) return "choice_list_mdl_base_etc_sel";

	LoadChoiceListPlayTxt (layout, module->attr, index);

	if ((module->attr & (ModuleAttr::FutureSound | ModuleAttr::ColorfulTone)) == (ModuleAttr::FutureSound | ModuleAttr::ColorfulTone)) return "choice_list_mdl_base_etc_sel";
	else if (module->attr & ModuleAttr::FutureSound) return "choice_list_mdl_base_f_sel";
	else if (module->attr & ModuleAttr::ColorfulTone) return "choice_list_mdl_base_t_sel";
	else return "choice_list_mdl_base_etc_sel";
}

HOOK (void, SetModuleChoiceListPriority, 0x140691DB7);
HOOK (void, SetModuleSprPriority, 0x140692C4A);
HOOK (void, Memset, 0x14097B0E0);
}

HOOK (void, DestroyModuleSelect, 0x1406910D0, u64 This) {
	for (size_t i = 0; i < COUNTOFARR (choiceListPackId); i++)
		StopAet (&choiceListPackId[i]);
	originalDestroyModuleSelect (This);
}

HOOK (void, DestroyHairstyleSelect, 0x140688550, u64 This) {
	for (size_t i = 0; i < COUNTOFARR (choiceListPackId); i++)
		StopAet (&choiceListPackId[i]);
	originalDestroyHairstyleSelect (This);
}

HOOK (void, PlayCharaNum, 0x140686690, u64 a1, i32 a2, bool a3) {
	if (a2 == 3) {
		for (size_t i = 0; i < COUNTOFARR (choiceListPackId); i++) {
			auto layer = aets->find (choiceListPackId[i]);
			if (!layer.has_value ()) continue;
			layer.value ()->color.w = 0.0;
		}
	} else {
		for (size_t i = 0; i < COUNTOFARR (choiceListPackId); i++) {
			auto layer = aets->find (choiceListPackId[i]);
			if (!layer.has_value ()) continue;
			layer.value ()->color.w = 1.0;
		}
	}
	originalPlayCharaNum (a1, a2, a3);
}

void
init () {
	INSTALL_HOOK (CustomizeSelInit);
	INSTALL_HOOK (CustomizeSelIsLoaded);
	INSTALL_HOOK (PlayCustomizeSelFooter);
	INSTALL_HOOK (StopCustomizeSelFooter);
	INSTALL_HOOK (PlayTshirtEditFooter);
	INSTALL_HOOK (GameOptionsLoop);
	INSTALL_HOOK (ButtonFxListIn);
	INSTALL_HOOK (ButtonFxUnload);
	INSTALL_HOOK (SetCursorColor);

	INSTALL_HOOK (LoadModuleChoiceList);
	INSTALL_HOOK (LoadHairstyleChoiceList);
	INSTALL_HOOK (DestroyModuleSelect);
	INSTALL_HOOK (DestroyHairstyleSelect);

	INSTALL_HOOK (SetModuleChoiceListPriority);
	INSTALL_HOOK (SetModuleSprPriority);
	INSTALL_HOOK (Memset);

	INSTALL_HOOK (PlayCharaNum);

	taskAddition addition;
	addition.loop    = CustomizeSelLoop;
	addition.destroy = CustomizeSelDestroy;
	addTaskAddition ("CustomizeSel", addition);

	// Use the right font
	// Modules
	WRITE_MEMORY (0x140692A3A, i8, 0x00);
	WRITE_MEMORY (0x140692A3D, i8, 0x10);
	// Hairstyles
	WRITE_MEMORY (0x140689C45, i8, 0x00);
	WRITE_MEMORY (0x140689C48, i8, 0x10);
	// Accessories
	WRITE_MEMORY (0x14068DC1A, i8, 0x00);
	WRITE_MEMORY (0x14068DC1D, i8, 0x10);

	WRITE_NOP (0x140691DC4, 6);

	WRITE_MEMORY (0x1406920B4, i32, 22); // Module name box priority
	WRITE_MEMORY (0x140692D50, i32, 23); // Module name text priority
	WRITE_MEMORY (0x14069217B, i32, 22); // VP cost box priority
	WRITE_MEMORY (0x1406930EE, i32, 23); // VP cost text priority
	WRITE_MEMORY (0x14069222E, i32, 24); // Module selected priority
	WRITE_NOP (0x14069223D, 4);
	WRITE_MEMORY (0x140692398, i32, 21); // Reccomended module priority
	WRITE_NOP (0x1406923A3, 4);

	WRITE_MEMORY (0x140677FA9, i32, 25); // Choice_conf priority
	WRITE_MEMORY (0x140677E86, i32, 26); // Choice_conf button priority
}
} // namespace customize
