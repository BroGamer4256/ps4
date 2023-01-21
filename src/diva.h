#pragma once
#include "helpers.h"

typedef struct String {
	union {
		char data[16];
		char *ptr;
	};
	u64 length;
	u64 capacity;
} String;

template <typename T>
struct ListElement {
	ListElement<T> *next;
	ListElement<T> *previous;
	T current;
};

template <typename T>
struct List {
	T *empty_element;
	u64 length;
};

typedef enum State : i32 {
	STATE_STARTUP     = 0,
	STATE_ADVERTISE   = 1,
	STATE_GAME        = 2,
	STATE_DATA_TEST   = 3,
	STATE_TEST_MODE   = 4,
	STATE_APP_ERROR   = 5,
	STATE_CS_MENU     = 6,
	STATE_CUSTOMIZE   = 7,
	STATE_GALLERY     = 8,
	STATE_MENU_SWITCH = 9,
	STATE_GAME_SWITCH = 10,
	STATE_TSHIRT_EDIT = 11,
	STATE_MAX         = 12,
} State;

typedef enum SubState : i32 {
	SUBSTATE_DATA_INITIALIZE           = 0,
	SUBSTATE_SYSTEM_STARTUP            = 1,
	SUBSTATE_LOGO                      = 2,
	SUBSTATE_TITLE                     = 3,
	SUBSTATE_CONCEAL                   = 4,
	SUBSTATE_PV_SEL                    = 5,
	SUBSTATE_PLAYLIST_SEL              = 6,
	SUBSTATE_GAME                      = 7,
	SUBSTATE_DATA_TEST_MAIN            = 8,
	SUBSTATE_DATA_TEST_MISC            = 9,
	SUBSTATE_DATA_TEST_OBJ             = 10,
	SUBSTATE_DATA_TEST_STG             = 11,
	SUBSTATE_DATA_TEST_MOT             = 12,
	SUBSTATE_DATA_TEST_COLLISION       = 13,
	SUBSTATE_DATA_TEST_SPR             = 14,
	SUBSTATE_DATA_TEST_AET             = 15,
	SUBSTATE_DATA_TEST_AUTH3D          = 16,
	SUBSTATE_DATA_TEST_CHR             = 17,
	SUBSTATE_DATA_TEST_ITEM            = 18,
	SUBSTATE_DATA_TEST_PERF            = 19,
	SUBSTATE_DATA_TEST_PVSCRIPT        = 20,
	SUBSTATE_DATA_TEST_PRINT           = 21,
	SUBSTATE_DATA_TEST_CARD            = 22,
	SUBSTATE_DATA_TEST_OPD             = 23,
	SUBSTATE_DATA_TEST_SLIDER          = 24,
	SUBSTATE_DATA_TEST_GLITTER         = 25,
	SUBSTATE_DATA_TEST_GRAPHICS        = 26,
	SUBSTATE_DATA_TEST_COLLECTION_CARD = 27,
	SUBSTATE_DATA_TEST_PAD             = 28,
	SUBSTATE_TEST_MODE                 = 29,
	SUBSTATE_APP_ERROR                 = 30,
	SUBSTATE_UNK_31                    = 31,
	SUBSTATE_CS_MENU                   = 32,
	SUBSTATE_CS_COMMERCE               = 33,
	SUBSTATE_CS_OPTION_MENU            = 34,
	SUBSTATE_CS_UNK_TUTORIAL_35        = 35,
	SUBSTATE_CS_CUSTOMIZE_SEL          = 36,
	SUBSTATE_CS_UNK_TUTORIAL_37        = 37,
	SUBSTATE_CS_GALLERY                = 38,
	SUBSTATE_UNK_39                    = 39,
	SUBSTATE_UNK_40                    = 40,
	SUBSTATE_UNK_41                    = 41,
	SUBSTATE_MENU_SWITCH               = 42,
	SUBSTATE_UNK_43                    = 43,
	SUBSTATE_OPTION_MENU_SWITCH        = 44,
	SUBSTATE_UNK_45                    = 45,
	SUBSTATE_UNK_46                    = 46,
	SUBSTATE_MAX                       = 47,
} SubState;

typedef enum InputType : i32 {
	INPUTTYPE_XBOX        = 0,
	INPUTTYPE_PLAYSTATION = 1,
	INPUTTYPE_SWITCH      = 2,
	INPUTTYPE_STEAM       = 3,
	INPUTTYPE_KEYBOARD    = 4,
	INPUTTYPE_UNKNOWN     = 5,
} InputType;

typedef enum AetAction : i32 {
	AETACTION_NONE         = 0, // Ignore all markers, just play through it all
	AETACTION_IN           = 1, // Start at st_in, end at ed_in
	AETACTION_IN_LOOP      = 2, // Start at st_in, play to ed_lp then jump to st_lp and keep looping
	AETACTION_LOOP         = 3, // Start at st_lp, end at ed_lp, loops
	AETACTION_OUT          = 4, // Start at st_out, end at ed_out
	AETACTION_SPECIAL_ONCE = 5, // Start at st_sp, end to ed_lp
	AETACTION_SPECIAL_LOOP = 6, // Start at st_sp, play to ed_sp, then jump to st_lp and loop through ed_lp
	AETACTION_UNK          = 7, // Start at st_in, end at ed_in, probably loops?
} AetAction;

typedef enum Button : i32 {
	BUTTON_UP       = 3,
	BUTTON_DOWN     = 4,
	BUTTON_LEFT     = 5,
	BUTTON_RIGHT    = 6,
	BUTTON_TRIANGLE = 7,
	BUTTON_SQUARE   = 8,
	BUTTON_BACK     = 9,
	BUTTON_ACCEPT   = 10,
	BUTTON_L        = 11,
	BUTTON_R        = 12,
	BUTTON_L2       = 13,
	BUTTON_R2       = 14,
	BUTTON_L3       = 15,
	BUTTON_R3       = 16,
} Button;

extern List<ListElement<i32>> *pvs;

FUNCTION_PTR_H (bool, __thiscall, CmnMenuTaskDest, u64 This);
FUNCTION_PTR_H (void *, __stdcall, DivaGetInputState, i32 a1);
FUNCTION_PTR_H (bool, __stdcall, IsButtonTapped, void *state, Button button);
FUNCTION_PTR_H (void, __stdcall, LoadAet, void *data, i32 aetSceneId, const char *layerName, i32 layer, AetAction action);
FUNCTION_PTR_H (i32, __stdcall, PlayAet, void *data, i32 id);
FUNCTION_PTR_H (void *, __stdcall, GetSubLayers, List<void> *sublayerData, i32 id);
FUNCTION_PTR_H (float *, __stdcall, GetSubLayer, List<void> *sublayerData, const char *name);
FUNCTION_PTR_H (void, __stdcall, ApplyLocation, void *data, Vec3 *locationData);
FUNCTION_PTR_H (void, __stdcall, PlaySoundEffect, const char *name, float volume);
FUNCTION_PTR_H (u64, __stdcall, GetPvLoadData);
FUNCTION_PTR_H (i32, __stdcall, GetCurrentStyle);
FUNCTION_PTR_H (InputType, __stdcall, NormalizeInputType, i32 inputType);
FUNCTION_PTR_H (String *, __stdcall, StringInit, String *to, const char *from, u64 len);
FUNCTION_PTR_H (void, __stdcall, FreeString, String *str);

void appendThemeInPlace (char *name);
char *appendTheme (const char *name);
void appendStringInPlace (String *str, const char *append);
void appendThemeInPlaceString (String *name);
InputType getInputType ();
bool isMovieOnly (u64 entry);
u64 getPvDbEntry (i32 id);
Vec4 getPlaceholderRect (float *placeholderData);
// This will automatically call freeSublayerData if there is any data present in out already
void initSubLayerData (List<void> *out);
void freeSubLayerData (List<void> *out);
Vec2 getClickedPos (void *inputState);
