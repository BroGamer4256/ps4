#pragma once
#include "helpers.h"

typedef struct string {
	union {
		char data[16];
		char *ptr;
	};
	u64 length;
	u64 capacity;
} string;

void appendThemeInPlace (char *name);
char *appendTheme (const char *name);
void appendStringInPlace (string *str, const char *append);
void appendThemeInPlaceString (string *name);

FUNCTION_PTR_H (bool, __thiscall, CmnMenuTaskDest, u64 This);
FUNCTION_PTR_H (void, __stdcall, DrawTextBox, u64 a1, i32 index);
FUNCTION_PTR_H (void, __stdcall, HideTextBox, u64 a1, i32 index);
FUNCTION_PTR_H (void *, __stdcall, DivaGetInputState, i32 a1);
FUNCTION_PTR_H (bool, __stdcall, IsButtonTapped, void *state, i32 offset);
FUNCTION_PTR_H (void, __stdcall, LoadAet, void *data, i32 aetSceneId, const char *layerName, i32 layer, i32 action);
FUNCTION_PTR_H (i32, __stdcall, PlayAet, void *data, u64 a2);
FUNCTION_PTR_H (void *, __stdcall, GetPlaceholders, void *placeholderData, i32 id);
FUNCTION_PTR_H (float *, __stdcall, GetPlaceholder, void *placeholderData, const char *name);
FUNCTION_PTR_H (void, __stdcall, ApplyPlaceholder, void *data, Vec3 *placeholderLocation);
FUNCTION_PTR_H (void, __stdcall, PlaySoundEffect, const char *name, float volume);
FUNCTION_PTR_H (u64, __stdcall, GetPvLoadData);
FUNCTION_PTR_H (i32, __stdcall, GetCurrentStyle);
