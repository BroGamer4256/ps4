#include "diva.h"
#include "helpers.h"

FUNCTION_PTR (bool, __thiscall, CmnMenuDestroy, 0x1401AAE50, u64 This);
FUNCTION_PTR (void *, __stdcall, DivaGetInputState, 0x1402AC970, i32 a1);
FUNCTION_PTR (bool, __stdcall, IsButtonTapped, 0x1402AB260, void *state, Button button);
FUNCTION_PTR (void *, __stdcall, CreateAetLayerData, 0x14028D560, void *data, i32 aetSceneId, const char *layerName, i32 layer, AetAction action);
FUNCTION_PTR (i32, __stdcall, PlayAetLayer, 0x1402CA220, void *data, i32 id);
FUNCTION_PTR (void, __stdcall, GetComposition, 0x1402CA670, Map<String, void *> *composition, i32 id);
FUNCTION_PTR (float *, __stdcall, GetCompositionLayer, 0x1402CA780, Map<String, void *> *composition, const char *layerName);
FUNCTION_PTR (void, __stdcall, ApplyLocation, 0x14065FCC0, void *data, Vec3 *locationData);
FUNCTION_PTR (void, __stdcall, PlaySoundEffect, 0x1405AA540, const char *name, float volume);
FUNCTION_PTR (u64, __stdcall, GetPvLoadData, 0x14040B2A0);
FUNCTION_PTR (i32, __stdcall, GetCurrentStyle, 0x1401D64F0);
FUNCTION_PTR (InputType, __stdcall, NormalizeInputType, 0x1402ACAA0, i32 inputType);
FUNCTION_PTR (String *, __stdcall, StringInit, 0x14014BA50, String *to, const char *from, u64 len);
FUNCTION_PTR (void, __stdcall, FreeSubLayers, 0x1401AC240, Map<String, void *> *sublayerData, Map<String, void *> *sublayerData2, void *first_element);
FUNCTION_PTR (void, __stdcall, StopAet, 0x1402ca330, i32 *id);

auto pvs       = (List<i32> *)0x141753808;
auto pvSprites = (Map<i32, PvSpriteIds> *)0x14CBBACC0;

extern i32 theme;
void
appendThemeInPlace (char *name) {
	switch (theme) {
	case 1: strcat (name, "_f"); break;
	case 2: strcat (name, "_t"); break;
	default: strcat (name, "_ft"); break;
	}
}

char *
appendTheme (const char *name) {
	char *themeStr = (char *)calloc (strlen (name) + 5, sizeof (char));
	strcpy (themeStr, name);
	appendThemeInPlace (themeStr);

	return themeStr;
}

void
appendStringInPlace (String *str, const char *append) {
	i32 lengthNeeded = str->length + strlen (append) + 1;
	if (lengthNeeded > str->capacity) {
		char *temp = (char *)calloc (lengthNeeded, sizeof (char));
		if (str->capacity > 15) {
			strcpy (temp, str->ptr);
			free (str->ptr);
		} else {
			strcpy (temp, str->data);
		}
		strcat (temp, append);
		str->ptr      = temp;
		str->capacity = lengthNeeded;
		str->length   = lengthNeeded;
	} else {
		strcat (str->data, append);
		str->length = lengthNeeded;
	}
}

void
appendThemeInPlaceString (String *name) {
	switch (theme) {
	case 1: appendStringInPlace (name, "_f"); break;
	case 2: appendStringInPlace (name, "_t"); break;
	default: appendStringInPlace (name, "_ft"); break;
	}
}

InputType
getInputType () {
	return NormalizeInputType (*(i32 *)((u64)DivaGetInputState (0) + 0x2E8));
}

// Will return false for any songs without an ex chart
bool
isMovieOnly (u64 entry) {
	if (entry == 0 || *(u64 *)(entry + 0xF8) == 0) return false;
	return *(bool *)(*(u64 *)(entry + 0xF8) + 0x4AC);
}

u64
getPvDbEntry (i32 id) {
	if (id < 0) return 0;
	for (ListElement<i32> *currentElement = pvs->empty_element->next; currentElement->current > 0; currentElement = currentElement->next) {
		if (currentElement->current != id) continue;
		return (u64)&currentElement->current;
	}
	return 0;
}

// Uses anchor X/Y + position X/Y to create a rect, primarily useful for touch interactions
Vec4
getPlaceholderRect (float *placeholderData) {
	float xDiff   = placeholderData[19] / 2;
	float yDiff   = placeholderData[20] / 2;
	float xCenter = placeholderData[16];
	float yCenter = placeholderData[17];

	Vec4 vec;
	vec.x = xCenter - xDiff;
	vec.y = xCenter + xDiff;
	vec.z = yCenter - yDiff;
	vec.w = yCenter + yDiff;

	return vec;
}

void
initCompositionData (Map<String, void *> *out) {
	if (out->root) FreeSubLayers (out, out, (void *)*(u64 *)((u64)out->root + 8));

	out->root          = (MapElement<String, void *> *)calloc (1, 0xB0);
	out->root->left    = out->root;
	out->root->parent  = out->root;
	out->root->right   = out->root;
	out->root->isBlack = true;
	out->root->isNull  = true;
	out->length        = 0;
}

Vec2
getClickedPos (void *inputState) {
	Vec2 initialVec = createVec2 (*(i32 *)((u64)inputState + 0x158), *(i32 *)((u64)inputState + 0x15C));

	RECT rect;
	GetClientRect (FindWindow ("DIVAMIXP", 0), &rect);
	float x = initialVec.x / (float)rect.right * 1920;
	float y = initialVec.y / (float)rect.bottom * 1080;

	return createVec2 (x, y);
}
