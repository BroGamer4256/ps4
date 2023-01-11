#include "diva.h"
#include "helpers.h"

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
appendStringInPlace (string *str, const char *append) {
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
appendThemeInPlaceString (string *name) {
	switch (theme) {
	case 1: appendStringInPlace (name, "_f"); break;
	case 2: appendStringInPlace (name, "_t"); break;
	default: appendStringInPlace (name, "_ft"); break;
	}
}

FUNCTION_PTR (bool, __thiscall, CmnMenuTaskDest, 0x1401AAE50, u64 This);
FUNCTION_PTR (void, __stdcall, DrawTextBox, 0x1401ACDA0, u64 a1, i32 index);
FUNCTION_PTR (void, __stdcall, HideTextBox, 0x1401ACAD0, u64 a1, i32 index);
FUNCTION_PTR (void *, __stdcall, DivaGetInputState, 0x1402AC960, i32 a1);
FUNCTION_PTR (bool, __stdcall, IsButtonTapped, 0x1402AB250, void *state, i32 offset);
FUNCTION_PTR (void, __stdcall, LoadAet, 0x14028D550, void *data, i32 aetSceneId, const char *layerName, i32 layer, i32 action);
FUNCTION_PTR (i32, __stdcall, PlayAet, 0x1402CA1E0, void *data, u64 a2);
FUNCTION_PTR (void *, __stdcall, GetPlaceholders, 0x1402CA630, void *placeholderData, i32 id);
FUNCTION_PTR (float *, __stdcall, GetPlaceholder, 0x1402CA740, void *placeholderData, const char *name);
FUNCTION_PTR (void, __stdcall, ApplyPlaceholder, 0x14065fa00, void *data, Vec3 *placeholderLocation);
FUNCTION_PTR (void, __stdcall, PlaySoundEffect, 0x1405AA500, const char *name, float volume);
FUNCTION_PTR (u64, __stdcall, GetPvLoadData, 0x14040B260);
FUNCTION_PTR (i32, __stdcall, GetCurrentStyle, 0x1401D64E0);
