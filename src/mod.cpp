#include "SigScan.h"
#include "diva.h"
#include "helpers.h"
#include "menus/menus.h"
#include <vector>

SIG_SCAN (sigPvDbSwitch, "pv_db_switch.txt");
SIG_SCAN (sigMenuTxt1, "menu_txt_01");
SIG_SCAN (sigMenuTxt2, "menu_txt_02");
SIG_SCAN (sigMenuTxt3, "menu_txt_03");
SIG_SCAN (sigMenuTxt4, "menu_txt_04");
SIG_SCAN (sigMenuTxt5, "menu_txt_05");
SIG_SCAN (sigMenuTxtBase, "menu_txt_base");

i32 theme;

bool wantsToSettings = false;
HOOK (void, __stdcall, ChangeSubGameState, 0x1527E49E0, State state, SubState subState) {
	if (state == STATE_MENU_SWITCH) {
		state = STATE_CS_MENU;
	} else if (subState == SUBSTATE_CS_OPTION_MENU) {
		state           = STATE_MENU_SWITCH;
		subState        = SUBSTATE_OPTION_MENU_SWITCH;
		wantsToSettings = true;
		CmnMenuDestroy (0x14114C370);
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
		LoadAetLayer (menuTxt1Data, 0x4FE, "menu_txt_01", 0x12, action);
		menuTxt1Id = PlayAetLayer (menuTxt1Data, menuTxt1Id);
		break;
	case 1:
		LoadAetLayer (menuTxt2Data, 0x4FE, "menu_txt_02", 0x12, action);
		menuTxt2Id = PlayAetLayer (menuTxt2Data, menuTxt2Id);
		break;
	case 2:
		LoadAetLayer (menuTxt3Data, 0x4FE, "menu_txt_03", 0x12, action);
		menuTxt3Id = PlayAetLayer (menuTxt3Data, menuTxt3Id);
		break;
	case 3:
		LoadAetLayer (menuTxt4Data, 0x4FE, "menu_txt_04", 0x12, action);
		menuTxt4Id = PlayAetLayer (menuTxt4Data, menuTxt4Id);
		break;
	case 4:
		LoadAetLayer (menuTxt5Data, 0x4FE, "menu_txt_05", 0x12, action);
		menuTxt5Id = PlayAetLayer (menuTxt5Data, menuTxt5Id);
		break;
	}
	LoadAetLayer (menuTxtBaseData, 0x4FE, "menu_txt_base", 0x12, action);
	menuTxtBaseId = PlayAetLayer (menuTxtBaseData, menuTxtBaseId);
}

// Fixes gallery not properly exiting
i32 previousButton = 5;
HOOK (bool, __thiscall, CsGalleryLoop, 0x1401AD590, u64 This) {
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

	return originalCsGalleryLoop (This);
}

// Fixes the header/footer being present on customize
HOOK (bool, __thiscall, CustomizeSelInit, 0x140687D10, u64 This) {
	CmnMenuDestroy (0x14114C370);
	shaderSel::hide ();
	return originalCustomizeSelInit (This);
}

HOOK (i32 *, __stdcall, GetFtTheme, 0x1401D6540) { return &theme; }

// Fixes gallery photos
HOOK (void, __stdcall, LoadAndPlayAet, 0x1401AF0E0, u64 data, AetAction action) {
	LoadAetLayer ((void *)data, 0x4FE, *(char **)(data + 0x08), *(i32 *)(data + 0x84), action);
	PlayAetLayer ((void *)data, *(i32 *)(data + 0x15C));
}

// Fixes switching to customize from playlists
HOOK (bool, __stdcall, CustomizeSelIsLoaded, 0x140687CD0) {
	if (*(i32 *)0x14CC6F118 == 1) {
		if (implOfCustomizeSelInit (0x14CC6F100)) {
			*(i32 *)0x14CC6F118 = 2;
			*(i32 *)0x14CC6F124 = 2;
		}
	}

	return originalCustomizeSelIsLoaded ();
}

void *optionMenuTopData = calloc (1, 0x1024);
i32 optionMenuTopId     = 0;
HOOK (bool, __stdcall, OptionMenuSwitchInit, 0x1406C3CB0, void *a1) {
	LoadAetLayer (optionMenuTopData, 0x525, "option_top_menu loop", 7, AETACTION_NONE);
	optionMenuTopId = PlayAetLayer (optionMenuTopData, optionMenuTopId);
	return originalOptionMenuSwitchInit (a1);
}

HOOK (bool, __stdcall, OptionMenuSwitchDestroy, 0x1406C2CE0, void *a1) {
	StopAet (&optionMenuTopId);
	return originalOptionMenuSwitchDestroy (a1);
}

typedef enum PauseAction : i32 {
	Load         = 0,
	Unload       = 1,
	ButtonChange = 2,
	ButtonSelect = 3,
} PauseAction;

void *pauseMenuBackground = calloc (1, 0x1024);
i32 pauseMenuBackgroundId = 0;
bool playedOut            = 0;
HOOK (void, __stdcall, LoadPauseBackground, 0x1406570E0, u64 a1, bool playOut) {
	if (playOut && !playedOut) {
		playedOut = true;
		LoadAetLayer (pauseMenuBackground, 0x51C, "pause_win_add_base", 0x12, AETACTION_OUT);
		*(u64 *)((u64)pauseMenuBackground + 0x148) = a1 + 0xB0;
		pauseMenuBackgroundId                      = PlayAetLayer (pauseMenuBackground, pauseMenuBackgroundId);
	} else {
		playedOut = false;
		LoadAetLayer (pauseMenuBackground, 0x51C, "pause_win_add_base", 0x12, AETACTION_IN_LOOP);
		*(u64 *)((u64)pauseMenuBackground + 0x148) = a1 + 0xB0;
		pauseMenuBackgroundId                      = PlayAetLayer (pauseMenuBackground, pauseMenuBackgroundId);
	}
	originalLoadPauseBackground (a1, playOut);
}

HOOK (void, __stdcall, PauseExit, 0x14065B810, u64 a1) {
	if (!playedOut) {
		playedOut = true;
		LoadAetLayer (pauseMenuBackground, 0x51C, "pause_win_add_base", 0x12, AETACTION_OUT);
		*(u64 *)((u64)pauseMenuBackground + 0x148) = a1 + 0xB0;
		pauseMenuBackgroundId                      = PlayAetLayer (pauseMenuBackground, pauseMenuBackgroundId);
	}
	originalPauseExit (a1);
}

HOOK (void, __stdcall, PauseDestroy, 0x14065B100, u64 a1) {
	StopAet (&pauseMenuBackgroundId);
	originalPauseDestroy (a1);
}

std::vector<const char *> themeStrings = {"option_sub_menu_eachsong",
                                          "option_sub_menu_allsong",
                                          "timing",
                                          "option_sub_menu_vibration",
                                          "option_sub_menu_bgm_volume",
                                          "option_sub_menu_button_volume",
                                          "option_sub_menu_se_volume",
                                          "option_sub_menu_back",
                                          "gam_btn_resume",
                                          "gam_btn_timing",
                                          "gam_btn_retry",
                                          "gam_btn_option",
                                          "gam_btn_back",
                                          "gam_btn_back_playlist",
                                          "nswgam_cmnbg_bg"};

HOOK (void *, __stdcall, LoadAetH, 0x14028D560, void *data, i32 aetSceneId, const char *layerName, i32 layer, AetAction action, u64 a6) {
	if (layerName == 0) return originalLoadAetH (data, aetSceneId, layerName, layer, action, a6);
	for (auto str : themeStrings) {
		if (strcmp (str, layerName) == 0) {
			const char *theme = appendTheme (layerName);
			return originalLoadAetH (data, aetSceneId, theme, layer, action, a6);
		}
	}
	return originalLoadAetH (data, aetSceneId, layerName, layer, action, a6);
}

HOOK (void, __stdcall, LoadAet2H, 0x14028de70, void *data, i32 aetSceneId, const char *layerName, i32 layer, const char *start_marker, const char *end_marker, const char *loop_marker,
      AetAction action) {
	if (layerName == 0) return originalLoadAet2H (data, aetSceneId, layerName, layer, start_marker, end_marker, loop_marker, action);
	for (auto str : themeStrings) {
		if (strcmp (str, layerName) == 0) {
			const char *theme = appendTheme (layerName);
			return originalLoadAet2H (data, aetSceneId, theme, layer, start_marker, end_marker, loop_marker, action);
		}
	}
	return originalLoadAet2H (data, aetSceneId, layerName, layer, start_marker, end_marker, loop_marker, action);
}

HOOK (void, __stdcall, LoadAetFrameH, 0x1402CA590, void *data, i32 aetSceneId, const char *layerName, AetAction action, i32 layer, char *a6, float frame) {
	if (layerName == 0) return originalLoadAetFrameH (data, aetSceneId, layerName, action, layer, a6, frame);
	for (auto str : themeStrings) {
		if (strcmp (str, layerName) == 0) {
			const char *theme = appendTheme (layerName);
			return originalLoadAetFrameH (data, aetSceneId, theme, action, layer, a6, frame);
		}
	}
	return originalLoadAetFrameH (data, aetSceneId, layerName, action, layer, a6, frame);
}

#ifdef __cplusplus
extern "C" {
#endif

void
init () {
	// freopen ("CONOUT$", "w", stdout);

	toml_table_t *config = openConfig ("config.toml");
	theme                = readConfigInt (config, "theme", 0);

	INSTALL_HOOK (ChangeSubGameState);
	INSTALL_HOOK (CsGalleryLoop);
	INSTALL_HOOK (CustomizeSelInit);
	INSTALL_HOOK (GetFtTheme);
	INSTALL_HOOK (LoadAndPlayAet);
	INSTALL_HOOK (CustomizeSelIsLoaded);
	INSTALL_HOOK (OptionMenuSwitchInit);
	INSTALL_HOOK (OptionMenuSwitchDestroy);
	INSTALL_HOOK (LoadPauseBackground);
	INSTALL_HOOK (PauseExit);
	INSTALL_HOOK (PauseDestroy);
	INSTALL_HOOK (LoadAetH);
	INSTALL_HOOK (LoadAet2H);
	INSTALL_HOOK (LoadAetFrameH);

	// 1.00 Samyuu, 1.03 BroGamer
	WRITE_MEMORY (0x1414AB9E3, u8, 0x01);

	// Stop returning to ADV from main menu
	WRITE_NOP (0x1401B2ADA, 36);

	// Fix SFX select layering
	WRITE_MEMORY (0x140699000, u8, 0x0A);

	// Fix song select ordering
	WRITE_MEMORY (0x140BE9520, u64, 0x140C85CB0, 0x140C85CE8);

	// Reduce number of loading screens to 3
	WRITE_MEMORY (0x1406542B4, u8, 0x02);

	// Properly load rights_bg02
	WRITE_MEMORY (0x15E99F118, u8, 0x04);

	// Use AETACTION_IN_LOOP
	WRITE_MEMORY (0x14066451D, AetAction, AETACTION_IN_LOOP);
	WRITE_MEMORY (0x140654505, AetAction, AETACTION_IN_LOOP);

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
