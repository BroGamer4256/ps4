#include "SigScan.h"
#include "helpers.h"
#include "isteamfriends.h"

SIG_SCAN (sigPvDbSwitch0, 0x140CBE1F0, "PV_DB_SWITCH", "xxxxxxxxxxx");
SIG_SCAN (sigPvDbSwitch1, 0x140CBE200, "pv_db_switch.txt", "xxxxxxxxxxxxxxx");

const char *store_url = (char *)"https://divamodarchive.xyz";
const char *credits_url = (char *)"https://divamodarchive.xyz";

// a1 here is a string *
// From credits and manual
HOOK (u8, __stdcall, printURL, 0x1401de9f0, void *a1) {
	SteamFriends ()->ActivateGameOverlayToWebPage (
		credits_url, k_EActivateGameOverlayToWebPageMode_Modal);
	return 0;
}

// From store button
HOOK (u8, __stdcall, printOpenDialog, 0x1401dea20) {
	SteamFriends ()->ActivateGameOverlayToWebPage (
		store_url, k_EActivateGameOverlayToWebPageMode_Modal);
	return 0;
}

HOOK (void, __stdcall, ChangeSubGameState, 0x152c49dd0, i32 state,
	  i32 subState) {
	if (state == 9)	 // MAINMENU_SWITCH
		state = 6;	 // CS_MENU
	if (state == 10) // GAME_SWITCH
		state = 2;	 // GAME
	return originalChangeSubGameState (state, subState);
}

FUNCTION_PTR (void, __fastcall, DrawTextBox, 0x1401acda0, u64 a1, i32 a2);
FUNCTION_PTR (void, __fastcall, HideTextBox, 0x1401acad0, u64 a1, i32 a2);
HOOK (u64, __fastcall, GalleryLoop, 0x1401AD590, u64 a1) {
	static i32 previous = 4;
	if (*(i32 *)(a1 + 104) == 3 && previous != *(i32 *)(a1 + 112)) {
		HideTextBox (a1 + 1168, previous);
		DrawTextBox (a1 + 1168, *(i32 *)(a1 + 112));
		previous = *(i32 *)(a1 + 112);
	} else if (*(i32 *)(a1 + 104) == 6) {
		*(i32 *)(a1 + 108) = 1;
		*(i32 *)(a1 + 17816) = 5;
		*(i32 *)(a1 + 104) = 14;
		previous = 4;
	} else if (*(i32 *)(a1 + 104) == 4) {
		previous = *(i32 *)(a1 + 112) + 1;
		if (previous == 5)
			previous = 3;
	}
	u64 value = originalGalleryLoop (a1);

	return value;
}

extern "C" __declspec(dllexport) void Init () {
	INSTALL_HOOK (printURL);
	INSTALL_HOOK (printOpenDialog);
	INSTALL_HOOK (ChangeSubGameState);
	INSTALL_HOOK (GalleryLoop);

	// 1.00 Samyuu, 1.02 BroGamer
	WRITE_MEMORY (0x1414AB9E3, u8, 0x01);

	// Stop calls to DrawTextBox
	WRITE_NOP (0x1401ad859, 5);
	WRITE_NOP (0x1401ad713, 5);

	// Stop call to HideTextBox
	WRITE_NOP (0x1401ad64c, 5);
}

extern "C" __declspec(dllexport) void PreInit () {
	WRITE_NULL (sigPvDbSwitch0 (), 1);
	WRITE_NULL (sigPvDbSwitch1 (), 1);
}
