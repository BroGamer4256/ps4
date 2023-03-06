#include "diva.h"
#include "menus.h"

namespace pvSel {
typedef enum Style : i32 {
	STYLE_FT   = 0,
	STYLE_MM   = 1,
	STYLE_NONE = 2,
} Style;

bool loaded           = false;
bool hasClicked       = false;
i32 pvId              = 0;
void *selectorData    = calloc (1, 0x1024);
void *selectorImgData = calloc (1, 0x1024);
void *keyHelpData     = calloc (1, 0x1024);
char *buttonName      = (char *)calloc (16, sizeof (char));
char *selectorImgName = (char *)calloc (64, sizeof (char));
i32 selectorId        = 0;
i32 selectorImgId     = 0;
i32 keyHelpId         = 0;
Map<String, void *> compositionData;
InputType lastInputType;
Vec4 touchArea;
Vec3 keyHelpLoc = createVec3 (0, 0, 0);
Vec3 txtLoc     = createVec3 (0, 0, 0);

bool optSelectorInited = false;
void *optSelectorData  = calloc (1, 0x1024);
i32 optSelectorId      = 0;
Map<String, void *> optSelectorCompositionData;
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

void
updateStyleAets (Style newStyle) {
	selectorImgName[0] = 0;
	strcpy (selectorImgName, "nswgam_songselector_visual_settings_0");
	char c;
	switch (newStyle) {
	case STYLE_FT: c = '2'; break;
	case STYLE_MM: c = '1'; break;
	case STYLE_NONE: c = '3'; break;
	}
	strncat (selectorImgName, &c, 1);
	strcat (selectorImgName, ".pic");
	CreateAetLayerData (selectorImgData, 0x4F8, selectorImgName, 0x12, AETACTION_NONE);
	ApplyLocation (selectorImgData, &txtLoc);
	selectorImgId = PlayAetLayer (selectorImgData, selectorImgId);
}

void
updateButtonPrompt (InputType input) {
	buttonName[0] = 0;
	strcpy (buttonName, "visual_key_0");
	const char c = (u8)input | 48;
	strncat (buttonName, &c, 1);
	strcat (buttonName, "");
	CreateAetLayerData (keyHelpData, 0x4F8, buttonName, 0x13, AETACTION_NONE);
	ApplyLocation (keyHelpData, &keyHelpLoc);
	keyHelpId = PlayAetLayer (keyHelpData, keyHelpId);
}

void
initStyle (Style style, InputType input) {
	CreateAetLayerData (selectorData, 0x4F8, "visual_settings", 0x12, AETACTION_IN_LOOP);
	selectorId = PlayAetLayer (selectorData, selectorId);

	initCompositionData (&compositionData);
	GetComposition (&compositionData, selectorId);

	float *buttonPlaceholderData = GetCompositionLayer (&compositionData, "key_help_lv_tab_01");
	if (buttonPlaceholderData) keyHelpLoc = createVec3 (buttonPlaceholderData[16], buttonPlaceholderData[17], buttonPlaceholderData[18]);
	float *textPlaceholderData = GetCompositionLayer (&compositionData, "visual_settings_txt");
	if (textPlaceholderData) txtLoc = createVec3 (textPlaceholderData[16], textPlaceholderData[17], textPlaceholderData[18]);
	float *buttonTouchAreaData = GetCompositionLayer (&compositionData, "p_visual_settings_touch");
	if (buttonTouchAreaData) touchArea = getPlaceholderRect (buttonTouchAreaData, false);

	updateStyleAets (style);
	updateButtonPrompt (input);
}

void
updateLocs () {
	initCompositionData (&compositionData);
	GetComposition (&compositionData, selectorId);

	float *buttonPlaceholderData = GetCompositionLayer (&compositionData, "key_help_lv_tab_01");
	if (buttonPlaceholderData) keyHelpLoc = createVec3 (buttonPlaceholderData[16], buttonPlaceholderData[17], buttonPlaceholderData[18]);
	float *textPlaceholderData = GetCompositionLayer (&compositionData, "visual_settings_txt");
	if (textPlaceholderData) txtLoc = createVec3 (textPlaceholderData[16], textPlaceholderData[17], textPlaceholderData[18]);
	float *buttonTouchAreaData = GetCompositionLayer (&compositionData, "p_visual_settings_touch");
	if (buttonTouchAreaData) touchArea = getPlaceholderRect (buttonTouchAreaData, false);

	ApplyLocation (keyHelpData, &keyHelpLoc);
	ApplyLocation (selectorImgData, &txtLoc);

	keyHelpId     = PlayAetLayer (keyHelpData, keyHelpId);
	selectorImgId = PlayAetLayer (selectorImgData, selectorImgId);
}

Style
getStyle (i32 currentStyle, bool isMovie) {
	if (isMovie) return STYLE_NONE;
	else return (Style)currentStyle;
}

FUNCTION_PTR (void, __stdcall, FillRectangle, 0x1402c5520, void *, Vec4 *);
FUNCTION_PTR (void, __stdcall, Test, 0x1402c53d0, void *);

void
initOptionsSelectTouch () {
	CreateAetLayerData (optSelectorData, 0x4F8, "conf_set_touch", 0, AETACTION_NONE);
	optSelectorId = PlayAetLayer (optSelectorData, 0);
	initCompositionData (&optSelectorCompositionData);
	GetComposition (&optSelectorCompositionData, optSelectorId);

	if (auto touch = getTouchArea (optSelectorCompositionData, "p_conf_set_base01_touch_c", true)) topButton = *touch;
	if (auto touch = getTouchArea (optSelectorCompositionData, "p_conf_set_arrow_l01_touch_c", true)) topButtonLeft = *touch;
	if (auto touch = getTouchArea (optSelectorCompositionData, "p_conf_set_arrow_r01_touch_c", true)) topButtonRight = *touch;
	if (auto touch = getTouchArea (optSelectorCompositionData, "p_conf_set_base02_touch_c", true)) middleButton = *touch;
	if (auto touch = getTouchArea (optSelectorCompositionData, "p_conf_set_arrow_l02_touch_c", true)) middleButtonLeft = *touch;
	if (auto touch = getTouchArea (optSelectorCompositionData, "p_conf_set_arrow_r02_touch_c", true)) middleButtonRight = *touch;
	if (auto touch = getTouchArea (optSelectorCompositionData, "p_conf_set_base03_touch_c", true)) bottomButton = *touch;
	if (auto touch = getTouchArea (optSelectorCompositionData, "p_conf_set_arrow_l03_touch_c", true)) bottomButtonLeft = *touch;
	if (auto touch = getTouchArea (optSelectorCompositionData, "p_conf_set_arrow_r03_touch_c", true)) bottomButtonRight = *touch;
	if (auto touch = getTouchArea (optSelectorCompositionData, "p_conf_start_btn_touch_c", true)) startButton = *touch;
	if (auto touch = getTouchArea (optSelectorCompositionData, "p_conf_start_arrow_l_touch_c", true)) startButtonLeft = *touch;
	if (auto touch = getTouchArea (optSelectorCompositionData, "p_conf_start_arrow_r_touch_c", true)) startButtonRight = *touch;

	StopAet (&optSelectorId);
}

FUNCTION_PTR (void, __stdcall, PlayButtonOut, 0x14020c010, u64, u8);
FUNCTION_PTR (void, __stdcall, UpdateButtons, 0x14020b3b0, u64, u8, u8, u8, u8);
FUNCTION_PTR (void, __stdcall, UpdateSubMenu, 0x140209460, u64, i32, u64, i32, i32, u8, u8, u64, u64, u8, u8);
void
updateSelectedButton (u64 This, i32 selectedButton) {
	*(i32 *)(This + 0x78) = selectedButton;
	PlaySoundEffect ("se_ft_sys_select_01", 1.0);
	UpdateButtons (This + 0x78, 0, 0, 0, 0);
}

void
subtractModifier (u64 This) {
	i32 *modifier = (i32 *)(This + 0xA4);
	if (*modifier == 0) *modifier = 3;
	else *modifier = *modifier - 1;
	PlaySoundEffect ("se_ft_music_selector_select_01", 1.0);
}

void
incrementModifier (u64 This) {
	i32 *modifier = (i32 *)(This + 0xA4);
	if (*modifier == 3) *modifier = 0;
	else *modifier = *modifier + 1;
	PlaySoundEffect ("se_ft_music_selector_select_01", 1.0);
}

void
optionsSelectTouch (u64 This) {
	if (!optSelectorInited) {
		initOptionsSelectTouch ();
		optSelectorInited = true;
	}

	void *inputState = DivaGetInputState (0);
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

	if (vec4ContainsVec2 (startButton, clickedPos) && selectedButton != 0) updateSelectedButton (This, 0);
	else if (vec4ContainsVec2 (topButton, clickedPos) && selectedButton != 1 && topButtonEnabled) updateSelectedButton (This, 1);
	else if (vec4ContainsVec2 (middleButton, clickedPos) && selectedButton != 2 && middleButtonEnabled) updateSelectedButton (This, 2);
	else if (vec4ContainsVec2 (bottomButton, clickedPos) && selectedButton != 3 && bottomButtonEnabled) updateSelectedButton (This, 3);

	if (subMenu == 0 || subMenu == 2) {
		if (selectedButton == 1) {
			if (subMenu == 0) {
				if (vec4ContainsVec2 (topButtonLeft, clickedPos) || vec4ContainsVec2 (topButtonRight, clickedPos)) {
					*(bool *)(This + 0xA0) = !*(bool *)(This + 0xA0);
					PlaySoundEffect ("se_ft_music_selector_select_01", 1.0);
					UpdateButtons (This + 0x78, 0, 0, 0, 0);
				}
			} else if (subMenu == 2) {
				float max = *(float *)(This + 0x36A3C) - 30.0;
				if (vec4ContainsVec2 (topButtonLeft, clickedPos)) {
					i32 start = *(i32 *)(This + 0xB0) - 10;
					if (start < 0) start = max;
					*(i32 *)(This + +0xB0) = start;
					PlaySoundEffect ("se_ft_music_selector_select_01", 1.0);
				} else if (vec4ContainsVec2 (topButtonRight, clickedPos)) {
					i32 start = *(i32 *)(This + 0xB0) + 10;
					if (start > max) start = 0;
					*(i32 *)(This + +0xB0) = start;
					PlaySoundEffect ("se_ft_music_selector_select_01", 1.0);
				}
			}
		} else if (selectedButton == 2) {
			if (vec4ContainsVec2 (middleButtonLeft, clickedPos)) subtractModifier (This);
			if (vec4ContainsVec2 (middleButtonRight, clickedPos)) incrementModifier (This);
		} else if (selectedButton == 3) {
			if (extraVocals) {
				if (vec4ContainsVec2 (bottomButtonLeft, clickedPos)) {
					*(i32 *)(This + 0xA8) = *(i32 *)(This + 0xA8) - 1;
					if (*(i32 *)(This + 0xA8) < 0) *(i32 *)(This + 0xA8) = extraVocalsCount - 1;
					PlaySoundEffect ("se_ft_music_selector_select_01", 1.0);
				} else if (vec4ContainsVec2 (bottomButtonRight, clickedPos)) {
					*(i32 *)(This + 0xA8) = *(i32 *)(This + 0xA8) + 1;
					if (*(i32 *)(This + 0xA8) >= extraVocalsCount) *(i32 *)(This + 0xA8) = 0;
					PlaySoundEffect ("se_ft_music_selector_select_01", 1.0);
				}
			} else if (vec4ContainsVec2 (bottomButtonLeft, clickedPos) || vec4ContainsVec2 (bottomButtonRight, clickedPos)) {
				*(bool *)(This + 0xAC) = !*(bool *)(This + 0xAC);
				PlaySoundEffect ("se_ft_music_selector_select_01", 1.0);
			}
		}
	} else if (subMenu == 1) {
		if (selectedButton == 1) {
			if (vec4ContainsVec2 (topButtonLeft, clickedPos) || vec4ContainsVec2 (topButtonRight, clickedPos)) {
				*(bool *)(This + 0xAD) = !*(bool *)(This + 0xAD);
				PlaySoundEffect ("se_ft_music_selector_select_01", 1.0);
			}
		} else if (selectedButton == 2) {
			if (vec4ContainsVec2 (bottomButtonLeft, clickedPos)) {
				*(i32 *)(This + 0xA8) = *(i32 *)(This + 0xA8) - 1;
				if (*(i32 *)(This + 0xA8) < 0) *(i32 *)(This + 0xA8) = extraVocalsCount - 1;
				PlaySoundEffect ("se_ft_music_selector_select_01", 1.0);
			} else if (vec4ContainsVec2 (bottomButtonRight, clickedPos)) {
				*(i32 *)(This + 0xA8) = *(i32 *)(This + 0xA8) + 1;
				if (*(i32 *)(This + 0xA8) >= extraVocalsCount) *(i32 *)(This + 0xA8) = 0;
				PlaySoundEffect ("se_ft_music_selector_select_01", 1.0);
			}
		} else if (selectedButton == 3) {
			if (vec4ContainsVec2 (bottomButtonLeft, clickedPos) || vec4ContainsVec2 (bottomButtonRight, clickedPos)) {
				*(bool *)(This + 0xAC) = !*(bool *)(This + 0xAC);
				PlaySoundEffect ("se_ft_music_selector_select_01", 1.0);
			}
		}
	}

	if (vec4ContainsVec2 (startButtonLeft, clickedPos)) {
		if (subMenu == 0) subMenu = 2;
		else subMenu = subMenu - 1;

		*(i32 *)(This + 0x78) = 0;
		UpdateButtons (This + 0x78, 1, 0, 0, 0);
		PlaySoundEffect ("se_ft_music_selector_select_01", 1.0);

		UpdateSubMenu (This + 0x78, subMenu, This + 0x78 + 0x40, *(i32 *)(This + 0x78 + 0x30), *(i32 *)(This + 0x78 + 0x38), *(u8 *)(This + 0x78 + 0x3D), *(u8 *)(This + 0x78 + 0x3E),
		               This + 0x78 + 0x40, This + 0x78 + 0x418, 0, *(u8 *)(This + 0x78 + 0x3F));
		UpdateButtons (This + 0x78, 1, 0, 0, 0);
	} else if (vec4ContainsVec2 (startButtonRight, clickedPos)) {
		if (subMenu == 2) subMenu = 0;
		else subMenu = subMenu + 1;

		*(i32 *)(This + 0x78) = 0;
		PlaySoundEffect ("se_ft_music_selector_select_01", 1.0);

		UpdateSubMenu (This + 0x78, subMenu, This + 0x78 + 0x40, *(i32 *)(This + 0x78 + 0x30), *(i32 *)(This + 0x78 + 0x38), *(u8 *)(This + 0x78 + 0x3D), *(u8 *)(This + 0x78 + 0x3E),
		               This + 0x78 + 0x40, This + 0x78 + 0x418, 0, *(u8 *)(This + 0x78 + 0x3F));
		UpdateButtons (This + 0x78, 0, 1, 0, 0);
	} else if (vec4ContainsVec2 (startButton, clickedPos) && selectedButton == 0) {
		PlaySoundEffect ("se_ft_music_selector_enter_01", 1.0);
		*(u8 *)(This + 0x99) = 1;
		*(u8 *)(This + 0x9B) = 1;
		PlayButtonOut (This + 0x78, 1);
	}
}

HOOK (bool, __thiscall, PVSelLoop, 0x1402033C0, u64 This) {
	// Touch for the box
	if (*(i32 *)(This + 0x68) == 8) optionsSelectTouch (This);

	// Allow swapping of visual style on song select
	// Disable on playlists
	if (*(i32 *)(This + 0x36A08) != 0 || *(u8 *)(0x14CC10480)) return originalPVSelLoop (This);

	loaded           = true;
	bool isMovie     = isMovieOnly (getPvDbEntry (*(i32 *)(This + 0x36A30)));
	InputType input  = getInputType ();
	void *inputState = DivaGetInputState (0);
	u64 pvLoadData   = GetPvLoadData ();
	i32 style        = *(i32 *)(pvLoadData + 0x1D08);

	if (input == INPUTTYPE_UNKNOWN) input = INPUTTYPE_PLAYSTATION;

	if (style == -1) {
		style = GetCurrentStyle ();
		initStyle (getStyle (style, isMovie), input);
		lastInputType = input;
	}

	if (input != lastInputType) {
		lastInputType = input;
		updateButtonPrompt (input);
	}

	if (IsButtonTapped (inputState, BUTTON_L3) && !isMovie) {
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
		if (vec4ContainsVec2 (touchArea, clickedPos) && !isMovie) {
			style = !style;
			PlaySoundEffect ("se_ft_music_selector_select_01", 1.0);
			updateStyleAets (getStyle (style, isMovie));
		}
	} else if (clickedPos.x == 0) hasClicked = false;

	updateLocs ();
	*(i32 *)(pvLoadData + 0x1D08) = style;

	return originalPVSelLoop (This);
}

HOOK (bool, __thiscall, PvSelDestroy, 0x140204DA0, u64 This) {
	if (*(i32 *)(This + 0x36A08) != 0) return originalPvSelDestroy (This);

	hide ();
	loaded = false;

	return originalPvSelDestroy (This);
}

void
init () {
	INSTALL_HOOK (PVSelLoop);
	INSTALL_HOOK (PvSelDestroy);
}

void
hide () {
	if (!loaded) return;
	Vec3 offscreen = createVec3 (-1920, -1080, 0);
	ApplyLocation (selectorImgData, &offscreen);
	ApplyLocation (keyHelpData, &offscreen);
	PlayAetLayer (selectorImgData, selectorImgId);
	PlayAetLayer (keyHelpData, keyHelpId);
}
} // namespace pvSel