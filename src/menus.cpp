#include "menus.h"
#include "menus/exit.h"
#include "menus/shaderSel.h"

void
customMenusHook () {
	exitMenuHook ();
	shaderSelHook ();
}
