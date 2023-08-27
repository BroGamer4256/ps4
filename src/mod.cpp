#include "SigScan.h"
#include "diva.h"
#include "menus/menus.h"

i32 theme;

using diva::AetAction;
using diva::Button;
using diva::State;
using diva::SubState;

bool wantsToSettings = false;
HOOK (void, ChangeSubGameState, 0x1527E49E0, State state, SubState subState) {
	if (state == State::MENU_SWITCH) {
		state = State::CS_MENU;
	} else if (subState == SubState::CS_OPTION_MENU) {
		state           = State::MENU_SWITCH;
		subState        = SubState::OPTION_MENU_SWITCH;
		wantsToSettings = true;
		diva::CmnMenuDestroy (0x14114C370);
	} else if (subState == SubState::MENU_SWITCH) {
		if (wantsToSettings) {
			subState        = SubState::OPTION_MENU_SWITCH;
			wantsToSettings = false;
		} else {
			state    = State::CS_MENU;
			subState = SubState::CS_MENU;
		}
	}

	return originalChangeSubGameState (state, subState);
}

HOOK (i32 *, GetFtTheme, 0x1401D6540) { return &theme; }

std::set<std::string> themeStrings = {"option_sub_menu_eachsong",
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
                                      "press_a_button",
                                      "footer_01",
                                      "option_top_menu loop",
                                      "fotter01",
                                      "fotter02",
                                      "fotter03",
                                      "fotter04",
                                      "fotter05",
                                      "fotter08",
                                      "fotter09",
                                      "setting_menu_bg_arcade_base_in",
                                      "setting_menu_bg_arcade_base_up",
                                      "setting_menu_bg_arcade_base_down",
                                      "bg02",
                                      "footer_02",
                                      "footer_03",
                                      "nswgam_tshirtsedit_colorselector_bg",
                                      "nswgam_tshirtsedit_keyhelp_bg",
                                      "cmn_win_help",
                                      "btn_close",
                                      "savedata_warning_dialog",
                                      "cmn_win_m"};

HOOK (void *, PlayAetLayerH, 0x1402CA220, diva::AetLayerArgs *args, i32 *id) {
	if (args->layerName == 0) return originalPlayAetLayerH (args, id);
	if (themeStrings.find (args->layerName) != themeStrings.end ()) {
		if (strcmp (args->layerName, "gam_btn_retry") == 0 && diva::IsSurvival ()) {
			args->StartMarker = diva::string ("st_sp_02");
			args->EndMarker   = diva::string ("ed_sp_02");
		}
		args->layerName = diva::appendTheme (args->layerName);
		return originalPlayAetLayerH (args, id);
	}
	return originalPlayAetLayerH (args, id);
}

HOOK (void, LoadAetFrameH, 0x1402CA590, void *data, i32 aetSceneId, const char *layerName, AetAction action, i32 layer, char *a6, float frame) {
	if (layerName == 0) return originalLoadAetFrameH (data, aetSceneId, layerName, action, layer, a6, frame);
	for (auto str : themeStrings) {
		if (strcmp (str.c_str (), layerName) == 0) {
			const char *theme = diva::appendTheme (layerName);
			return originalLoadAetFrameH (data, aetSceneId, theme, action, layer, a6, frame);
		}
	}
	return originalLoadAetFrameH (data, aetSceneId, layerName, action, layer, a6, frame);
}

HOOK (void, OpenNpCommerce, 0x1401DEA30) { ShellExecuteA (NULL, "open", "https://gamebanana.com/games/16522", NULL, NULL, SW_SHOWNORMAL); }
HOOK (void, OpenCredits, 0x1401DEA00) { ShellExecuteA (NULL, "open", "https://gamebanana.com/games/16522", NULL, NULL, SW_SHOWNORMAL); }

extern "C" {

FUNCTION_PTR (float, GetLayerFrame, 0x1402CA120, i32 id, char *layer_name);
FUNCTION_PTR (diva::string *, AppendLayerSuffix, 0x14022D070, void *a1, diva::string *base_layer_name);
HOOK (void, CmnMenuTouchCheck, 0x14022C590);

__declspec (dllexport) void init () {
	freopen ("CONOUT$", "w", stdout);

	auto file   = fopen ("config.toml", "r");
	auto config = toml_parse_file (file, NULL, 0);
	fclose (file);
	if (!config) {
		theme = 0;
	} else {
		auto data = toml_int_in (config, "theme");
		if (!data.ok) theme = 0;
		else theme = data.u.i;
	}

	INSTALL_HOOK (ChangeSubGameState);
	INSTALL_HOOK (GetFtTheme);
	INSTALL_HOOK (PlayAetLayerH);
	INSTALL_HOOK (LoadAetFrameH);
	INSTALL_HOOK (OpenNpCommerce);
	INSTALL_HOOK (OpenCredits);
	INSTALL_HOOK (CmnMenuTouchCheck);

	// Turn on FT mode 1.00 Samyuu, 1.03 BroGamer
	WRITE_MEMORY (0x1414AB9E3, u8, 1);

	// Stop returning to ADV from main menu
	WRITE_NOP (0x1401B2ADA, 36);

	// Fix SFX select priority
	WRITE_MEMORY (0x140699000, u8, 0x0A);

	// Fix song select sort options
	WRITE_MEMORY (0x140BE9520, u64, 0x140C85CB0, 0x140C85CE8);

	// Reduce number of loading screens to 3
	WRITE_MEMORY (0x1406542B4, u8, 0x02);

	// Properly load rights_bg02
	WRITE_NULL (0x15E99F0A8, 1);

	// Use Left/Right on FX select
	WRITE_MEMORY (0x14069997F, Button, Button::LEFT);
	WRITE_MEMORY (0x1406999A6, Button, Button::RIGHT);

	// Load survival_attention from aet_nswgam_game in the pause menu
	WRITE_MEMORY (0x140657012, i32, 0x51C);

	WRITE_MEMORY (0x14066451D, AetAction, AetAction::IN_LOOP);
	WRITE_MEMORY (0x140654505, AetAction, AetAction::IN_LOOP);
	WRITE_MEMORY (0x1401A9CF8, AetAction, AetAction::IN_LOOP);

	// Autosave messages
	WRITE_MEMORY (0x140663883, AetAction, AetAction::IN_LOOP);
	WRITE_MEMORY (0x140666383, AetAction, AetAction::IN_LOOP);
	WRITE_MEMORY (0x140661E13, AetAction, AetAction::IN_LOOP);
	WRITE_MEMORY (0x140661813, AetAction, AetAction::IN_LOOP);

	diva::init ();
	exitMenu::init ();
	pvSel::init ();
	gallery::init ();
	options::init ();
	pause::init ();
	customize::init ();
	result::init ();
	decoration::init ();
	pvWatch::init ();
	pvGame::init ();
	genericDialog::init ();
	commonUi::init ();
	commonMenu::init ();
}

SIG_SCAN_STRING (sigPvDbSwitch, "pv_db_switch.txt");
SIG_SCAN_STRING (sigMenuTxt1, "menu_txt_01");
SIG_SCAN_STRING (sigMenuTxt2, "menu_txt_02");
SIG_SCAN_STRING (sigMenuTxt3, "menu_txt_03");
SIG_SCAN_STRING (sigMenuTxt4, "menu_txt_04");
SIG_SCAN_STRING (sigMenuTxt5, "menu_txt_05");
SIG_SCAN_STRING (sigMenuTxtBase, "menu_txt_base");
SIG_SCAN_STRING (sigCmnWinHelpG, "cmn_win_help_g_inout");
SIG_SCAN_STRING (sigCmnWinHelpP, "cmn_win_help_p_inout");
SIG_SCAN_STRING (sigCmnWinHelpR, "cmn_win_help_r_inout");
SIG_SCAN_STRING (sigCmnWinHelpY, "cmn_win_help_y_inout");
SIG_SCAN_STRING (sigCmnWinG, "cmn_win_g");
SIG_SCAN_STRING (sigCmnWinP, "cmn_win_p");

__declspec (dllexport) void preInit () {
	WRITE_NULL (sigPvDbSwitch (), 1);
	WRITE_NULL (sigMenuTxt1 (), 1);
	WRITE_NULL (sigMenuTxt2 (), 1);
	WRITE_NULL (sigMenuTxt3 (), 1);
	WRITE_NULL (sigMenuTxt4 (), 1);
	WRITE_NULL (sigMenuTxt5 (), 1);
	WRITE_NULL (sigMenuTxtBase (), 1);

	WRITE_MEMORY (sigCmnWinHelpG (), char, "cmn_win_help");
	WRITE_MEMORY (sigCmnWinHelpP (), char, "cmn_win_help");
	WRITE_MEMORY (sigCmnWinHelpR (), char, "cmn_win_help");
	WRITE_MEMORY (sigCmnWinHelpY (), char, "cmn_win_help");

	WRITE_MEMORY (sigCmnWinG (), char, "cmn_win_m");
	WRITE_MEMORY (sigCmnWinP (), char, "cmn_win_m");
}
}
