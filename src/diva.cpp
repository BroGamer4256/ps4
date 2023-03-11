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

FUNCTION_PTR (bool, CmnMenuDestroy, 0x1401AAE50, u64 This);
FUNCTION_PTR (void *, GetInputState, 0x1402AC970, i32 a1);
FUNCTION_PTR (bool, IsButtonTapped, 0x1402AB260, void *state, Button button);
FUNCTION_PTR (void *, CreateAetLayerData, 0x14028D560, aetLayerArgs *args, i32 aetSceneId, const char *layerName, i32 priority, AetAction action);
FUNCTION_PTR (i32, PlayAetLayer, 0x1402CA220, aetLayerArgs *args, i32 id);
FUNCTION_PTR (void, GetComposition, 0x1402CA670, aetComposition *composition, i32 id);
FUNCTION_PTR (void, ApplyAetLayerLocation, 0x14065FCC0, aetLayerArgs *args, Vec3 *locationData);
FUNCTION_PTR (void, PlaySoundEffect, 0x1405AA540, const char *name, float volume);
FUNCTION_PTR (u64, GetPvLoadData, 0x14040B2A0);
FUNCTION_PTR (i32, GetCurrentStyle, 0x1401D64F0);
FUNCTION_PTR (InputType, NormalizeInputType, 0x1402ACAA0, i32 inputType);
FUNCTION_PTR (void, FreeSubLayers, 0x1401AC240, aetComposition *sublayerData, aetComposition *sublayerData2, void *first_element);
FUNCTION_PTR (void, StopAet, 0x1402CA330, i32 *id);
FUNCTION_PTR (void *, operatorNew, sigOperatorNew (), u64);
FUNCTION_PTR (void *, operatorDelete, sigOperatorDelete (), void *);
FUNCTION_PTR (void, FreeString, 0x14014bcd0, string *);

vector<pvDbEntry *> *pvs         = (vector<pvDbEntry *> *)0x141753818;
map<i32, PvSpriteIds> *pvSprites = (map<i32, PvSpriteIds> *)0x14CBBACC0;
map<i32, aetData> *aetLayers     = (map<i32, aetData> *)0x1414AB448;

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

bool
isMovieOnly (pvDbEntry *entry) {
	if (entry->extreme.first->isMovieOnly) return true;

	return false;
}

std::optional<pvDbEntry *>
getPvDbEntry (i32 id) {
	if (id < 0) return std::nullopt;
	for (auto entry = pvs->first; entry != pvs->last; entry++) {
		if ((*entry)->id != id) continue;
		return std::optional (*entry);
	}
	return std::nullopt;
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
	if (auto placeholderData = composition.find (string (name))) return std::optional (getPlaceholderRect (**placeholderData));
	else return std::nullopt;
}

aetLayerArgs::aetLayerArgs (i32 sceneId, const char *layerName, i32 priority, AetAction action) { this->with_data (sceneId, layerName, priority, action); }

void
aetLayerArgs::with_data (i32 sceneId, const char *layerName, i32 priority, AetAction action) {
	CreateAetLayerData (this, sceneId, layerName, priority, action);
}

void
aetLayerArgs::play (i32 *id) {
	*id = PlayAetLayer (this, *id);
}

void
aetLayerArgs::setPosition (Vec3 position) {
	ApplyAetLayerLocation (this, &position);
}

template <>
aetComposition::~map () {
	for (auto it = this->begin (); it != this->end (); it = it->next ()) {
		it->key.~string ();
		deallocate (it);
	}

	deallocate (this->root);
}
} // namespace diva
