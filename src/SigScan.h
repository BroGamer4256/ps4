#pragma once
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <windows.h>

#define SIG_SCAN(x, y)                                                                                                                                                                                 \
	void *x ();                                                                                                                                                                                        \
	void *x##Addr = x ();                                                                                                                                                                              \
	void *x () {                                                                                                                                                                                       \
		if (!x##Addr) {                                                                                                                                                                                \
			x##Addr = sigScan (y);                                                                                                                                                                     \
			if (x##Addr) return x##Addr;                                                                                                                                                               \
			sigValid = false;                                                                                                                                                                          \
		}                                                                                                                                                                                              \
		return x##Addr;                                                                                                                                                                                \
	}

#define SIG_SCAN_STRING(x, y)                                                                                                                                                                          \
	void *x ();                                                                                                                                                                                        \
	void *x##Addr = x ();                                                                                                                                                                              \
	void *x () {                                                                                                                                                                                       \
		if (!x##Addr) {                                                                                                                                                                                \
			x##Addr = sigScanString (y);                                                                                                                                                               \
			if (x##Addr) return x##Addr;                                                                                                                                                               \
			sigValid = false;                                                                                                                                                                          \
		}                                                                                                                                                                                              \
		return x##Addr;                                                                                                                                                                                \
	}

void *sigScan (const char *signature);
void *sigScanString (const char *signature);

// Automatically scanned signatures, these are expected to exist in all game
// versions sigValid is going to be false if any automatic signature scan fails
extern bool sigValid;
