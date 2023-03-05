#include "SigScan.h"
#include <psapi.h>

MODULEINFO moduleInfo;

const MODULEINFO &
getModuleInfo () {
	if (moduleInfo.SizeOfImage) return moduleInfo;

	ZeroMemory (&moduleInfo, sizeof (MODULEINFO));
	GetModuleInformation (GetCurrentProcess (), GetModuleHandle (nullptr), &moduleInfo, sizeof (MODULEINFO));

	return moduleInfo;
}

void *
sigScan (const char *signature) {
	const MODULEINFO &info = getModuleInfo ();
	const u64 sigSize      = strlen (signature);

	char *newSig    = (char *)calloc (sigSize + 1, sizeof (char));
	int newSigIndex = 0;
	for (int i = 0; i < sigSize; i++) {
		if (signature[i] == ' ') continue;
		newSig[newSigIndex] = signature[i];
		newSigIndex++;
	}

	if (strlen (newSig) % 2 != 0) return 0;
	u64 newSigSize = strlen (newSig) / 2;

	for (u64 i = 0; i < info.SizeOfImage; i++) {
		u8 *currMemory = (u8 *)info.lpBaseOfDll + i;

		u64 j;
		for (j = 0; j < newSigSize; j++) {
			char buf[3];
			buf[0] = newSig[j];
			buf[1] = newSig[j + 1];
			buf[2] = 0;
			if (strcmp (buf, "??") == 0) continue;
			if (strtol (buf, 0, 16) == currMemory[j]) continue;
			break;
		}

		if (j == sigSize) {
			free (newSig);
			return currMemory;
		}
	}

	free (newSig);
	return 0;
}

void *
sigScanString (const char *signature) {
	const MODULEINFO &info = getModuleInfo ();
	const u64 sigSize      = strlen (signature);

	for (u64 i = 0; i < info.SizeOfImage; i++) {
		u8 *currMemory = (u8 *)info.lpBaseOfDll + i;

		u64 j;
		for (j = 0; j < sigSize; j++)
			if (signature[j] != currMemory[j]) break;

		if (j == sigSize) return currMemory;
	}

	return 0;
}

bool sigValid = true;
