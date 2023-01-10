#pragma once
#include "helpers.h"

typedef struct Vec2 {
	float x;
	float y;
} Vec2;

typedef struct Vec3 {
	float x;
	float y;
	float z;
} Vec3;

typedef struct Vec4 {
	float x;
	float y;
	float z;
	float w;
} Vec4;

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
Vec2 createVec2 (float x, float y);
Vec3 createVec3 (float x, float y, float z);
Vec4 getPlaceholderRect (float *placeholderData);
bool vec4ContainsVec2 (Vec4 box, Vec2 location);
