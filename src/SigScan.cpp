#include "SigScan.h"
#include "helpers.h"
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

	for (u64 i = 0; i < info.SizeOfImage; i++) {
		u8 *currMemory = (u8 *)info.lpBaseOfDll + i;

		u64 j;
		for (j = 0; j < sigSize; j++)
			if (signature[j] != '?' && signature[j] != currMemory[j]) break;

		if (j == sigSize) return currMemory;
	}

	return 0;
}

bool sigValid = true;
