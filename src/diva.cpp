#include "diva.h"
#include "SigScan.h"

extern i32 theme;
namespace diva {
SIG_SCAN (sigOperatorNew, 0x1409777D0,
          "\x40\x53\x48\x83\xEC\x20\x48\x8B\xD9\xEB\x0F\x48\x8B\xCB\xE8\x00\x00\x00\x00\x85\xC0\x74\x13\x48\x8B\xCB\xE8\x00\x00\x00\x00\x48\x85\xC0\x74\xE7\x48\x83\xC4\x20\x5B\xC3\x48\x83\xFB\xFF\x74"
          "\x06\xE8\x00\x00\x00\x00\xCC\xE8\x00\x00\x00\x00\xCC\x40\x53",
          "xxxxxxxxxxxxxxx????xxxxxxxx????xxxxxxxxxxxxxxxxxx????xx????xxx");
SIG_SCAN (sigOperatorDelete, 0x1409B1E90,
          "\x48\x85\xC9\x74\x37\x53\x48\x83\xEC\x20\x4C\x8B\xC1\x33\xD2\x48\x8B\x0D\x00\x00\x00\x00\xFF\x15\x00\x00\x00\x00\x85\xC0\x75\x17\xE8\x00\x00\x00\x00\x48\x8B\xD8\xFF\x15\x00\x00\x00\x00\x8B"
          "\xC8\xE8\x00\x00\x00\x00\x89\x03\x48\x83\xC4\x20\x5B\xC3",
          "xxxxxxxxxxxxxxxxxx????xx????xxxxx????xxxxx????xxx????xxxxxxxx");

FUNCTION_PTR (bool, __thiscall, CmnMenuDestroy, 0x1401AAE50, u64 This);
FUNCTION_PTR (void *, __stdcall, GetInputState, 0x1402AC970, i32 a1);
FUNCTION_PTR (bool, __stdcall, IsButtonTapped, 0x1402AB260, void *state, Button button);
FUNCTION_PTR (void *, __stdcall, CreateAetLayerData, 0x14028D560, aetLayerArgs *args, i32 aetSceneId, const char *layerName, i32 layer, AetAction action);
FUNCTION_PTR (i32, __stdcall, PlayAetLayer, 0x1402CA220, aetLayerArgs *args, i32 id);
FUNCTION_PTR (void, __stdcall, GetComposition, 0x1402CA670, aetComposition *composition, i32 id);
FUNCTION_PTR (void, __stdcall, ApplyAetLayerLocation, 0x14065FCC0, aetLayerArgs *args, Vec3 *locationData);
FUNCTION_PTR (void, __stdcall, PlaySoundEffect, 0x1405AA540, const char *name, float volume);
FUNCTION_PTR (u64, __stdcall, GetPvLoadData, 0x14040B2A0);
FUNCTION_PTR (i32, __stdcall, GetCurrentStyle, 0x1401D64F0);
FUNCTION_PTR (InputType, __stdcall, NormalizeInputType, 0x1402ACAA0, i32 inputType);
FUNCTION_PTR (void, __stdcall, FreeSubLayers, 0x1401AC240, aetComposition *sublayerData, aetComposition *sublayerData2, void *first_element);
FUNCTION_PTR (void, __stdcall, StopAet, 0x1402CA330, i32 *id);
FUNCTION_PTR (void *, __fastcall, operatorNew, sigOperatorNew (), u64);
FUNCTION_PTR (void *, __fastcall, operatorDelete, sigOperatorDelete (), void *);

list<i32> *pvs                   = (list<i32> *)0x141753808;
map<i32, PvSpriteIds> *pvSprites = (map<i32, PvSpriteIds> *)0x14CBBACC0;

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
appendStringInPlace (string *str, const char *append) {
	u64 lengthNeeded = str->length + strlen (append) + 1;
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

InputType
getInputType () {
	return NormalizeInputType (*(i32 *)((u64)GetInputState (0) + 0x2E8));
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
	for (auto current = pvs->begin (); current != pvs->end (); current = current->next) {
		if (current->current != id) continue;
		return (u64)&current->current;
	}
	return 0;
}

Vec4
getPlaceholderRect (aetLayerData layer) {
	float xDiff = layer.width / 2;
	float yDiff = layer.height / 2;

	Vec4 vec;
	vec.x = layer.position.x - xDiff;
	vec.y = layer.position.x + xDiff;
	vec.z = layer.position.y - yDiff;
	vec.w = layer.position.y + yDiff;

	return vec;
}

Vec2
getClickedPos (void *inputState) {
	Vec2 initialVec (*(i32 *)((u64)inputState + 0x158), *(i32 *)((u64)inputState + 0x15C));

	RECT rect;
	GetClientRect (FindWindow ("DIVAMIXP", 0), &rect);
	float x = initialVec.x / (float)rect.right * 1920;
	float y = initialVec.y / (float)rect.bottom * 1080;

	return Vec2 (x, y);
}

std::optional<Vec4>
getTouchArea (aetComposition composition, const char *name) {
	auto placeholderData = composition.find (string (name));
	if (placeholderData) return std::optional (getPlaceholderRect (placeholderData->value));
	else return std::nullopt;
}

aetLayerArgs::aetLayerArgs (i32 sceneId, const char *layerName, i32 layer, AetAction action) { this->with_data (sceneId, layerName, layer, action); }

void
aetLayerArgs::with_data (i32 sceneId, const char *layerName, i32 layer, AetAction action) {
	CreateAetLayerData (this, sceneId, layerName, layer, action);
}

void
aetLayerArgs::play (i32 *id) {
	*id = PlayAetLayer (this, *id);
}

void
aetLayerArgs::setPosition (Vec3 position) {
	ApplyAetLayerLocation (this, &position);
}
} // namespace diva
