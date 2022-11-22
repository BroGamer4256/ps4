#include "SigScan.h"
#include "helpers.h"
#include "isteamfriends.h"

SIG_SCAN (sigPvDbSwitch0, 0x140CBE1F0, "PV_DB_SWITCH", "xxxxxxxxxxx");
SIG_SCAN (sigPvDbSwitch1, 0x140CBE200, "pv_db_switch.txt", "xxxxxxxxxxxxxxx");

const char *store_url = "https://divamodarchive.com";
const char *credits_url = "https://divamodarchive.com";

// a1 here is a string *
// From credits and manual
HOOK (u8, __stdcall, printURL, 0x1401DE9F0, void *a1) {
	SteamFriends ()->ActivateGameOverlayToWebPage (
		credits_url, k_EActivateGameOverlayToWebPageMode_Modal);
	return 0;
}

// From store button
HOOK (u8, __stdcall, printOpenDialog, 0x1401DEA20) {
	SteamFriends ()->ActivateGameOverlayToWebPage (
		store_url, k_EActivateGameOverlayToWebPageMode_Modal);
	return 0;
}

HOOK (void, __stdcall, ChangeSubGameState, 0x152C49DD0, i32 state,
	  i32 subState) {
	if (state == 9)	 // MAINMENU_SWITCH
		state = 6;	 // CS_MENU
	if (state == 10) // GAME_SWITCH
		state = 2;	 // GAME
	return originalChangeSubGameState (state, subState);
}

// HideTextBox loads out anims but they arent played
FUNCTION_PTR (void, __stdcall, DrawTextBox, 0x1401ACDA0, u64 a1, i32 index);
FUNCTION_PTR (void, __stdcall, HideTextBox, 0x1401ACAD0, u64 a1, i32 index);
HOOK (bool, __stdcall, CsGalleryTaskCtrl, 0x1401AD590, u64 data) {
	static i32 previousButton = 4;
	i32 state = *(i32 *)(data + 104);
	i32 selectedButton = *(i32 *)(data + 112);
	if (state == 3 && previousButton != selectedButton) {
		HideTextBox (data + 1168, previousButton);
		DrawTextBox (data + 1168, selectedButton);
		previousButton = selectedButton;
	} else if (state == 6) {
		*(i32 *)(data + 108) = 1;
		*(i32 *)(data + 17816) = 5;
		*(i32 *)(data + 104) = 14;
		previousButton = 4;
	} else if (state == 4) {
		previousButton = selectedButton + 1;
		if (previousButton == 5)
			previousButton = 3;
	}

	return originalCsGalleryTaskCtrl (data);
}

FUNCTION_PTR (bool, __stdcall, CmnMenuTaskDest, 0x1401AAE50, u64 data);
HOOK (bool, __stdcall, CustomizeSelTaskInit, 0x140687A50, u64 data) {
	CmnMenuTaskDest (0x14114C370);
	return originalCustomizeSelTaskInit (data);
}

extern "C" __declspec(dllexport) void Init () {
	INSTALL_HOOK (printURL);
	INSTALL_HOOK (printOpenDialog);
	INSTALL_HOOK (ChangeSubGameState);
	INSTALL_HOOK (CsGalleryTaskCtrl);
	INSTALL_HOOK (CustomizeSelTaskInit);

	// 1.00 Samyuu, 1.02 BroGamer
	WRITE_MEMORY (0x1414AB9E3, u8, 0x01);

	// Stop calls to DrawTextBox
	WRITE_NOP (0x1401AD859, 5);
	WRITE_NOP (0x1401AD713, 5);

	// Stop call to HideTextBox
	WRITE_NOP (0x1401AD64C, 5);
}

extern "C" __declspec(dllexport) void PreInit () {
	WRITE_NULL (sigPvDbSwitch0 (), 1);
	WRITE_NULL (sigPvDbSwitch1 (), 1);
}
