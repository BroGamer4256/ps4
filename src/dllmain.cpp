#include "SigScan.h"
#include "helpers.h"
#include "state.h"
#include "toml.h"

SIG_SCAN (sigPvDbSwitch0, 0x140CBE1F0, "PV_DB_SWITCH", "xxxxxxxxxxx");
SIG_SCAN (sigPvDbSwitch1, 0x140CBE200, "pv_db_switch.txt", "xxxxxxxxxxxxxxx");

i32 theme;

FUNCTION_PTR (bool, __thiscall, CmnMenuTaskDest, 0x1401AAE50, u64 data);
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

FUNCTION_PTR (void, __stdcall, DrawTextBox, 0x1401ACDA0, u64 a1, i32 index);
FUNCTION_PTR (void, __stdcall, HideTextBox, 0x1401ACAD0, u64 a1, i32 index);
HOOK (bool, __thiscall, CsGalleryTaskCtrl, 0x1401AD590, u64 data) {
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

typedef struct string {
	union {
		char data[16];
		char *ptr;
	};
	u64 length;
	u64 capacity;
} string;

FUNCTION_PTR (void, __stdcall, divaAppendTheme, 0x1405d96e0, string *str);
char *
appendTheme (const char *name) {
	char *themeStr = (char *)calloc (strlen (name) + 4, sizeof (char));
	strcpy (themeStr, name);
	switch (theme) {
	case 1: strcat (themeStr, "_f"); break;
	case 2: strcat (themeStr, "_t"); break;
	default: strcat (themeStr, "_ft"); break;
	}

	return themeStr;
}

FUNCTION_PTR (void *, __stdcall, DivaGetInputState, 0x1402AC960, i32 a1);
FUNCTION_PTR (bool, __stdcall, IsButtonTapped, 0x1402AB250, void *state, i32 offset);
FUNCTION_PTR (void, __stdcall, LoadAet, 0x14028D550, void *data, i32 aetSceneId, const char *layerName, i32 layer, i32 action);
FUNCTION_PTR (i32, __stdcall, PlayAet, 0x1402CA1E0, void *data, u64 a2);
FUNCTION_PTR (void *, __stdcall, GetPlaceholders, 0x1402CA630, void *placeholderData, i32 id);
FUNCTION_PTR (float *, __stdcall, GetPlaceholder, 0x1402CA740, void *placeholderData, const char *name);
FUNCTION_PTR (void, __stdcall, ApplyPlaceholder, 0x14065fa00, void *data, Vec3 *placeholderLocation);
FUNCTION_PTR (void, __stdcall, PlaySoundEffect, 0x1405AA500, const char *name, float volume);
HOOK (bool, __thiscall, CsMenuTaskCtrl, 0x1401B29D0, u64 data) {
	static bool wantsToExit = false;
	// Overkill but dont want to fuck with memory bugs
	static void *menuAetData      = calloc (1, 0x1000);
	static void *yesButtonAetData = calloc (1, 0x1000);
	static void *noButtonAetData  = calloc (1, 0x1000);
	static char *yesButtonName    = appendTheme ("cmn_menu_yes");
	static char *noButtonName     = appendTheme ("cmn_menu_no");
	static i32 menuAetId          = 0;
	static i32 yesButtonAetId     = 0;
	static i32 noButtonAetId      = 0;
	static i32 hoveredButton      = 0;
	static Vec3 yesButtonLoc;
	static Vec3 noButtonLoc;

	void *inputState = DivaGetInputState (0);

	if (wantsToExit) {
		if (IsButtonTapped (inputState, 9)) { // BACK
			goto LEAVE;
		} else if (IsButtonTapped (inputState, 10)) { // SELECT
			if (hoveredButton == 0) {
				goto LEAVE;
			} else {
				ExitProcess (0);
			}
		} else if (IsButtonTapped (inputState, 3) && hoveredButton == 0) { // UP
			hoveredButton = 1;
			LoadAet (yesButtonAetData, 0x4F8, yesButtonName, 0x13, 3);
			LoadAet (noButtonAetData, 0x4F8, noButtonName, 0x13, 1);

			ApplyPlaceholder (yesButtonAetData, &yesButtonLoc);
			ApplyPlaceholder (noButtonAetData, &noButtonLoc);

			PlayAet (yesButtonAetData, yesButtonAetId);
			PlayAet (noButtonAetData, noButtonAetId);

			PlaySoundEffect ("se_ft_sys_select_01", 1.0);
		} else if (IsButtonTapped (inputState, 4) && hoveredButton == 1) { // Down
			hoveredButton = 0;
			LoadAet (yesButtonAetData, 0x4F8, yesButtonName, 0x13, 1);
			LoadAet (noButtonAetData, 0x4F8, noButtonName, 0x13, 3);

			ApplyPlaceholder (yesButtonAetData, &yesButtonLoc);
			ApplyPlaceholder (noButtonAetData, &noButtonLoc);

			PlayAet (yesButtonAetData, yesButtonAetId);
			PlayAet (noButtonAetData, noButtonAetId);
			PlaySoundEffect ("se_ft_sys_select_01", 1.0);
		}
		return false;
	LEAVE:
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
		return false;
	}

	if (IsButtonTapped (inputState, 9)) { // BACK
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

		yesButtonAetId = PlayAet (yesButtonAetData, 0);
		noButtonAetId  = PlayAet (noButtonAetData, 0);

		PlaySoundEffect ("se_ft_sys_enter_01", 1.0);
		return false;
	}

	return originalCsMenuTaskCtrl (data);
}

HOOK (bool, __thiscall, CustomizeSelTaskInit, 0x140687A50, u64 data) {
	CmnMenuTaskDest (0x14114C370);
	return originalCustomizeSelTaskInit (data);
}

HOOK (i32 *, __stdcall, GetFtTheme, 0x1401D6530) { return &theme; }

HOOK (void, __stdcall, LoadAndPlayAet, 0x1401AF0E0, u64 data, i32 action) {
	if (strcmp (*(char **)(data + 0x08), "menu_bg")) action = 6;
	LoadAet ((void *)data, 0x4FE, *(char **)(data + 0x08), *(i32 *)(data + 0x84), action);
	PlayAet ((void *)data, *(i32 *)(data + 0x15C));
}

extern "C" __declspec(dllexport) void Init () {
	INSTALL_HOOK (ChangeSubGameState);
	INSTALL_HOOK (CsGalleryTaskCtrl);
	INSTALL_HOOK (CsMenuTaskCtrl);
	INSTALL_HOOK (CustomizeSelTaskInit);
	INSTALL_HOOK (GetFtTheme);
	INSTALL_HOOK (LoadAndPlayAet);

	// 1.00 Samyuu, 1.02 BroGamer
	WRITE_MEMORY (0x1414AB9E3, u8, 0x01);

	// Stop calls to DrawTextBox
	WRITE_NOP (0x1401AD859, 5);
	WRITE_NOP (0x1401AD713, 5);

	// Stop call to HideTextBox
	WRITE_NOP (0x1401AD64C, 5);

	// Stop returning to ADV from main menu
	WRITE_NOP (0x1401B2ADA, 36);

	// Fix SFX select layering
	WRITE_MEMORY (0x140698D40, u8, 0x0A);

	toml_table_t *config = openConfig ("config.toml");
	theme                = readConfigInt (config, "theme", 0);
}

extern "C" __declspec(dllexport) void PreInit () {
	WRITE_NULL (sigPvDbSwitch0 (), 1);
	WRITE_NULL (sigPvDbSwitch1 (), 1);
}
