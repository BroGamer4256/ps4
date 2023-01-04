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
