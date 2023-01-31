#include "SigScan.h"
#include "diva.h"
#include "helpers.h"
#include "menus/menus.h"

SIG_SCAN (sigPvDbSwitch, "pv_db_switch.txt");
SIG_SCAN (sigMenuTxt1, "menu_txt_01");
SIG_SCAN (sigMenuTxt2, "menu_txt_02");
SIG_SCAN (sigMenuTxt3, "menu_txt_03");
SIG_SCAN (sigMenuTxt4, "menu_txt_04");
SIG_SCAN (sigMenuTxt5, "menu_txt_05");
SIG_SCAN (sigMenuTxtBase, "menu_txt_base");

i32 theme;

bool wantsToSettings = false;
HOOK (void, __stdcall, ChangeSubGameState, 0x152C49DD0, State state, SubState subState) {
	if (state == STATE_MENU_SWITCH) {
		state = STATE_CS_MENU;
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

void *menuTxt1Data    = calloc (1, 0x1024);
void *menuTxt2Data    = calloc (1, 0x1024);
void *menuTxt3Data    = calloc (1, 0x1024);
void *menuTxt4Data    = calloc (1, 0x1024);
void *menuTxt5Data    = calloc (1, 0x1024);
void *menuTxtBaseData = calloc (1, 0x1024);
i32 menuTxt1Id        = 0;
i32 menuTxt2Id        = 0;
i32 menuTxt3Id        = 0;
i32 menuTxt4Id        = 0;
i32 menuTxt5Id        = 0;
i32 menuTxtBaseId     = 0;
void
playGalleryTxt (i32 button, AetAction action) {
	switch (button) {
	case 0:
		LoadAet (menuTxt1Data, 0x4FE, "menu_txt_01", 0x12, action);
		menuTxt1Id = PlayAet (menuTxt1Data, menuTxt1Id);
		break;
	case 1:
		LoadAet (menuTxt2Data, 0x4FE, "menu_txt_02", 0x12, action);
		menuTxt2Id = PlayAet (menuTxt2Data, menuTxt2Id);
		break;
	case 2:
		LoadAet (menuTxt3Data, 0x4FE, "menu_txt_03", 0x12, action);
		menuTxt3Id = PlayAet (menuTxt3Data, menuTxt3Id);
		break;
	case 3:
		LoadAet (menuTxt4Data, 0x4FE, "menu_txt_04", 0x12, action);
		menuTxt4Id = PlayAet (menuTxt4Data, menuTxt4Id);
		break;
	case 4:
		LoadAet (menuTxt5Data, 0x4FE, "menu_txt_05", 0x12, action);
		menuTxt5Id = PlayAet (menuTxt5Data, menuTxt5Id);
		break;
	}
	LoadAet (menuTxtBaseData, 0x4FE, "menu_txt_base", 0x12, action);
	menuTxtBaseId = PlayAet (menuTxtBaseData, menuTxtBaseId);
}

// Fixes gallery not properly exiting
i32 previousButton = 5;
HOOK (bool, __thiscall, CsGalleryTaskCtrl, 0x1401AD590, u64 This) {
	i32 state          = *(i32 *)(This + 0x68);
	i32 selectedButton = *(i32 *)(This + 0x70);
	if (state == 3 && previousButton != selectedButton) {
		playGalleryTxt (selectedButton, AETACTION_IN_LOOP);
		if (previousButton != 5) playGalleryTxt (previousButton, AETACTION_OUT);
		previousButton = selectedButton;
	} else if (state == 6) {
		*(i32 *)(This + 0x68)   = 14;
		*(i32 *)(This + 0x6C)   = 1;
		*(i32 *)(This + 0x4598) = 5;
		previousButton          = 5;
		playGalleryTxt (selectedButton, AETACTION_OUT);
	} else if (state == 4) {
		previousButton = 5;
		playGalleryTxt (selectedButton, AETACTION_OUT);
	}

	return originalCsGalleryTaskCtrl (This);
}

// Fixes the header/footer being present on customize
HOOK (bool, __thiscall, CustomizeSelTaskInit, 0x140687A50, u64 This) {
	CmnMenuTaskDest (0x14114C370);
	shaderSel::hide ();
	return originalCustomizeSelTaskInit (This);
}

HOOK (i32 *, __stdcall, GetFtTheme, 0x1401D6530) { return &theme; }

// Fixes gallery photos
HOOK (void, __stdcall, LoadAndPlayAet, 0x1401AF0E0, u64 data, AetAction action) {
	LoadAet ((void *)data, 0x4FE, *(char **)(data + 0x08), *(i32 *)(data + 0x84), action);
	PlayAet ((void *)data, *(i32 *)(data + 0x15C));
}

// Fixes switching to customize from playlists
HOOK (bool, __stdcall, CustomizeSelIsLoaded, 0x140687A10) {
	if (*(i32 *)0x14CC6F118 == 1) {
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

	// Stop returning to ADV from main menu
	WRITE_NOP (0x1401B2ADA, 36);

	// Fix SFX select layering
	WRITE_MEMORY (0x140698D40, u8, 0x0A);

	// Fix song select ordering
	WRITE_MEMORY (0x140BE9520, u64, 0x140C85CA0, 0x140C85CD8);

	// Play load screen in
	WRITE_MEMORY (0x140654245, u8, 0x02);

	// Reduce number of loading screens to 3
	WRITE_MEMORY (0x140653FF4, u8, 0x02);

	// Properly load rights_bg02
	WRITE_MEMORY (0x15E65CFED, u8, 0x04);

	// Use AETACTION_IN_LOOP for cmn_win_help_y_inout
	WRITE_MEMORY (0x14066425D, AetAction, AETACTION_IN_LOOP);

	toml_table_t *config = openConfig ("config.toml");
	theme                = readConfigInt (config, "theme", 0);

	appendThemeInPlaceString ((String *)0x140DCB300);

	exitMenu::init ();
	shaderSel::init ();
}

void
preInit () {
	WRITE_NULL (sigPvDbSwitch (), 1);
	WRITE_NULL (sigMenuTxt1 (), 1);
	WRITE_NULL (sigMenuTxt2 (), 1);
	WRITE_NULL (sigMenuTxt3 (), 1);
	WRITE_NULL (sigMenuTxt4 (), 1);
	WRITE_NULL (sigMenuTxt5 (), 1);
	WRITE_NULL (sigMenuTxtBase (), 1);
}

#ifdef __cplusplus
}
#endif
