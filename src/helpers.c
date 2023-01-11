#include "helpers.h"
#include <windows.h>

toml_table_t *
openConfig (const char *configFilePath) {
	FILE *file = fopen (configFilePath, "r");
	if (!file) { return 0; }
	char errorbuf[200];
	toml_table_t *config = toml_parse_file (file, errorbuf, 200);
	fclose (file);

	if (!config) { return 0; }

	return config;
}

toml_table_t *
openConfigSection (toml_table_t *config, const char *sectionName) {
	toml_table_t *section = toml_table_in (config, sectionName);
	if (!section) { return 0; }

	return section;
}

bool
readConfigBool (toml_table_t *table, const char *key, bool notFoundValue) {
	toml_datum_t data = toml_bool_in (table, key);
	if (!data.ok) return notFoundValue;

	return (bool)data.u.b;
}

int64_t
readConfigInt (toml_table_t *table, const char *key, int64_t notFoundValue) {
	toml_datum_t data = toml_int_in (table, key);
	if (!data.ok) return notFoundValue;

	return data.u.i;
}

char *
readConfigString (toml_table_t *table, const char *key, char *notFoundValue) {
	toml_datum_t data = toml_string_in (table, key);
	if (!data.ok) return notFoundValue;

	return data.u.s;
}

Vec2
createVec2 (float x, float y) {
	Vec2 vec;
	vec.x = x;
	vec.y = y;
	return vec;
}

Vec3
createVec3 (float x, float y, float z) {
	Vec3 vec;
	vec.x = x;
	vec.y = y;
	vec.z = z;
	return vec;
}

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

bool
vec4ContainsVec2 (Vec4 box, Vec2 location) {
	return location.x > box.x && location.x < box.y && location.y > box.z && location.y < box.w;
}
