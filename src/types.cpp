#include "types.h"
#include "mod.h"

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

Vec2
createVec2 (float x, float y) {
	return Vec2{ x, y };
}

Vec3
createVec3 (float x, float y, float z) {
	return Vec3{ x, y, z };
}

Vec4
getPlaceholderRect (float *placeholderData) {
	float xDiff   = placeholderData[19] / 2;
	float yDiff   = placeholderData[20] / 2;
	float xCenter = placeholderData[16];
	float yCenter = placeholderData[17];
	return Vec4{
		xCenter - xDiff, // Left
		xCenter + xDiff, // Right
		yCenter - yDiff, // Top
		yCenter + yDiff, // Bottom
	};
}

bool
vec4ContainsVec2 (Vec4 box, Vec2 location) {
	return location.x > box.x && location.x < box.y && location.y > box.z && location.y < box.w;
}
