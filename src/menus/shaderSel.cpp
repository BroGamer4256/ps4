#include "diva.h"
#include "helpers.h"

namespace shaderSel {
// Allow swapping of visual style on song select
HOOK (bool, __thiscall, PVSelCtrl, 0x1402033B0, u64 This) {
	// Disable on playlists
	if (*(i32 *)(This + 0x36A08) != 0) return originalPVSelCtrl (This);

	bool isMovie     = isMovieOnly (getPvDbEntry (*(i32 *)(This + 0x36A30)));
	InputType input  = getInputType ();
	void *inputState = DivaGetInputState (0);
	u64 data         = GetPvLoadData ();

	if (*(i32 *)(data + 0x1D08) == -1) *(i32 *)(data + 0x1D08) = GetCurrentStyle ();

	if (IsButtonTapped (inputState, 15)) { // F3
		PlaySoundEffect ("se_ft_music_selector_select_01", 1.0);
		*(i32 *)(data + 0x1D08) = !*(i32 *)(data + 0x1D08);
	} else if (IsButtonTapped (inputState, 3) || IsButtonTapped (inputState, 4)) { // Up, Down
		*(i32 *)(data + 0x1D08) = GetCurrentStyle ();
	}

	return originalPVSelCtrl (This);
}

void
init () {
	INSTALL_HOOK (PVSelCtrl);
	INSTALL_HOOK (PvSelDestroy);
}
} // namespace shaderSel
