#include "SigScan.h"
#include "diva.h"
#include "helpers.h"
#include "menus/menus.h"
#include <vector>

SIG_SCAN_STRING (sigPvDbSwitch, "pv_db_switch.txt");
SIG_SCAN_STRING (sigMenuTxt1, "menu_txt_01");
SIG_SCAN_STRING (sigMenuTxt2, "menu_txt_02");
SIG_SCAN_STRING (sigMenuTxt3, "menu_txt_03");
SIG_SCAN_STRING (sigMenuTxt4, "menu_txt_04");
SIG_SCAN_STRING (sigMenuTxt5, "menu_txt_05");
SIG_SCAN_STRING (sigMenuTxtBase, "menu_txt_base");

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

// Fixes the header/footer being present on customize
HOOK (bool, __thiscall, CustomizeSelInit, 0x140687D10, u64 This) {
	CmnMenuDestroy (0x14114C370);
	shaderSel::hide ();
	return originalCustomizeSelInit (This);
}

HOOK (i32 *, __stdcall, GetFtTheme, 0x1401D6540) { return &theme; }

// Fixes gallery photos
HOOK (void, __stdcall, LoadAndPlayAet, 0x1401AF0E0, u64 data, AetAction action) {
	CreateAetLayerData ((void *)data, 0x4FE, *(char **)(data + 0x08), *(i32 *)(data + 0x84), action);
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
                                          "nswgam_cmnbg_bg",
                                          "nswgam_adv_bg",
                                          "press_a_button"};

HOOK (void *, __stdcall, CreateAetH, 0x14028D560, void *data, i32 aetSceneId, const char *layerName, i32 layer, AetAction action, u64 a6) {
	if (layerName == 0) return originalCreateAetH (data, aetSceneId, layerName, layer, action, a6);
	for (auto str : themeStrings) {
		if (strcmp (str, layerName) == 0) {
			const char *theme = appendTheme (layerName);
			return originalCreateAetH (data, aetSceneId, theme, layer, action, a6);
		}
	}
	return originalCreateAetH (data, aetSceneId, layerName, layer, action, a6);
}

HOOK (void, __stdcall, CreateAet2H, 0x14028DE70, void *data, i32 aetSceneId, const char *layerName, i32 layer, const char *start_marker, const char *end_marker, const char *loop_marker,
      AetAction action) {
	if (layerName == 0) return originalCreateAet2H (data, aetSceneId, layerName, layer, start_marker, end_marker, loop_marker, action);
	for (auto str : themeStrings) {
		if (strcmp (str, layerName) == 0) {
			const char *theme = appendTheme (layerName);
			return originalCreateAet2H (data, aetSceneId, theme, layer, start_marker, end_marker, loop_marker, action);
		}
	}
	return originalCreateAet2H (data, aetSceneId, layerName, layer, start_marker, end_marker, loop_marker, action);
}

HOOK (void, __stdcall, CreateAetFrameH, 0x1402CA590, void *data, i32 aetSceneId, const char *layerName, AetAction action, i32 layer, char *a6, float frame) {
	if (layerName == 0) return originalCreateAetFrameH (data, aetSceneId, layerName, action, layer, a6, frame);
	for (auto str : themeStrings) {
		if (strcmp (str, layerName) == 0) {
			const char *theme = appendTheme (layerName);
			return originalCreateAetFrameH (data, aetSceneId, theme, action, layer, a6, frame);
		}
	}
	return originalCreateAetFrameH (data, aetSceneId, layerName, action, layer, a6, frame);
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
	INSTALL_HOOK (CustomizeSelInit);
	INSTALL_HOOK (GetFtTheme);
	INSTALL_HOOK (LoadAndPlayAet);
	INSTALL_HOOK (CustomizeSelIsLoaded);
	INSTALL_HOOK (CreateAetH);
	INSTALL_HOOK (CreateAet2H);
	INSTALL_HOOK (CreateAetFrameH);

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

	// Use Left/Right on FX select
	WRITE_MEMORY (0x14069997F, Button, BUTTON_LEFT);
	WRITE_MEMORY (0x1406999A6, Button, BUTTON_RIGHT);

	// Use AETACTION_IN_LOOP
	WRITE_MEMORY (0x14066451D, AetAction, AETACTION_IN_LOOP);
	WRITE_MEMORY (0x140654505, AetAction, AETACTION_IN_LOOP);
	WRITE_MEMORY (0x1401A9CF8, AetAction, AETACTION_IN_LOOP);

	exitMenu::init ();
	shaderSel::init ();
	gallery::init ();
	options::init ();
	pause::init ();
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
