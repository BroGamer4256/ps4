#include "SigScan.h"
#include "helpers.h"
#include "isteamfriends.h"

SIG_SCAN (sigPvDbSwitch0, 0x140CBE1F0, "PV_DB_SWITCH", "xxxxxxxxxxx");
SIG_SCAN (sigPvDbSwitch1, 0x140CBE200, "pv_db_switch.txt", "xxxxxxxxxxxxxxx");

// a1 here is a string
HOOK (u8, __stdcall, printURL, 0x1401de9f0, void *a1) {
	SteamFriends ()->ActivateGameOverlayToWebPage (
		"https://divamodarchive.xyz");
	return 0;
}

HOOK (u8, __stdcall, openDialog, 0x1401dea20) {
	SteamFriends ()->ActivateGameOverlayToWebPage (
		"https://divamodarchive.xyz");
	return 0;
}

extern "C" __declspec(dllexport) void Init () {
	INSTALL_HOOK (printURL);
	INSTALL_HOOK (openDialog);

	// 1.00 Samyuu, 1.02 BroGamer
	WRITE_MEMORY (0x1414AB9E3, u8, 0x01);
}

extern "C" __declspec(dllexport) void PreInit () {
	WRITE_MEMORY (sigPvDbSwitch0 (), u8, "XX_DB_SWITCH");
	WRITE_MEMORY (sigPvDbSwitch1 (), u8, "xx_db_switch.txt");
}
