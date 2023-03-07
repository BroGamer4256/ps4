namespace diva {
struct string {
	union {
		char data[16];
		char *ptr;
	};
	u64 length;
	u64 capacity;

	char *c_str () {
		if (this->capacity > 16) return this->ptr;
		else return this->data;
	}
};

template <typename T>
struct listElement {
	listElement<T> *next;
	listElement<T> *previous;
	T current;

	bool operator== (const listElement<T> &rhs) { return this->current == rhs->current; }
	bool operator== (const T &rhs) { return this->current == rhs; }
};

template <typename T>
struct list {
	listElement<T> *root;
	u64 length;

	listElement<T> *begin () { return this->length ? this->root->next : this->root; }
	listElement<T> *end () { return this->root; }
};

template <typename K, typename V>
struct mapElement {
	mapElement<K, V> *left;
	mapElement<K, V> *parent;
	mapElement<K, V> *right;
	bool isBlack;
	bool isNull;
	alignas (8) K key;
	V value;

	mapElement<K, V> *next () {
		auto ptr = this;
		if (ptr->right->isNull) {
			while (!ptr->parent->isNull && ptr == ptr->parent->right)
				ptr = ptr->parent;
			ptr = ptr->parent;
		} else {
			ptr = ptr->right;
			while (!ptr->left->isNull)
				ptr = ptr->left;
		}
		return ptr;
	}
};

template <typename K, typename V>
struct map {
	mapElement<K, V> *root;
	u64 length;

	map () {
		this->root          = (mapElement<K, V> *)calloc (1, sizeof (mapElement<K, V>));
		this->root->left    = this->root;
		this->root->parent  = this->root;
		this->root->right   = this->root;
		this->root->isBlack = true;
		this->root->isNull  = true;
		this->length        = 0;
	}

	mapElement<K, V> *find (K key) {
		auto ptr = this->root->parent;
		while (!ptr->isNull) {
			if (key == ptr->key) return ptr;
			if (key > ptr->key) ptr = ptr->right;
			if (key < ptr->key) ptr = ptr->left;
		}
		return this->end ();
	}

	mapElement<K, V> *begin () { return this->length ? this->root->left : this->root; }
	mapElement<K, V> *end () { return this->root; }
};

enum class State : i32 {
	STARTUP     = 0,
	ADVERTISE   = 1,
	GAME        = 2,
	DATA_TEST   = 3,
	TEST_MODE   = 4,
	APP_ERROR   = 5,
	CS_MENU     = 6,
	CUSTOMIZE   = 7,
	GALLERY     = 8,
	MENU_SWITCH = 9,
	GAME_SWITCH = 10,
	TSHIRT_EDIT = 11,
	MAX         = 12,
};

enum class SubState : i32 {
	DATA_INITIALIZE           = 0,
	SYSTEM_STARTUP            = 1,
	LOGO                      = 2,
	TITLE                     = 3,
	CONCEAL                   = 4,
	PV_SEL                    = 5,
	PLAYLIST_SEL              = 6,
	GAME                      = 7,
	DATA_TEST_MAIN            = 8,
	DATA_TEST_MISC            = 9,
	DATA_TEST_OBJ             = 10,
	DATA_TEST_STG             = 11,
	DATA_TEST_MOT             = 12,
	DATA_TEST_COLLISION       = 13,
	DATA_TEST_SPR             = 14,
	DATA_TEST_AET             = 15,
	DATA_TEST_AUTH3D          = 16,
	DATA_TEST_CHR             = 17,
	DATA_TEST_ITEM            = 18,
	DATA_TEST_PERF            = 19,
	DATA_TEST_PVSCRIPT        = 20,
	DATA_TEST_PRINT           = 21,
	DATA_TEST_CARD            = 22,
	DATA_TEST_OPD             = 23,
	DATA_TEST_SLIDER          = 24,
	DATA_TEST_GLITTER         = 25,
	DATA_TEST_GRAPHICS        = 26,
	DATA_TEST_COLLECTION_CARD = 27,
	DATA_TEST_PAD             = 28,
	TEST_MODE                 = 29,
	APP_ERROR                 = 30,
	UNK_31                    = 31,
	CS_MENU                   = 32,
	CS_COMMERCE               = 33,
	CS_OPTION_MENU            = 34,
	CS_UNK_TUTORIAL_35        = 35,
	CS_CUSTOMIZE_SEL          = 36,
	CS_UNK_TUTORIAL_37        = 37,
	CS_GALLERY                = 38,
	UNK_39                    = 39,
	UNK_40                    = 40,
	UNK_41                    = 41,
	MENU_SWITCH               = 42,
	UNK_43                    = 43,
	OPTION_MENU_SWITCH        = 44,
	UNK_45                    = 45,
	UNK_46                    = 46,
	MAX                       = 47,
};

enum class InputType : i32 {
	XBOX        = 0,
	PLAYSTATION = 1,
	SWITCH      = 2,
	STEAM       = 3,
	KEYBOARD    = 4,
	UNKNOWN     = 5,
};

enum class AetAction : i32 {
	NONE         = 0, // Ignore all markers, just play through it all
	IN_ONCE      = 1, // Start at st_in, end at ed_in
	IN_LOOP      = 2, // Start at st_in, play to ed_lp then jump to st_lp and keep looping
	LOOP         = 3, // Start at st_lp, end at ed_lp, loops
	OUT_ONCE     = 4, // Start at st_out, end at ed_out
	SPECIAL_ONCE = 5, // Start at st_sp, end to ed_lp
	SPECIAL_LOOP = 6, // Start at st_sp, play to ed_sp, then jump to st_lp and loop through ed_lp
	UNK          = 7, // Start at st_in, end at ed_in, probably loops?
};

enum class Button : i32 {
	UP       = 3,
	DOWN     = 4,
	LEFT     = 5,
	RIGHT    = 6,
	TRIANGLE = 7,
	SQUARE   = 8,
	BACK     = 9,
	ACCEPT   = 10,
	L1       = 11,
	R1       = 12,
	L2       = 13,
	R2       = 14,
	L3       = 15,
	R3       = 16,
};

struct PvSpriteIds {
	void *unk;
	i32 setId;
	i32 bgIdEasy;
	i32 bgIdNormal;
	i32 bgIdHard;
	i32 bgIdExtreme;
	i32 jkIdEasy;
	i32 jkIdNormal;
	i32 jkIdHard;
	i32 jkIdExtreme;
	i32 logoIdEasy;
	i32 logoIdNormal;
	i32 logoIdHard;
	i32 logoIdExtreme;
	i32 thumbnailIdEasy;
	i32 thumbnailIdNormal;
	i32 thumbnailIdHard;
	i32 thumbnailIdExtreme;
};

extern list<i32> *pvs;
extern map<i32, PvSpriteIds> *pvSprites;
} // namespace diva

FUNCTION_PTR_H (bool, __thiscall, CmnMenuDestroy, u64 This);
FUNCTION_PTR_H (void *, __stdcall, DivaGetInputState, i32 a1);
FUNCTION_PTR_H (bool, __stdcall, IsButtonTapped, void *state, diva::Button button);
FUNCTION_PTR_H (void *, __stdcall, CreateAetLayerData, void *data, i32 aetSceneId, const char *layerName, i32 layer, diva::AetAction action);
FUNCTION_PTR_H (i32, __stdcall, PlayAetLayer, void *data, i32 id);
FUNCTION_PTR_H (void, __stdcall, GetComposition, diva::map<diva::string, void *> *composition, i32 id);
FUNCTION_PTR_H (float *, __stdcall, GetCompositionLayer, diva::map<diva::string, void *> *composition, const char *layerName);
FUNCTION_PTR_H (void, __stdcall, ApplyLocation, void *data, Vec3 *locationData);
FUNCTION_PTR_H (void, __stdcall, PlaySoundEffect, const char *name, float volume);
FUNCTION_PTR_H (u64, __stdcall, GetPvLoadData);
FUNCTION_PTR_H (i32, __stdcall, GetCurrentStyle);
FUNCTION_PTR_H (diva::InputType, __stdcall, NormalizeInputType, i32 inputType);
FUNCTION_PTR_H (diva::string *, __stdcall, StringInit, diva::string *to, const char *from, u64 len);
FUNCTION_PTR_H (void, __stdcall, StopAet, i32 *id);

void appendThemeInPlace (char *name);
char *appendTheme (const char *name);
void appendStringInPlace (diva::string *str, const char *append);
void appendThemeInPlaceString (diva::string *name);
diva::InputType getInputType ();
bool isMovieOnly (u64 entry);
u64 getPvDbEntry (i32 id);
Vec4 getPlaceholderRect (float *placeholderData, bool centeredAnchor);
void initCompositionData (diva::map<diva::string, void *> *out);
Vec2 getClickedPos (void *inputState);
std::optional<Vec4> getTouchArea (diva::map<diva::string, void *> compositionData, const char *name, bool centeredAnchor);
