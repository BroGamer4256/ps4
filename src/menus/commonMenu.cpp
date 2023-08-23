#include "diva.h"

namespace commonMenu {
using namespace diva;
InputType prevInput;
bool inited = true;
i32 sceneId;
const char *layerName;

bool
CommonMenuDisplay (u64 This) {
	auto footer_id = (i32 *)(This + 0x70);
	auto input     = getInputType ();

	if (!inited || input != prevInput) {
		prevInput = input;
		inited    = true;

		char buf[128];
		sprintf (buf, "%s_footer_%02d", layerName, (i32)input);
		AetLayerArgs args (sceneId, buf, 0xE, AetAction::NONE);
		args.play (footer_id);
	}

	return false;
}

HOOK (void, PlayHeaderFooter, 0x1401AB150, u64 a1, u32 id, const char *name) {
	inited    = false;
	sceneId   = id;
	layerName = name;
	return originalPlayHeaderFooter (a1, id, name);
}

void
init () {
	taskAddition addition;
	addition.display = CommonMenuDisplay;
	addTaskAddition ("CMN_MENU", addition);

	INSTALL_HOOK (PlayHeaderFooter);
}
} // namespace commonMenu
