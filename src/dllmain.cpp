#include "SigScan.h"
#include "helpers.h"
#include "isteamfriends.h"
#include "state.h"
#include "toml.h"

SIG_SCAN (sigPvDbSwitch0, 0x140CBE1F0, "PV_DB_SWITCH", "xxxxxxxxxxx");
SIG_SCAN (sigPvDbSwitch1, 0x140CBE200, "pv_db_switch.txt", "xxxxxxxxxxxxxxx");

const char *store_url   = "https://divamodarchive.com";
const char *credits_url = "https://divamodarchive.com";
i32 value;

// a1 here is a string *
// From credits and manual
HOOK (u8, __stdcall, printURL, 0x1401DE9F0, void *a1) {
	SteamFriends ()->ActivateGameOverlayToWebPage (credits_url, k_EActivateGameOverlayToWebPageMode_Modal);
	return 0;
}

// From store button
HOOK (u8, __stdcall, printOpenDialog, 0x1401DEA20) {
	SteamFriends ()->ActivateGameOverlayToWebPage (store_url, k_EActivateGameOverlayToWebPageMode_Modal);
	return 0;
}

FUNCTION_PTR (bool, __stdcall, CmnMenuTaskDest, 0x1401AAE50, u64 data);
HOOK (void, __stdcall, ChangeSubGameState, 0x152C49DD0, State state, SubState subState) {
	static bool wantsToSettings = false;
	if (state == STATE_MENU_SWITCH) {
		state = STATE_CS_MENU;
	} else if (state == STATE_GAME_SWITCH) {
		state = STATE_GAME;
	} else if (subState == SUBSTATE_CS_OPTION_MENU) {
		state           = STATE_MENU_SWITCH;
		subState        = SUBSTATE_OPTION_MENU_SWITCH;
		wantsToSettings = true;
		CmnMenuTaskDest (0x14114C370);
	} else if (subState == SUBSTATE_MENU_SWITCH) {
		if (wantsToSettings) {
			subState        = SUBSTATE_OPTION_MENU_SWITCH;
			wantsToSettings = false;
		} else {
			state    = STATE_CS_MENU;
			subState = SUBSTATE_CS_MENU;
		}
	}
	return originalChangeSubGameState (state, subState);
}

// HideTextBox loads out anims but they arent played
FUNCTION_PTR (void, __stdcall, DrawTextBox, 0x1401ACDA0, u64 a1, i32 index);
FUNCTION_PTR (void, __stdcall, HideTextBox, 0x1401ACAD0, u64 a1, i32 index);
HOOK (bool, __stdcall, CsGalleryTaskCtrl, 0x1401AD590, u64 data) {
	static i32 previousButton = 4;
	i32 state                 = *(i32 *)(data + 104);
	i32 selectedButton        = *(i32 *)(data + 112);
	if (state == 3 && previousButton != selectedButton) {
		HideTextBox (data + 1168, previousButton);
		DrawTextBox (data + 1168, selectedButton);
		previousButton = selectedButton;
	} else if (state == 6) {
		*(i32 *)(data + 108)   = 1;
		*(i32 *)(data + 17816) = 5;
		*(i32 *)(data + 104)   = 14;
		previousButton         = 4;
	} else if (state == 4) {
		previousButton = selectedButton + 1;
		if (previousButton == 5) previousButton = 3;
	}

	return originalCsGalleryTaskCtrl (data);
}

typedef struct Vec3 {
	float x;
	float y;
	float z;
} Vec3;

Vec3
createVec3 (float x, float y, float z) {
	return Vec3{ x, y, z };
}

FUNCTION_PTR (u64, __stdcall, DivaGetInputState, 0x1402AC960, i32 a1);
FUNCTION_PTR (bool, __stdcall, IsButtonTapped, 0x1402AB250, u64 state, i32 offset);
FUNCTION_PTR (void, __stdcall, LoadAet, 0x14028D550, void *data, i32 aetSceneId, char *layerName, i32 layer, i32 action);
FUNCTION_PTR (i32, __stdcall, PlayAet, 0x1402CA1E0, void *data, u64 a2);
FUNCTION_PTR (void *, __stdcall, GetPlaceholders, 0x1402CA630, void *placeholderData, i32 id);
FUNCTION_PTR (void *, __stdcall, GetPlaceholder, 0x1402CA740, void *placeholderData, char *name);
FUNCTION_PTR (void, __stdcall, ApplyPlaceholder, 0x14065fa00, void *data, Vec3 *placeholderLocation);
HOOK (bool, __stdcall, CsMenuTaskCtrl, 0x1401B29D0, u64 data) {
	static bool wantsToExit       = false;
	static void *menuAetData      = calloc (1, 352);
	static void *yesButtonAetData = calloc (1, 352);
	static void *noButtonAetData  = calloc (1, 352);
	static i32 menuAetId          = 0;
	static i32 yesButtonAetId     = 0;
	static i32 noButtonAetId      = 0;
	static i32 hoveredButton      = 0;
	static Vec3 yesButtonLoc;
	static Vec3 noButtonLoc;

	if (wantsToExit) {
		if (IsButtonTapped (DivaGetInputState (0), 9)) { // BACK
			goto LEAVE;
		} else if (IsButtonTapped (DivaGetInputState (0), 10)) { // SELECT
			if (hoveredButton == 0) {
				goto LEAVE;
			} else {
			}
		} else if (IsButtonTapped (DivaGetInputState (0), 3) && hoveredButton == 0) { // UP
			hoveredButton = 1;
			LoadAet (yesButtonAetData, 0x4F8, (char *)"cmn_menu_yes", 0x12, 3);
			LoadAet (noButtonAetData, 0x4F8, (char *)"cmn_menu_no", 0x12, 1);

			ApplyPlaceholder (yesButtonAetData, &yesButtonLoc);
			ApplyPlaceholder (noButtonAetData, &noButtonLoc);

			PlayAet (yesButtonAetData, yesButtonAetId);
			PlayAet (noButtonAetData, noButtonAetId);
		} else if (IsButtonTapped (DivaGetInputState (0), 4) && hoveredButton == 1) { // Down
			hoveredButton = 0;
			LoadAet (yesButtonAetData, 0x4F8, (char *)"cmn_menu_yes", 0x12, 1);
			LoadAet (noButtonAetData, 0x4F8, (char *)"cmn_menu_no", 0x12, 3);

			ApplyPlaceholder (yesButtonAetData, &yesButtonLoc);
			ApplyPlaceholder (noButtonAetData, &noButtonLoc);

			PlayAet (yesButtonAetData, yesButtonAetId);
			PlayAet (noButtonAetData, noButtonAetId);
		}
		return false;
	LEAVE:
		LoadAet (menuAetData, 0x4F8, (char *)"dialog_01", 0x12, 4);
		PlayAet (menuAetData, menuAetId);

		wantsToExit = false;
		return false;
	}

	wantsToExit = IsButtonTapped (DivaGetInputState (0), 9);
	if (wantsToExit) {
		LoadAet (menuAetData, 0x4F8, (char *)"dialog_01", 0x12, 2);
		menuAetId = PlayAet (menuAetData, 0);

		void *placeholderData                 = malloc (0xB0);
		*(void **)placeholderData             = placeholderData;
		*(void **)((u64)placeholderData + 8)  = placeholderData;
		*(void **)((u64)placeholderData + 16) = placeholderData;
		*(u16 *)((u64)placeholderData + 24)   = 0x101;
		GetPlaceholders (&placeholderData, menuAetId);
		void *yesButtonPlaceholderData = GetPlaceholder (&placeholderData, (char *)"p_submenu_03_c");
		yesButtonLoc = createVec3 (*(float *)((u64)yesButtonPlaceholderData + 0x40), *(float *)((u64)yesButtonPlaceholderData + 0x44), *(float *)((u64)yesButtonPlaceholderData + 0x48));
		void *noButtonPlaceholderData = GetPlaceholder (&placeholderData, (char *)"p_submenu_04_c");
		noButtonLoc                   = createVec3 (*(float *)((u64)noButtonPlaceholderData + 0x40), *(float *)((u64)noButtonPlaceholderData + 0x44), *(float *)((u64)noButtonPlaceholderData + 0x48));

		LoadAet (yesButtonAetData, 0x4F8, (char *)"cmn_menu_yes", 0x13, 1);
		LoadAet (noButtonAetData, 0x4F8, (char *)"cmn_menu_no", 0x13, 3);

		ApplyPlaceholder (yesButtonAetData, &yesButtonLoc);
		ApplyPlaceholder (noButtonAetData, &noButtonLoc);

		yesButtonAetId = PlayAet (yesButtonAetData, 0);
		noButtonAetId  = PlayAet (noButtonAetData, 0);
	}
	return originalCsMenuTaskCtrl (data);
}

HOOK (bool, __stdcall, CustomizeSelTaskInit, 0x140687A50, u64 data) {
	CmnMenuTaskDest (0x14114C370);
	return originalCustomizeSelTaskInit (data);
}

HOOK (i32 *, __stdcall, GetFtTheme, 0x1401D6530) { return &value; }

extern "C" __declspec(dllexport) void Init () {
	INSTALL_HOOK (printURL);
	INSTALL_HOOK (printOpenDialog);
	INSTALL_HOOK (ChangeSubGameState);
	INSTALL_HOOK (CsGalleryTaskCtrl);
	INSTALL_HOOK (CsMenuTaskCtrl);
	INSTALL_HOOK (CustomizeSelTaskInit);
	INSTALL_HOOK (GetFtTheme);

	// 1.00 Samyuu, 1.02 BroGamer
	WRITE_MEMORY (0x1414AB9E3, u8, 0x01);

	// Stop calls to DrawTextBox
	WRITE_NOP (0x1401AD859, 5);
	WRITE_NOP (0x1401AD713, 5);

	// Stop call to HideTextBox
	WRITE_NOP (0x1401AD64C, 5);

	// Fix SFX select layering
	WRITE_MEMORY (0x140698D40, u8, 0x0A);
	toml_table_t *config = openConfig ((char *)"config.toml");
	value                = readConfigInt (config, (char *)"theme", 0);
}

extern "C" __declspec(dllexport) void PreInit () {
	WRITE_NULL (sigPvDbSwitch0 (), 1);
	WRITE_NULL (sigPvDbSwitch1 (), 1);
}
