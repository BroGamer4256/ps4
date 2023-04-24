#include "diva.h"

namespace exitMenu {
using namespace diva;

bool wantsToExit   = false;
bool hasClicked    = false;
i32 menuAetId      = 0;
i32 yesButtonAetId = 0;
i32 noButtonAetId  = 0;
i32 hoveredButton  = 0;
char *yesButtonName;
char *noButtonName;
Vec3 yesButtonLoc;
Vec3 noButtonLoc;
Vec4 yesButtonRect;
Vec4 noButtonRect;

void
moveDown () {
	AetLayerArgs yesButtonAetData ("AET_PS4_MENU_MAIN", yesButtonName, 0x13, AetAction::IN_ONCE);
	AetLayerArgs noButtonAetData ("AET_PS4_MENU_MAIN", noButtonName, 0x13, AetAction::LOOP);

	yesButtonAetData.setPosition (yesButtonLoc);
	noButtonAetData.setPosition (noButtonLoc);

	yesButtonAetData.play (&yesButtonAetId);
	noButtonAetData.play (&noButtonAetId);

	hoveredButton = 0;
	PlaySoundEffect ("se_ft_sys_select_01", 1.0);
}

void
moveUp () {
	AetLayerArgs yesButtonAetData ("AET_PS4_MENU_MAIN", yesButtonName, 0x13, AetAction::LOOP);
	AetLayerArgs noButtonAetData ("AET_PS4_MENU_MAIN", noButtonName, 0x13, AetAction::IN_ONCE);

	yesButtonAetData.setPosition (yesButtonLoc);
	noButtonAetData.setPosition (noButtonLoc);

	yesButtonAetData.play (&yesButtonAetId);
	noButtonAetData.play (&noButtonAetId);

	hoveredButton = 1;
	PlaySoundEffect ("se_ft_sys_select_01", 1.0);
}

void
leaveMenu () {
	AetLayerArgs menuAetData ("AET_PS4_MENU_MAIN", "dialog_01", 0x12, AetAction::OUT_ONCE);
	menuAetData.play (&menuAetId);

	PlaySoundEffect ("se_ft_sys_cansel_01", 1.0);
	wantsToExit   = false;
	hoveredButton = 0;
}

void
initMenu () {
	AetLayerArgs menuAetData ("AET_PS4_MENU_MAIN", "dialog_01", 0x12, AetAction::IN_LOOP);
	menuAetData.play (&menuAetId);

	AetComposition compositionData;
	GetComposition (&compositionData, menuAetId);

	f32 yesButtonOpacity = 1.0;
	f32 noButtonOpacity  = 1.0;

	if (auto opt = compositionData.find (string ("p_submenu_03_c"))) {
		auto layer       = *opt;
		yesButtonLoc     = layer->position;
		yesButtonRect    = getPlaceholderRect (*layer);
		yesButtonOpacity = layer->opacity;
	}
	if (auto opt = compositionData.find (string ("p_submenu_04_c"))) {
		auto layer      = *opt;
		noButtonLoc     = layer->position;
		noButtonRect    = getPlaceholderRect (*layer);
		noButtonOpacity = layer->opacity;
	}

	AetLayerArgs yesButtonAetData ("AET_PS4_MENU_MAIN", yesButtonName, 0x13, AetAction::IN_ONCE);
	AetLayerArgs noButtonAetData ("AET_PS4_MENU_MAIN", noButtonName, 0x13, AetAction::LOOP);

	yesButtonAetData.setPosition (yesButtonLoc);
	noButtonAetData.setPosition (noButtonLoc);

	yesButtonAetData.color.w = yesButtonOpacity;
	noButtonAetData.color.w  = noButtonOpacity;

	yesButtonAetData.play (&yesButtonAetId);
	noButtonAetData.play (&noButtonAetId);

	wantsToExit = true;
	PlaySoundEffect ("se_ft_sys_enter_01", 1.0);
}

void
handleClick (Vec2 clickedPos) {
	if (yesButtonRect.contains (clickedPos)) {
		if (hoveredButton == 0) moveUp ();
		else *(u8 *)0x1414ABB90 = 1;
	} else if (noButtonRect.contains (clickedPos)) {
		if (hoveredButton == 1) moveDown ();
		else leaveMenu ();
	}
}

// Custom exit menu
HOOK (bool, CsMenuLoop, 0x1401B29D0, u64 This) {
	if (*(i32 *)(This + 0x68) != 2) return originalCsMenuLoop (This);

	void *inputState = diva::GetInputState (0);
	Vec2 clickedPos  = getClickedPos (inputState);

	AetComposition compositionData;
	GetComposition (&compositionData, menuAetId);

	if (auto layer = compositionData.find (string ("p_submenu_03_c")))
		if (auto aet = aets->find (yesButtonAetId)) aet.value ()->color.w = layer.value ()->opacity;

	if (auto layer = compositionData.find (string ("p_submenu_04_c")))
		if (auto aet = aets->find (noButtonAetId)) aet.value ()->color.w = layer.value ()->opacity;

	if (wantsToExit) {
		if (clickedPos.x > 0 && !hasClicked) {
			handleClick (clickedPos);
			hasClicked = true;
		} else if (clickedPos.x == 0) {
			hasClicked = false;
		}

		if (IsButtonTapped (inputState, Button::BACK)) leaveMenu ();
		else if (IsButtonTapped (inputState, Button::ACCEPT) && hoveredButton == 0) leaveMenu ();
		else if (IsButtonTapped (inputState, Button::ACCEPT) && hoveredButton == 1) *(u8 *)0x1414ABB90 = 1;
		else if (IsButtonTapped (inputState, Button::UP) && hoveredButton == 0) moveUp ();
		else if (IsButtonTapped (inputState, Button::DOWN) && hoveredButton == 1) moveDown ();
		return false;
	}

	if (IsButtonTapped (inputState, Button::BACK)) initMenu ();
	return originalCsMenuLoop (This);
}

void
init () {
	yesButtonName = appendTheme ("cmn_menu_yes");
	noButtonName  = appendTheme ("cmn_menu_no");
	INSTALL_HOOK (CsMenuLoop);
}
} // namespace exitMenu
