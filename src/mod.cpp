#include "SigScan.h"
#include "diva.h"
#include "helpers.h"
#include "menus.h"

SIG_SCAN (sigPvDbSwitch, 0x140CBE200, "pv_db_switch.txt", "xxxxxxxxxxxxxxx");

i32 theme;

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

// Fixes gallery not properly exiting
HOOK (bool, __thiscall, CsGalleryTaskCtrl, 0x1401AD590, u64 This) {
	static i32 previousButton = 4;

	i32 state          = *(i32 *)(This + 104);
	i32 selectedButton = *(i32 *)(This + 112);
	if (state == 3 && previousButton != selectedButton) {
		HideTextBox (This + 1168, previousButton);
		DrawTextBox (This + 1168, selectedButton);
		previousButton = selectedButton;
	} else if (state == 6) {
		*(i32 *)(This + 108)   = 1;
		*(i32 *)(This + 17816) = 5;
		*(i32 *)(This + 104)   = 14;
		previousButton         = 4;
	} else if (state == 4) {
		previousButton = selectedButton + 1;
		if (previousButton == 5) previousButton = 3;
	}

	return originalCsGalleryTaskCtrl (This);
}

// Fixes the header/footer being present on customize
HOOK (bool, __thiscall, CustomizeSelTaskInit, 0x140687A50, u64 This) {
	CmnMenuTaskDest (0x14114C370);
	return originalCustomizeSelTaskInit (This);
}

HOOK (i32 *, __stdcall, GetFtTheme, 0x1401D6530) { return &theme; }

// Fixes gallery photos
HOOK (void, __stdcall, LoadAndPlayAet, 0x1401AF0E0, u64 data, i32 action) {
	LoadAet ((void *)data, 0x4FE, *(char **)(data + 0x08), *(i32 *)(data + 0x84), action);
	PlayAet ((void *)data, *(i32 *)(data + 0x15C));
}

// Fixes switching to customize from playlists
HOOK (bool, __stdcall, CustomizeSelIsLoaded, 0x140687A10) {
	if (*(i32 *)0x14CC6F118 != 2) {
		if (implOfCustomizeSelTaskInit (0x14CC6F100)) {
			*(i32 *)0x14CC6F118 = 2;
			*(i32 *)0x14CC6F124 = 2;
		}
	}

	return originalCustomizeSelIsLoaded ();
}

#ifdef __cplusplus
extern "C" {
#endif

void
init () {
	INSTALL_HOOK (ChangeSubGameState);
	INSTALL_HOOK (CsGalleryTaskCtrl);
	INSTALL_HOOK (CustomizeSelTaskInit);
	INSTALL_HOOK (GetFtTheme);
	INSTALL_HOOK (LoadAndPlayAet);
	INSTALL_HOOK (CustomizeSelIsLoaded);

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

	// Fix song select ordering
	WRITE_MEMORY (0x140BE9520, u64, 0x140C85CA0, 0x140C85CD8);

	toml_table_t *config = openConfig ("config.toml");
	theme                = readConfigInt (config, "theme", 0);

	appendThemeInPlaceString ((string *)0x140DCB300);

	customMenusHook ();
}

void
preInit () {
	WRITE_NULL (sigPvDbSwitch (), 1);
}

#ifdef __cplusplus
}
#endif
