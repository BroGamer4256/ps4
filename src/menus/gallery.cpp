#include "diva.h"
#include "menus.h"

namespace gallery {
void *menuTxt1Data    = calloc (1, 0x1024);
void *menuTxt2Data    = calloc (1, 0x1024);
void *menuTxt3Data    = calloc (1, 0x1024);
void *menuTxt4Data    = calloc (1, 0x1024);
void *menuTxt5Data    = calloc (1, 0x1024);
void *menuTxtBaseData = calloc (1, 0x1024);
i32 menuTxt1Id        = 0;
i32 menuTxt2Id        = 0;
i32 menuTxt3Id        = 0;
i32 menuTxt4Id        = 0;
i32 menuTxt5Id        = 0;
i32 menuTxtBaseId     = 0;

using diva::AetAction;

void
playGalleryTxt (i32 button, AetAction action) {
	switch (button) {
	case 0:
		CreateAetLayerData (menuTxt1Data, 0x4FE, "menu_txt_01", 0x12, action);
		menuTxt1Id = PlayAetLayer (menuTxt1Data, menuTxt1Id);
		break;
	case 1:
		CreateAetLayerData (menuTxt2Data, 0x4FE, "menu_txt_02", 0x12, action);
		menuTxt2Id = PlayAetLayer (menuTxt2Data, menuTxt2Id);
		break;
	case 2:
		CreateAetLayerData (menuTxt3Data, 0x4FE, "menu_txt_03", 0x12, action);
		menuTxt3Id = PlayAetLayer (menuTxt3Data, menuTxt3Id);
		break;
	case 3:
		CreateAetLayerData (menuTxt4Data, 0x4FE, "menu_txt_04", 0x12, action);
		menuTxt4Id = PlayAetLayer (menuTxt4Data, menuTxt4Id);
		break;
	case 4:
		CreateAetLayerData (menuTxt5Data, 0x4FE, "menu_txt_05", 0x12, action);
		menuTxt5Id = PlayAetLayer (menuTxt5Data, menuTxt5Id);
		break;
	}
	CreateAetLayerData (menuTxtBaseData, 0x4FE, "menu_txt_base", 0x12, action);
	menuTxtBaseId = PlayAetLayer (menuTxtBaseData, menuTxtBaseId);
}

// Fixes gallery not properly exiting and text boxes not working
i32 previousButton = 5;
HOOK (bool, __thiscall, CsGalleryLoop, 0x1401AD590, u64 This) {
	i32 state          = *(i32 *)(This + 0x68);
	i32 selectedButton = *(i32 *)(This + 0x70);
	if (state == 3 && previousButton != selectedButton) {
		playGalleryTxt (selectedButton, AetAction::IN_LOOP);
		if (previousButton != 5) playGalleryTxt (previousButton, AetAction::OUT_ONCE);
		previousButton = selectedButton;
	} else if (state == 6) {
		*(i32 *)(This + 0x68)   = 14;
		*(i32 *)(This + 0x6C)   = 1;
		*(i32 *)(This + 0x4598) = 5;
		previousButton          = 5;
		playGalleryTxt (selectedButton, AetAction::OUT_ONCE);
	} else if (state == 4) {
		previousButton = 5;
		playGalleryTxt (selectedButton, AetAction::OUT_ONCE);
	}

	return originalCsGalleryLoop (This);
}

void
init () {
	INSTALL_HOOK (CsGalleryLoop);
}
} // namespace gallery
