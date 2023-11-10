#include "diva.h"

namespace pvSel {
typedef enum Style : i32 {
	STYLE_FT   = 0,
	STYLE_MM   = 1,
	STYLE_NONE = 2,
} Style;

using namespace diva;

bool loaded     = false;
bool hasClicked = false;
i32 pvId        = 0;
char buttonName[16];
char selectorImgName[64];
i32 selectorId    = 0;
i32 selectorImgId = 0;
i32 keyHelpId     = 0;
InputType lastInputType;
Vec4 touchArea;
Vec3 keyHelpLoc;
Vec3 txtLoc;

bool optSelectorInited = false;
i32 optSelectorId      = 0;
Vec4 topButton;
Vec4 topButtonLeft;
Vec4 topButtonRight;
Vec4 middleButton;
Vec4 middleButtonLeft;
Vec4 middleButtonRight;
Vec4 bottomButton;
Vec4 bottomButtonLeft;
Vec4 bottomButtonRight;
Vec4 startButton;
Vec4 startButtonLeft;
Vec4 startButtonRight;

void *nswgamPVSelTask = malloc (0x27540);

void
updateStyleAets (Style newStyle) {
	int i;
	switch (newStyle) {
	case STYLE_FT: i = 2; break;
	case STYLE_MM: i = 1; break;
	case STYLE_NONE: i = 3; break;
	}
	sprintf (selectorImgName, "nswgam_songselector_visual_settings_%02d.pic", i);

	AetLayerArgs selectorImgData ("AET_PS4_MENU_MAIN", selectorImgName, 0x12, AetAction::NONE);
	selectorImgData.setPosition (txtLoc);
	selectorImgData.play (&selectorImgId);
}

void
updateButtonPrompt (InputType input) {
	sprintf (buttonName, "visual_key_%02d", (u8)input);

	AetLayerArgs keyHelpData ("AET_PS4_MENU_MAIN", buttonName, 0x13, AetAction::NONE);
	keyHelpData.setPosition (keyHelpLoc);
	keyHelpData.play (&keyHelpId);
}

void
initStyle (Style style, InputType input) {
	AetLayerArgs selectorData ("AET_PS4_MENU_MAIN", "visual_settings", 0x12, AetAction::NONE);
	selectorData.play (&selectorId);

	AetComposition compositionData;
	GetComposition (&compositionData, selectorId);

	if (auto buttonPlaceholderData = compositionData.find (string ("key_help_lv_tab_01"))) keyHelpLoc = buttonPlaceholderData.value ()->position;
	if (auto textPlaceholderData = compositionData.find (string ("visual_settings_txt"))) txtLoc = textPlaceholderData.value ()->position;
	if (auto buttonTouchAreaData = compositionData.find (string ("p_visual_settings_touch"))) touchArea = getPlaceholderRect (**buttonTouchAreaData);

	updateStyleAets (style);
	updateButtonPrompt (input);
}

Style
getStyle (i32 currentStyle, bool isMovie) {
	if (isMovie) return STYLE_NONE;
	else return (Style)currentStyle;
}

void
initOptionsSelectTouch () {
	AetLayerArgs optSelectorData ("AET_PS4_MENU_MAIN", "conf_set_touch", 0, AetAction::NONE);
	optSelectorData.play (&optSelectorId);
	AetComposition compositionData;
	GetComposition (&compositionData, optSelectorId);

	if (auto layer = compositionData.find ("p_conf_set_base01_touch_c")) topButton = getPlaceholderRect (*layer.value ());
	if (auto layer = compositionData.find ("p_conf_set_arrow_l01_touch_c")) topButtonLeft = getPlaceholderRect (*layer.value ());
	if (auto layer = compositionData.find ("p_conf_set_arrow_r01_touch_c")) topButtonRight = getPlaceholderRect (*layer.value ());
	if (auto layer = compositionData.find ("p_conf_set_base02_touch_c")) middleButton = getPlaceholderRect (*layer.value ());
	if (auto layer = compositionData.find ("p_conf_set_arrow_l02_touch_c")) middleButtonLeft = getPlaceholderRect (*layer.value ());
	if (auto layer = compositionData.find ("p_conf_set_arrow_r02_touch_c")) middleButtonRight = getPlaceholderRect (*layer.value ());
	if (auto layer = compositionData.find ("p_conf_set_base03_touch_c")) bottomButton = getPlaceholderRect (*layer.value ());
	if (auto layer = compositionData.find ("p_conf_set_arrow_l03_touch_c")) bottomButtonLeft = getPlaceholderRect (*layer.value ());
	if (auto layer = compositionData.find ("p_conf_set_arrow_r03_touch_c")) bottomButtonRight = getPlaceholderRect (*layer.value ());
	if (auto layer = compositionData.find ("p_conf_start_btn_touch_c")) startButton = getPlaceholderRect (*layer.value ());
	if (auto layer = compositionData.find ("p_conf_start_arrow_l_touch_c")) startButtonLeft = getPlaceholderRect (*layer.value ());
	if (auto layer = compositionData.find ("p_conf_start_arrow_r_touch_c")) startButtonRight = getPlaceholderRect (*layer.value ());

	StopAet (&optSelectorId);
}

FUNCTION_PTR (void, PlayButtonOut, 0x14020c010, u64, u8);
FUNCTION_PTR (void, UpdateButtons, 0x14020b3b0, u64, u8, u8, u8, u8);
FUNCTION_PTR (void, UpdateSubMenu, 0x140209460, u64, i32, u64, i32, i32, u8, u8, u64, u64, u8, u8);
void
updateSelectedButton (u64 This, i32 selectedButton) {
	*(i32 *)(This + 0x78) = selectedButton;
	PlaySoundEffect ("se_ft_sys_select_01", 1.0);
	UpdateButtons (This + 0x78, 0, 0, 0, 0);
}

void
optionsSelectTouch (u64 This) {
	if (!optSelectorInited) {
		initOptionsSelectTouch ();
		optSelectorInited = true;
	}

	void *inputState = diva::GetInputState (0);
	Vec2 clickedPos  = getClickedPos (inputState);
	if (hasClicked) return;
	if (clickedPos.x <= 0) {
		hasClicked = false;
		return;
	}
	hasClicked = true;

	i32 selectedButton   = *(i32 *)(This + 0x78);
	i32 subMenu          = *(i32 *)(This + 0x9C);
	bool extraVocals     = *(bool *)(This + 0xB4);
	i32 extraVocalsCount = 0;
	if (extraVocals) extraVocalsCount = (*(u64 *)(This + 0x480) - *(u64 *)(This + 0x478)) / 0x68;
	bool extraStage = *(bool *)(This + 0xB5);
	bool success    = *(bool *)(This + 0xB6);

	bool topButtonEnabled    = subMenu != 1 || success;
	bool middleButtonEnabled = subMenu != 1 || extraVocals;
	bool bottomButtonEnabled = (subMenu == 1 && extraStage) || (subMenu != 1 && (extraVocals || extraStage));

	if (startButton.contains (clickedPos) && selectedButton != 0) updateSelectedButton (This, 0);
	else if (topButton.contains (clickedPos) && selectedButton != 1 && topButtonEnabled) updateSelectedButton (This, 1);
	else if (middleButton.contains (clickedPos) && selectedButton != 2 && middleButtonEnabled) updateSelectedButton (This, 2);
	else if (bottomButton.contains (clickedPos) && selectedButton != 3 && bottomButtonEnabled) updateSelectedButton (This, 3);

	if (subMenu == 0 || subMenu == 2) {
		if (selectedButton == 1) {
			if (subMenu == 0) {
				if (topButtonLeft.contains (clickedPos) || topButtonRight.contains (clickedPos)) {
					*(bool *)(This + 0xA0) = !*(bool *)(This + 0xA0);
					PlaySoundEffect ("se_ft_music_selector_select_01", 1.0);
					UpdateButtons (This + 0x78, 0, 0, 0, 0);
				}
			} else if (subMenu == 2) {
				float maxTime = *(float *)(This + 0x36A3C) - 30.0;
				if (topButtonLeft.contains (clickedPos)) {
					i32 startTime = *(i32 *)(This + 0xB0) - 10;
					if (startTime < 0) startTime = std::floor ((float)maxTime / 10) * 10;
					*(i32 *)(This + +0xB0) = startTime;
					PlaySoundEffect ("se_ft_music_selector_select_01", 1.0);
				} else if (topButtonRight.contains (clickedPos)) {
					i32 startTime = *(i32 *)(This + 0xB0) + 10;
					if (startTime > maxTime) startTime = 0;
					*(i32 *)(This + +0xB0) = startTime;
					PlaySoundEffect ("se_ft_music_selector_select_01", 1.0);
				}
			}
		} else if (selectedButton == 2) {
			if (middleButtonLeft.contains (clickedPos)) {
				i32 *modifier = (i32 *)(This + 0xA4);
				if (*modifier == 0) *modifier = 3;
				else *modifier = *modifier - 1;
				PlaySoundEffect ("se_ft_music_selector_select_01", 1.0);
			} else if (middleButtonRight.contains (clickedPos)) {
				i32 *modifier = (i32 *)(This + 0xA4);
				if (*modifier == 3) *modifier = 0;
				else *modifier = *modifier + 1;
				PlaySoundEffect ("se_ft_music_selector_select_01", 1.0);
			}
		} else if (selectedButton == 3) {
			if (extraVocals) {
				if (bottomButtonLeft.contains (clickedPos)) {
					*(i32 *)(This + 0xA8) = *(i32 *)(This + 0xA8) - 1;
					if (*(i32 *)(This + 0xA8) < 0) *(i32 *)(This + 0xA8) = extraVocalsCount - 1;
					PlaySoundEffect ("se_ft_music_selector_select_01", 1.0);
				} else if (bottomButtonRight.contains (clickedPos)) {
					*(i32 *)(This + 0xA8) = *(i32 *)(This + 0xA8) + 1;
					if (*(i32 *)(This + 0xA8) >= extraVocalsCount) *(i32 *)(This + 0xA8) = 0;
					PlaySoundEffect ("se_ft_music_selector_select_01", 1.0);
				}
			} else if (bottomButtonLeft.contains (clickedPos) || bottomButtonRight.contains (clickedPos)) {
				*(bool *)(This + 0xAC) = !*(bool *)(This + 0xAC);
				PlaySoundEffect ("se_ft_music_selector_select_01", 1.0);
			}
		}
	} else if (subMenu == 1) {
		if (selectedButton == 1) {
			if (topButtonLeft.contains (clickedPos) || topButtonRight.contains (clickedPos)) {
				*(bool *)(This + 0xAD) = !*(bool *)(This + 0xAD);
				PlaySoundEffect ("se_ft_music_selector_select_01", 1.0);
			}
		} else if (selectedButton == 2) {
			if (bottomButtonLeft.contains (clickedPos)) {
				*(i32 *)(This + 0xA8) = *(i32 *)(This + 0xA8) - 1;
				if (*(i32 *)(This + 0xA8) < 0) *(i32 *)(This + 0xA8) = extraVocalsCount - 1;
				PlaySoundEffect ("se_ft_music_selector_select_01", 1.0);
			} else if (bottomButtonRight.contains (clickedPos)) {
				*(i32 *)(This + 0xA8) = *(i32 *)(This + 0xA8) + 1;
				if (*(i32 *)(This + 0xA8) >= extraVocalsCount) *(i32 *)(This + 0xA8) = 0;
				PlaySoundEffect ("se_ft_music_selector_select_01", 1.0);
			}
		} else if (selectedButton == 3) {
			if (bottomButtonLeft.contains (clickedPos) || bottomButtonRight.contains (clickedPos)) {
				*(bool *)(This + 0xAC) = !*(bool *)(This + 0xAC);
				PlaySoundEffect ("se_ft_music_selector_select_01", 1.0);
			}
		}
	}

	if (startButtonLeft.contains (clickedPos)) {
		if (subMenu == 0) subMenu = 2;
		else subMenu = subMenu - 1;

		*(i32 *)(This + 0x78) = 0;
		UpdateButtons (This + 0x78, 1, 0, 0, 0);
		PlaySoundEffect ("se_ft_music_selector_select_01", 1.0);

		UpdateSubMenu (This + 0x78, subMenu, This + 0xB8, *(i32 *)(This + 0xA8), *(i32 *)(This + 0xB0), *(u8 *)(This + 0xB5), *(u8 *)(This + 0xB6), This + 0xB8, This + 0x490, 0, *(u8 *)(This + 0xB7));
		UpdateButtons (This + 0x78, 1, 0, 0, 0);
	} else if (startButtonRight.contains (clickedPos)) {
		if (subMenu == 2) subMenu = 0;
		else subMenu = subMenu + 1;

		*(i32 *)(This + 0x78) = 0;
		PlaySoundEffect ("se_ft_music_selector_select_01", 1.0);

		UpdateSubMenu (This + 0x78, subMenu, This + 0xB8, *(i32 *)(This + 0xA8), *(i32 *)(This + 0xB0), *(u8 *)(This + 0xB5), *(u8 *)(This + 0xB6), This + 0xB8, This + 0x490, 0, *(u8 *)(This + 0xB7));
		UpdateButtons (This + 0x78, 0, 1, 0, 0);
	} else if (startButton.contains (clickedPos) && selectedButton == 0) {
		PlaySoundEffect ("se_ft_music_selector_enter_01", 1.0);
		*(u8 *)(This + 0x99) = 1;
		*(u8 *)(This + 0x9B) = 1;
		PlayButtonOut (This + 0x78, 1);
	}
}

bool
PVSelLoop (u64 This) {
	// Touch
	if (*(i32 *)(This + 0x68) == 8) optionsSelectTouch (This);

	// Allow swapping of visual style on song select
	// Disable on playlists
	if (*(i32 *)(This + 0x36A08) != 0 || *(u8 *)(0x14CC10480)) return false;

	loaded       = true;
	auto entry   = getPvDbEntry (*(i32 *)(This + 0x36A30));
	bool isMovie = false;
	if (entry) isMovie = isMovieOnly (*entry);

	InputType input  = getInputType ();
	void *inputState = diva::GetInputState (0);
	u64 pvLoadData   = GetPvLoadData ();
	i32 style        = *(i32 *)(pvLoadData + 0x1D08);

	if (input == InputType::UNKNOWN) input = InputType::PLAYSTATION;

	if (style == -1) {
		style = GetCurrentStyle ();
		if (style == -1) style = STYLE_FT;
		initStyle (getStyle (style, isMovie), input);
		lastInputType = input;
	}

	if (input != lastInputType) {
		lastInputType = input;
		updateButtonPrompt (input);
	}

	if (IsButtonTapped (inputState, Button::L3) && !isMovie) {
		PlaySoundEffect ("se_ft_music_selector_select_01", 1.0);
		style = !style;
		updateStyleAets (getStyle (style, isMovie));
	} else if (pvId != *(i32 *)(This + 0x36A30)) {
		style = GetCurrentStyle ();
		updateStyleAets (getStyle (style, isMovie));
		pvId = *(i32 *)(This + 0x36A30);
	}

	Vec2 clickedPos = getClickedPos (inputState);
	if (clickedPos.x > 0 && !hasClicked) {
		hasClicked = true;
		if (touchArea.contains (clickedPos) && !isMovie) {
			style = !style;
			PlaySoundEffect ("se_ft_music_selector_select_01", 1.0);
			updateStyleAets (getStyle (style, isMovie));
		}
	} else if (clickedPos.x == 0) hasClicked = false;

	*(i32 *)(pvLoadData + 0x1D08)           = style;
	*(u8 *)((u64)nswgamPVSelTask + 0x27538) = (u8)style; // Fix Future Tone Customization

	return false;
}

void
hide () {
	if (!loaded) return;

	StopAet (&keyHelpId);
	StopAet (&selectorImgId);
	StopAet (&selectorId);
	loaded = false;
}

bool
PvSelDestroy (u64 This) {
	if (*(i32 *)(This + 0x36A08) != 0) return false;
	hide ();
	return false;
}

bool
PvSelDisplay (u64 This) {
	// Disable on playlist
	if (*(i32 *)(This + 0x36A08) != 0 || *(u8 *)(0x14CC10480)) return false;
	else if (selectorId == 0) return false;

	AetComposition compositionData;
	GetComposition (&compositionData, selectorId);

	if (auto buttonPlaceholderData = compositionData.find (string ("key_help_lv_tab_01"))) keyHelpLoc = buttonPlaceholderData.value ()->position;
	if (auto textPlaceholderData = compositionData.find (string ("visual_settings_txt"))) txtLoc = textPlaceholderData.value ()->position;
	if (auto buttonTouchAreaData = compositionData.find (string ("p_visual_settings_touch"))) touchArea = getPlaceholderRect (**buttonTouchAreaData);

	AetLayerArgs keyHelpData ("AET_PS4_MENU_MAIN", buttonName, 0x13, AetAction::NONE);
	keyHelpData.setPosition (keyHelpLoc);
	keyHelpData.play (&keyHelpId);

	AetLayerArgs selectorImgData ("AET_PS4_MENU_MAIN", selectorImgName, 0x12, AetAction::NONE);
	selectorImgData.setPosition (txtLoc);
	selectorImgData.play (&selectorImgId);

	return false;
}

void
init () {
	taskAddition addition;
	addition.loop    = PVSelLoop;
	addition.destroy = PvSelDestroy;
	addition.display = PvSelDisplay;
	addTaskAddition ("PVsel", addition);

	WRITE_MEMORY (0x14CC5EF18, void *, nswgamPVSelTask);
}
} // namespace pvSel
