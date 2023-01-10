#include "exit.h"
#include "divaFuncs.h"
#include "helpers.h"
#include "types.h"

bool wantsToExit       = false;
bool hasClicked        = false;
void *menuAetData      = calloc (1, 0x1000);
void *yesButtonAetData = calloc (1, 0x1000);
void *noButtonAetData  = calloc (1, 0x1000);
char *yesButtonName    = appendTheme ("cmn_menu_yes");
char *noButtonName     = appendTheme ("cmn_menu_no");
i32 menuAetId          = 0;
i32 yesButtonAetId     = 0;
i32 noButtonAetId      = 0;
i32 hoveredButton      = 0;
Vec3 yesButtonLoc;
Vec3 noButtonLoc;
Vec4 yesButtonRect;
Vec4 noButtonRect;

void
moveDown () {
	hoveredButton = 0;
	LoadAet (yesButtonAetData, 0x4F8, yesButtonName, 0x13, 1);
	LoadAet (noButtonAetData, 0x4F8, noButtonName, 0x13, 3);

	ApplyPlaceholder (yesButtonAetData, &yesButtonLoc);
	ApplyPlaceholder (noButtonAetData, &noButtonLoc);

	PlayAet (yesButtonAetData, yesButtonAetId);
	PlayAet (noButtonAetData, noButtonAetId);
	PlaySoundEffect ("se_ft_sys_select_01", 1.0);
}

void
moveUp () {
	hoveredButton = 1;
	LoadAet (yesButtonAetData, 0x4F8, yesButtonName, 0x13, 3);
	LoadAet (noButtonAetData, 0x4F8, noButtonName, 0x13, 1);

	ApplyPlaceholder (yesButtonAetData, &yesButtonLoc);
	ApplyPlaceholder (noButtonAetData, &noButtonLoc);

	PlayAet (yesButtonAetData, yesButtonAetId);
	PlayAet (noButtonAetData, noButtonAetId);

	PlaySoundEffect ("se_ft_sys_select_01", 1.0);
}

void
leaveMenu () {
	LoadAet (menuAetData, 0x4F8, "dialog_01", 0x12, 4);
	PlayAet (menuAetData, menuAetId);

	Vec3 offscreen = createVec3 (-1920, -1080, 0);
	ApplyPlaceholder (yesButtonAetData, &offscreen);
	ApplyPlaceholder (noButtonAetData, &offscreen);

	PlayAet (yesButtonAetData, yesButtonAetId);
	PlayAet (noButtonAetData, noButtonAetId);

	PlaySoundEffect ("se_ft_sys_cansel_01", 1.0);

	wantsToExit   = false;
	hoveredButton = 0;
}

void
initMenu () {
	wantsToExit = true;

	LoadAet (menuAetData, 0x4F8, "dialog_01", 0x12, 2);
	menuAetId = PlayAet (menuAetData, 0);

	void *placeholderData                 = calloc (1, 0xB0);
	*(void **)placeholderData             = placeholderData;
	*(void **)((u64)placeholderData + 8)  = placeholderData;
	*(void **)((u64)placeholderData + 16) = placeholderData;
	*(u16 *)((u64)placeholderData + 24)   = 0x101;
	GetPlaceholders (&placeholderData, menuAetId);

	float *yesButtonPlaceholderData = GetPlaceholder (&placeholderData, "p_submenu_03_c");
	yesButtonLoc                    = createVec3 (yesButtonPlaceholderData[16], yesButtonPlaceholderData[17], yesButtonPlaceholderData[18]);
	float *noButtonPlaceholderData  = GetPlaceholder (&placeholderData, "p_submenu_04_c");
	noButtonLoc                     = createVec3 (noButtonPlaceholderData[16], noButtonPlaceholderData[17], noButtonPlaceholderData[18]);

	LoadAet (yesButtonAetData, 0x4F8, yesButtonName, 0x13, 1);
	LoadAet (noButtonAetData, 0x4F8, noButtonName, 0x13, 3);

	ApplyPlaceholder (yesButtonAetData, &yesButtonLoc);
	ApplyPlaceholder (noButtonAetData, &noButtonLoc);

	yesButtonRect = getPlaceholderRect (yesButtonPlaceholderData);
	noButtonRect  = getPlaceholderRect (noButtonPlaceholderData);

	yesButtonAetId = PlayAet (yesButtonAetData, 0);
	noButtonAetId  = PlayAet (noButtonAetData, 0);

	PlaySoundEffect ("se_ft_sys_enter_01", 1.0);
}

void
handleClick (i32 clickedX, i32 clickedY) {
	RECT rect;
	GetClientRect (FindWindow ("DIVAMIXP", 0), &rect);
	float x  = clickedX / (float)rect.right * 1920;
	float y  = clickedY / (float)rect.bottom * 1080;
	Vec2 vec = createVec2 (x, y);
	if (vec4ContainsVec2 (yesButtonRect, vec)) {
		if (hoveredButton == 0) {
			moveUp ();
		} else {
			ExitProcess (0);
		}
	} else if (vec4ContainsVec2 (noButtonRect, vec)) {
		if (hoveredButton == 1) {
			moveDown ();
		} else {
			leaveMenu ();
		}
	}
	hasClicked = true;
}

// Custom exit menu
HOOK (bool, __thiscall, CsMenuTaskCtrl, 0x1401B29D0, void *This) {
	void *inputState = DivaGetInputState (0);
	if (wantsToExit) {
		i32 clickedX = *(i32 *)((u64)inputState + 0x158);
		i32 clickedY = *(i32 *)((u64)inputState + 0x15C);
		if (clickedX > 0 && !hasClicked) {
			handleClick (clickedX, clickedY);
		} else if (clickedX == 0) {
			hasClicked = false;
		}

		if (IsButtonTapped (inputState, 9)) { // Back
			leaveMenu ();
		} else if (IsButtonTapped (inputState, 10)) { // Select
			if (hoveredButton == 0) {
				leaveMenu ();
			} else {
				ExitProcess (0);
			}
		} else if (IsButtonTapped (inputState, 3) && hoveredButton == 0) { // Up
			moveUp ();
		} else if (IsButtonTapped (inputState, 4) && hoveredButton == 1) { // Down
			moveDown ();
		}
		return false;
	}

	if (IsButtonTapped (inputState, 9)) { // Back
		initMenu ();
		return false;
	}

	return originalCsMenuTaskCtrl (This);
}

void
customExitMenuInit () {
	INSTALL_HOOK (CsMenuTaskCtrl);
}
