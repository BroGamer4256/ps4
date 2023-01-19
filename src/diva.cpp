#include "diva.h"
#include "helpers.h"

FUNCTION_PTR (bool, __thiscall, CmnMenuTaskDest, 0x1401AAE50, u64 This);
FUNCTION_PTR (void, __stdcall, DrawTextBox, 0x1401ACDA0, u64 a1, i32 index);
FUNCTION_PTR (void, __stdcall, HideTextBox, 0x1401ACAD0, u64 a1, i32 index);
FUNCTION_PTR (void *, __stdcall, DivaGetInputState, 0x1402AC960, i32 a1);
FUNCTION_PTR (bool, __stdcall, IsButtonTapped, 0x1402AB250, void *state, i32 offset);
FUNCTION_PTR (void, __stdcall, LoadAet, 0x14028D550, void *data, i32 aetSceneId, const char *layerName, i32 layer, AetAction action);
FUNCTION_PTR (i32, __stdcall, PlayAet, 0x1402CA1E0, void *data, i32 id);
FUNCTION_PTR (void *, __stdcall, GetSubLayers, 0x1402CA630, List<void> *sublayerData, i32 id);
FUNCTION_PTR (float *, __stdcall, GetSubLayer, 0x1402CA740, List<void> *sublayerData, const char *name);
FUNCTION_PTR (void, __stdcall, ApplyLocation, 0x14065FA00, void *data, Vec3 *locationData);
FUNCTION_PTR (void, __stdcall, PlaySoundEffect, 0x1405AA500, const char *name, float volume);
FUNCTION_PTR (u64, __stdcall, GetPvLoadData, 0x14040B260);
FUNCTION_PTR (i32, __stdcall, GetCurrentStyle, 0x1401D64E0);
FUNCTION_PTR (InputType, __stdcall, NormalizeInputType, 0x1402ACA90, i32 inputType);
FUNCTION_PTR (String *, __stdcall, StringInit, 0x14014BA50, String *to, const char *from, u64 len);
FUNCTION_PTR (void, __stdcall, FreeSubLayers, 0x1401AC240, List<void> *sublayerData, List<void> *sublayerData2, void *first_element);
FUNCTION_PTR (void, __stdcall, FreeString, 0x14014BCD0, String *str);
FUNCTION_PTR (void, __stdcall, FreeBase, 0x1409880A8, void *data, u64 size);

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
	char *themeStr = (char *)calloc (strlen (name) + 4, sizeof (char));
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

List<ListElement<i32>> *pvs = (List<ListElement<i32>> *)0x141753808;

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
initSublayerData (List<void> *out) {
	if (out->length > 0) {
		for (u64 cur_element = *(u64 *)((u64)out->empty_element + 8); *(u8 *)(cur_element + 0x19) == 0; cur_element = *(u64 *)(cur_element + 0x10))
			FreeSubLayers (out, out, (void *)cur_element);
		free (out->empty_element);
	}

	out->empty_element                       = calloc (1, 0xB0);
	*(void **)out->empty_element             = out->empty_element;
	*(void **)((u64)out->empty_element + 8)  = out->empty_element;
	*(void **)((u64)out->empty_element + 16) = out->empty_element;
	*(u16 *)((u64)out->empty_element + 24)   = 0x101;
	out->length                              = 0;
}
