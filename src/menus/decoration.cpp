#include "diva.h"
#include "menus.h"

namespace decoration {
using namespace diva;
diva::AetLayerArgs **cmnbgArgs = (diva::AetLayerArgs **)0x14CC5EEA0;
i32 bgId                       = 0;
bool decorationInited          = false;
u8 decoScreen                  = 1;
i32 footerButtonId             = 0;

bool
DecorationLoop (u64 a1) {
	if (!decorationInited) {
		diva::StopAet (&(*cmnbgArgs)->id);
		diva::AetLayerArgs bgArgs ("AET_NSWGAM_DECORATION_MAIN", "bg", 5, AetAction::NONE);
		bgArgs.play (&bgId);
		decorationInited = true;
	}
	u8 screen = *(u8 *)(a1 + 0x6B);
	if (screen != decoScreen) {
		decoScreen      = screen;
		InputType input = diva::getInputType ();
		char buf[128];
		sprintf (buf, "footer_button_%02d_%02d", decoScreen + 1, (i32)input);
		diva::AetLayerArgs footerArgs ("AET_NSWGAM_DECORATION_MAIN", buf, 11, AetAction::NONE);
		footerArgs.play (&footerButtonId);
	}
	return false;
}

bool
DecorationDestroy (u64 a1) {
	diva::StopAet (&bgId);
	(*cmnbgArgs)->play (&(*cmnbgArgs)->id);
	decorationInited = false;
	diva::StopAet (&footerButtonId);
	return false;
}

void
init () {
	diva::taskAddition decoAddition;
	decoAddition.loop    = DecorationLoop;
	decoAddition.destroy = DecorationDestroy;
	diva::addTaskAddition ("DECO", decoAddition);
}

void
placeholderApply (AetLayerArgs *args, AetLayerData *placeholder) {
	if (auto aet = aets->find (args->id)) aet.value ()->color.w = placeholder->opacity;
}
} // namespace decoration
