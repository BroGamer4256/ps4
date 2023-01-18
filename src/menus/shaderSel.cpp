#include "diva.h"
#include "helpers.h"

namespace shaderSel {
typedef enum Style : i32 {
	STYLE_FT   = 0,
	STYLE_MM   = 1,
	STYLE_NONE = 2,
} Style;

bool hasClicked       = false;
i32 pvId              = 0;
void *selectorData    = calloc (1, 1024);
void *selectorImgData = calloc (1, 1024);
void *keyHelpData     = calloc (1, 1024);
char *buttonName      = (char *)calloc (16, sizeof (char));
char *selectorImgName = (char *)calloc (64, sizeof (char));
i32 selectorId        = 0;
i32 selectorImgId     = 0;
i32 keyHelpId         = 0;
List<void> sublayerData;
InputType lastInputType;
Vec4 touchArea;
Vec3 keyHelpLoc;
Vec3 txtLoc;

void
updateStyleAets (Style newStyle) {
	selectorImgName[0] = 0;
	strcpy (selectorImgName, "nswgam_songselector_visual_settings_0");
	const char *c;
	switch (newStyle) {
	case STYLE_FT: c = "2"; break;
	case STYLE_MM: c = "1"; break;
	case STYLE_NONE: c = "3"; break;
	}
	strcat (selectorImgName, c);
	strcat (selectorImgName, ".pic");
	LoadAet (selectorImgData, 0x4F8, selectorImgName, 0x20, AETACTION_NONE);
	ApplyLocation (selectorImgData, &txtLoc);
	selectorImgId = PlayAet (selectorImgData, selectorImgId);
}

void
updateButtonPrompt (InputType input) {
	buttonName[0] = 0;
	strcpy (buttonName, "visual_key_0");
	char c = (u8)input | 48;
	strncat (buttonName, &c, 1);
	LoadAet (keyHelpData, 0x4F8, buttonName, 0x21, AETACTION_NONE);
	ApplyLocation (keyHelpData, &keyHelpLoc);
	keyHelpId = PlayAet (keyHelpData, keyHelpId);
}

void
initStyle (Style style, InputType input) {
	LoadAet (selectorData, 0x4F8, "visual_settings", 0x20, AETACTION_NONE);
	selectorId = PlayAet (selectorData, selectorId);

	*(void **)sublayerData.empty_element             = sublayerData.empty_element;
	*(void **)((u64)sublayerData.empty_element + 8)  = sublayerData.empty_element;
	*(void **)((u64)sublayerData.empty_element + 16) = sublayerData.empty_element;
	*(u16 *)((u64)sublayerData.empty_element + 24)   = 0x101;
	sublayerData.length                              = 0;
	GetSubLayers (&sublayerData, selectorId);

	float *buttonPlaceholderData = GetSubLayer (&sublayerData, "key_help_lv_tab_01");
	keyHelpLoc                   = createVec3 (buttonPlaceholderData[16], buttonPlaceholderData[17], buttonPlaceholderData[18]);
	float *textPlaceholderData   = GetSubLayer (&sublayerData, "visual_settings_txt");
	txtLoc                       = createVec3 (textPlaceholderData[16], textPlaceholderData[17], textPlaceholderData[18]);
	touchArea                    = getPlaceholderRect (textPlaceholderData);

	updateStyleAets (style);
	updateButtonPrompt (input);
}

Style
getStyle (i32 currentStyle, bool isMovie) {
	if (isMovie) return STYLE_NONE;
	else return (Style)currentStyle;
}

// Allow swapping of visual style on song select
HOOK (bool, __thiscall, PVSelCtrl, 0x1402033B0, u64 This) {
	// Disable on playlists
	if (*(i32 *)(This + 0x36A08) != 0) return originalPVSelCtrl (This);

	bool isMovie     = isMovieOnly (getPvDbEntry (*(i32 *)(This + 0x36A30)));
	InputType input  = getInputType ();
	void *inputState = DivaGetInputState (0);
	u64 data         = GetPvLoadData ();
	i32 style        = *(i32 *)(data + 0x1D08);

	if (style == -1) {
		style = GetCurrentStyle ();
		initStyle (getStyle (style, isMovie), input);
		lastInputType = input;
	}

	if (input != lastInputType) {
		lastInputType = input;
		updateButtonPrompt (input);
	}

	if (IsButtonTapped (inputState, 15)) { // F3
		PlaySoundEffect ("se_ft_music_selector_select_01", 1.0);
		style = !style;
		updateStyleAets (getStyle (style, isMovie));
	} else if (pvId != *(i32 *)(This + 0x36A30)) {
		style = GetCurrentStyle ();
		updateStyleAets (getStyle (style, isMovie));
		pvId = *(i32 *)(This + 0x36A30);
	}

	i32 clickedX = *(i32 *)((u64)inputState + 0x158);
	i32 clickedY = *(i32 *)((u64)inputState + 0x15C);
	if (clickedX > 0 && !hasClicked) {
		hasClicked = true;
		RECT rect;
		GetClientRect (FindWindow ("DIVAMIXP", 0), &rect);
		float x  = clickedX / (float)rect.right * 1920;
		float y  = clickedY / (float)rect.bottom * 1080;
		Vec2 vec = createVec2 (x, y);
		if (vec4ContainsVec2 (touchArea, vec)) {
			style = !style;
			PlaySoundEffect ("se_ft_music_selector_select_01", 1.0);
			updateStyleAets (getStyle (style, isMovie));
		}
	} else if (clickedX == 0) {
		hasClicked = false;
	}

	*(i32 *)(data + 0x1D08) = style;

	return originalPVSelCtrl (This);
}

HOOK (bool, __thiscall, PvSelDestroy, 0x140204D90, u64 This) {
	if (*(i32 *)(This + 0x36A08) != 0) return originalPvSelDestroy (This);

	Vec3 offscreen = createVec3 (-1920, -1080, 0);
	ApplyLocation (selectorImgData, &offscreen);
	ApplyLocation (keyHelpData, &offscreen);
	PlayAet (selectorImgData, selectorImgId);
	PlayAet (keyHelpData, keyHelpId);

	return originalPvSelDestroy (This);
}

void
init () {
	sublayerData.empty_element = calloc (1, 0xB0);
	sublayerData.length        = 0;
	INSTALL_HOOK (PVSelCtrl);
	INSTALL_HOOK (PvSelDestroy);
}
} // namespace shaderSel
