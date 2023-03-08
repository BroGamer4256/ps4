#include "diva.h"

namespace exitMenu {
bool wantsToExit   = false;
bool hasClicked    = false;
i32 menuAetId      = 0;
i32 yesButtonAetId = 0;
i32 noButtonAetId  = 0;
i32 hoveredButton  = 0;
char *yesButtonName;
char *noButtonName;
diva::map<diva::string, void *> compositionData;
Vec3 yesButtonLoc;
Vec3 noButtonLoc;
Vec4 yesButtonRect;
Vec4 noButtonRect;

using diva::AetAction;
using diva::aetLayer;
using diva::Button;

void
moveDown () {
	aetLayer yesButtonAetData (0x4F8, yesButtonName, 0x13, AetAction::IN_ONCE);
	aetLayer noButtonAetData (0x4F8, noButtonName, 0x13, AetAction::LOOP);

	yesButtonAetData.setPosition (yesButtonLoc);
	noButtonAetData.setPosition (noButtonLoc);

	yesButtonAetData.play (&yesButtonAetId);
	noButtonAetData.play (&noButtonAetId);

	hoveredButton = 0;
	PlaySoundEffect ("se_ft_sys_select_01", 1.0);
}

void
moveUp () {
	aetLayer yesButtonAetData (0x4F8, yesButtonName, 0x13, AetAction::LOOP);
	aetLayer noButtonAetData (0x4F8, noButtonName, 0x13, AetAction::IN_ONCE);

	yesButtonAetData.setPosition (yesButtonLoc);
	noButtonAetData.setPosition (noButtonLoc);

	yesButtonAetData.play (&yesButtonAetId);
	noButtonAetData.play (&noButtonAetId);

	hoveredButton = 1;
	PlaySoundEffect ("se_ft_sys_select_01", 1.0);
}

void
leaveMenu () {
	aetLayer menuAetData (0x4F8, "dialog_01", 0x12, AetAction::OUT_ONCE);
	menuAetData.play (&menuAetId);

	StopAet (&yesButtonAetId);
	StopAet (&noButtonAetId);

	PlaySoundEffect ("se_ft_sys_cansel_01", 1.0);
	wantsToExit   = false;
	hoveredButton = 0;
}

void
initMenu () {
	aetLayer menuAetData (0x4F8, "dialog_01", 0x12, AetAction::IN_LOOP);
	menuAetData.play (&menuAetId);

	initCompositionData (&compositionData);
	GetComposition (&compositionData, menuAetId);

	float *yesButtonPlaceholderData = GetCompositionLayer (&compositionData, "p_submenu_03_c");
	if (yesButtonPlaceholderData) {
		yesButtonLoc  = Vec3 (yesButtonPlaceholderData[16], yesButtonPlaceholderData[17], yesButtonPlaceholderData[18]);
		yesButtonRect = getPlaceholderRect (yesButtonPlaceholderData, false);
	}
	float *noButtonPlaceholderData = GetCompositionLayer (&compositionData, "p_submenu_04_c");
	if (noButtonPlaceholderData) {
		noButtonLoc  = Vec3 (noButtonPlaceholderData[16], noButtonPlaceholderData[17], noButtonPlaceholderData[18]);
		noButtonRect = getPlaceholderRect (noButtonPlaceholderData, false);
	}

	aetLayer yesButtonAetData (0x4F8, yesButtonName, 0x13, AetAction::IN_ONCE);
	aetLayer noButtonAetData (0x4F8, noButtonName, 0x13, AetAction::LOOP);

	yesButtonAetData.setPosition (yesButtonLoc);
	noButtonAetData.setPosition (noButtonLoc);

	yesButtonAetData.play (&yesButtonAetId);
	noButtonAetData.play (&noButtonAetId);

	wantsToExit = true;
	PlaySoundEffect ("se_ft_sys_enter_01", 1.0);
}

void
handleClick (Vec2 clickedPos) {
	if (yesButtonRect.contains (clickedPos)) {
		if (hoveredButton == 0) moveUp ();
		else ExitProcess (0);
	} else if (noButtonRect.contains (clickedPos)) {
		if (hoveredButton == 1) moveDown ();
		else leaveMenu ();
	}
}

// Custom exit menu
HOOK (bool, __thiscall, CsMenuLoop, 0x1401B29D0, u64 This) {
	if (*(i32 *)(This + 0x68) != 2) return originalCsMenuLoop (This);

	void *inputState = DivaGetInputState (0);
	Vec2 clickedPos  = getClickedPos (inputState);

	if (wantsToExit) {
		if (clickedPos.x > 0 && !hasClicked) {
			handleClick (clickedPos);
			hasClicked = true;
		} else if (clickedPos.x == 0) {
			hasClicked = false;
		}

		if (IsButtonTapped (inputState, Button::BACK)) leaveMenu ();
		else if (IsButtonTapped (inputState, Button::ACCEPT) && hoveredButton == 0) leaveMenu ();
		else if (IsButtonTapped (inputState, Button::ACCEPT) && hoveredButton == 1) ExitProcess (0);
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
