#include "diva.h"
#include "helpers.h"

namespace exitMenu {
bool wantsToExit       = false;
bool hasClicked        = false;
void *menuAetData      = calloc (1, 0x1024);
void *yesButtonAetData = calloc (1, 0x1024);
void *noButtonAetData  = calloc (1, 0x1024);
void *testAetData      = calloc (1, 0x1024);
i32 menuAetId          = 0;
i32 yesButtonAetId     = 0;
i32 noButtonAetId      = 0;
i32 hoveredButton      = 0;
char *yesButtonName;
char *noButtonName;
Map<String, void *> compositionData;
Vec3 yesButtonLoc;
Vec3 noButtonLoc;
Vec4 yesButtonRect;
Vec4 noButtonRect;

void
moveDown () {
	hoveredButton = 0;
	CreateAetLayerData (yesButtonAetData, 0x4F8, yesButtonName, 0x13, AETACTION_IN);
	CreateAetLayerData (noButtonAetData, 0x4F8, noButtonName, 0x13, AETACTION_LOOP);

	ApplyLocation (yesButtonAetData, &yesButtonLoc);
	ApplyLocation (noButtonAetData, &noButtonLoc);

	PlayAetLayer (yesButtonAetData, yesButtonAetId);
	PlayAetLayer (noButtonAetData, noButtonAetId);
	PlaySoundEffect ("se_ft_sys_select_01", 1.0);
}

void
moveUp () {
	hoveredButton = 1;
	CreateAetLayerData (yesButtonAetData, 0x4F8, yesButtonName, 0x13, AETACTION_LOOP);
	CreateAetLayerData (noButtonAetData, 0x4F8, noButtonName, 0x13, AETACTION_IN);

	ApplyLocation (yesButtonAetData, &yesButtonLoc);
	ApplyLocation (noButtonAetData, &noButtonLoc);

	PlayAetLayer (yesButtonAetData, yesButtonAetId);
	PlayAetLayer (noButtonAetData, noButtonAetId);

	PlaySoundEffect ("se_ft_sys_select_01", 1.0);
}

void
leaveMenu () {
	CreateAetLayerData (menuAetData, 0x4F8, "dialog_01", 0x12, AETACTION_OUT);

	Vec3 offscreen = createVec3 (-1920, -1080, 0);
	ApplyLocation (yesButtonAetData, &offscreen);
	ApplyLocation (noButtonAetData, &offscreen);

	PlayAetLayer (menuAetData, menuAetId);
	PlayAetLayer (yesButtonAetData, yesButtonAetId);
	PlayAetLayer (noButtonAetData, noButtonAetId);

	PlaySoundEffect ("se_ft_sys_cansel_01", 1.0);

	wantsToExit   = false;
	hoveredButton = 0;
}

void
initMenu () {
	wantsToExit = true;

	CreateAetLayerData (menuAetData, 0x4F8, "dialog_01", 0x12, AETACTION_IN_LOOP);
	menuAetId = PlayAetLayer (menuAetData, 0);

	initCompositionData (&compositionData);
	GetComposition (&compositionData, menuAetId);

	float *yesButtonPlaceholderData = GetCompositionLayer (&compositionData, "p_submenu_03_c");
	if (yesButtonPlaceholderData) {
		yesButtonLoc  = createVec3 (yesButtonPlaceholderData[16], yesButtonPlaceholderData[17], yesButtonPlaceholderData[18]);
		yesButtonRect = getPlaceholderRect (yesButtonPlaceholderData);
	}
	float *noButtonPlaceholderData = GetCompositionLayer (&compositionData, "p_submenu_04_c");
	if (noButtonPlaceholderData) {
		noButtonLoc  = createVec3 (noButtonPlaceholderData[16], noButtonPlaceholderData[17], noButtonPlaceholderData[18]);
		noButtonRect = getPlaceholderRect (noButtonPlaceholderData);
	}

	CreateAetLayerData (yesButtonAetData, 0x4F8, yesButtonName, 0x13, AETACTION_IN);
	CreateAetLayerData (noButtonAetData, 0x4F8, noButtonName, 0x13, AETACTION_LOOP);

	ApplyLocation (yesButtonAetData, &yesButtonLoc);
	ApplyLocation (noButtonAetData, &noButtonLoc);

	yesButtonAetId = PlayAetLayer (yesButtonAetData, 0);
	noButtonAetId  = PlayAetLayer (noButtonAetData, 0);

	PlaySoundEffect ("se_ft_sys_enter_01", 1.0);
}

void
handleClick (Vec2 clickedPos) {
	if (vec4ContainsVec2 (yesButtonRect, clickedPos)) {
		if (hoveredButton == 0) moveUp ();
		else ExitProcess (0);
	} else if (vec4ContainsVec2 (noButtonRect, clickedPos)) {
		if (hoveredButton == 1) moveDown ();
		else leaveMenu ();
	}
}

// Custom exit menu
HOOK (bool, __thiscall, CsMenuLoop, 0x1401B29D0, u64 This) {
	if (*(i32 *)(This + 0x68) != 2) return originalCsMenuLoop (This);

	void *inputState = DivaGetInputState (0);
	if (wantsToExit) {
		Vec2 clickedPos = getClickedPos (inputState);
		if (clickedPos.x > 0 && !hasClicked) {
			handleClick (clickedPos);
			hasClicked = true;
		} else if (clickedPos.x == 0) {
			hasClicked = false;
		}

		if (IsButtonTapped (inputState, BUTTON_BACK)) leaveMenu ();
		else if (IsButtonTapped (inputState, BUTTON_ACCEPT) && hoveredButton == 0) leaveMenu ();
		else if (IsButtonTapped (inputState, BUTTON_ACCEPT) && hoveredButton == 1) ExitProcess (0);
		else if (IsButtonTapped (inputState, BUTTON_UP) && hoveredButton == 0) moveUp ();
		else if (IsButtonTapped (inputState, BUTTON_DOWN) && hoveredButton == 1) moveDown ();
		return false;
	}

	if (IsButtonTapped (inputState, BUTTON_BACK)) initMenu ();

	return originalCsMenuLoop (This);
}

void
init () {
	yesButtonName = appendTheme ("cmn_menu_yes");
	noButtonName  = appendTheme ("cmn_menu_no");
	INSTALL_HOOK (CsMenuLoop);
}
} // namespace exitMenu
