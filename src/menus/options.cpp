#include "diva.h"
#include "helpers.h"
#include "menus.h"

namespace options {
void *optionMenuTopData = calloc (1, 0x1024);
i32 optionMenuTopId     = 0;
HOOK (bool, __stdcall, OptionMenuSwitchInit, 0x1406C3CB0, void *a1) {
	CreateAetLayerData (optionMenuTopData, 0x525, "option_top_menu loop", 7, AETACTION_NONE);
	optionMenuTopId = PlayAetLayer (optionMenuTopData, optionMenuTopId);
	return originalOptionMenuSwitchInit (a1);
}

HOOK (bool, __stdcall, OptionMenuSwitchDestroy, 0x1406C2CE0, void *a1) {
	StopAet (&optionMenuTopId);
	return originalOptionMenuSwitchDestroy (a1);
}

void
init () {
	INSTALL_HOOK (OptionMenuSwitchInit);
	INSTALL_HOOK (OptionMenuSwitchDestroy);
}
} // namespace options
