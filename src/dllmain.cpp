#include "SigScan.h"
#include "helpers.h"
#include "isteamfriends.h"

SIG_SCAN (sigPvDbSwitch0, 0x140CBE1F0, "PV_DB_SWITCH", "xxxxxxxxxxx");
SIG_SCAN (sigPvDbSwitch1, 0x140CBE200, "pv_db_switch.txt", "xxxxxxxxxxxxxxx");

const char *url = (char *)"https://divamodarchive.xyz";

// a1 here is a string *
HOOK (u8, __stdcall, printURL, 0x1401de9f0, void *a1) {
	SteamFriends ()->ActivateGameOverlayToWebPage (
		url, k_EActivateGameOverlayToWebPageMode_Modal);
	return 0;
}

HOOK (u8, __stdcall, printOpenDialog, 0x1401dea20) {
	SteamFriends ()->ActivateGameOverlayToWebPage (
		url, k_EActivateGameOverlayToWebPageMode_Modal);
	return 0;
}

extern "C" __declspec(dllexport) void Init () {
	INSTALL_HOOK (printURL);
	INSTALL_HOOK (printOpenDialog);

	// 1.00 Samyuu, 1.02 BroGamer
	WRITE_MEMORY (0x1414AB9E3, u8, 0x01);
}

extern "C" __declspec(dllexport) void PreInit () {
	WRITE_NULL (sigPvDbSwitch0 (), 1);
	WRITE_NULL (sigPvDbSwitch1 (), 1);
}
