namespace diva {
struct Vec2 {
	float x;
	float y;

	inline Vec2 () {
		this->x = 0;
		this->y = 0;
	}

	inline Vec2 (float x, float y) {
		this->x = x;
		this->y = y;
	}
};

struct Vec3 {
	float x;
	float y;
	float z;

	inline Vec3 () {
		this->x = 0;
		this->y = 0;
		this->z = 0;
	}

	inline Vec3 (float x, float y, float z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}
};

struct Vec4 {
	float x;
	float y;
	float z;
	float w;

	inline Vec4 () {
		this->x = 0;
		this->y = 0;
		this->z = 0;
		this->w = 0;
	}

	inline Vec4 (float x, float y, float z, float w) {
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}

	inline bool contains (Vec2 location) { return location.x > this->x && location.x < this->y && location.y > this->z && location.y < this->w; }
};

FUNCTION_PTR_H (void *, __fastcall, operatorNew, u64);
FUNCTION_PTR_H (void *, __fastcall, operatorDelete, void *);
template <typename T>
T *
allocate (u64 count) {
	return (T *)(operatorNew (count * sizeof (T)));
}

inline void
deallocate (void *p) {
	operatorDelete (p);
}

#pragma pack(push, 8)
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

template <typename T>
struct vector {
	T *first;
	T *last;
	void *capacity_end;

	vector () {}
	vector (u64 n) {
		this->first        = allocate<T> (n);
		this->last         = this->first;
		this->capacity_end = (void *)((u64)this->first + (n * sizeof (T)));
	}

	~vector () { deallocate (this->first); }

	std::optional<T *> at (u64 index) {
		if (index >= this->length ()) return std::nullopt;
		std::optional (&this->first[index]);
	}

	void push_back (T value) {
		if (this->remaining_capcity () > 0) {
			this->first[this->length ()] = value;
			this->last++;
			return;
		}

		u64 new_length = this->length () + (this->length () / 2);
		T *new_first   = allocate<T> (new_length);
		u64 old_length = (u64)this->last - (u64)this->first;
		memcpy (new_first, this->first, old_length);
		deallocate (this->first);

		this->first        = new_first;
		this->last         = (T *)((u64)new_first + old_length);
		this->capacity_end = (void *)((u64)new_first + (new_length * sizeof (T)));
		this->push_back (value);
	}

	u64 length () { return ((u64)this->last - (u64)this->first) / sizeof (T); }
	u64 capacity () { return ((u64)this->capacity_end - (u64)this->first) / sizeof (T); }
	u64 remaining_capcity () { return this->capacity () - this->length (); }

	T *begin () { return this->first; }
	T *end () { return this->last; }
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

struct aetLayer {
	int sceneId;
	char *layerName;
	string StartMarker;
	string EndMarker;
	string LoopMarker;
	float start_time;
	float end_time;
	int flags;
	int unk_0x7C;
	int unk_0x80;
	int layer;
	int resolutionMode;
	Vec3 position;
	Vec3 rotation;
	Vec3 scale;
	Vec3 anchor;
	float frameSpeed;
	Vec4 color;
	map<string, i32> layerSprite;
	string sound_path;
	map<string, string> soundReplace;
	int soundQueueIndex;
	map<u32, u32> spriteReplace;
	map<u32, void *> spriteTexture;
	map<u32, u32> spriteDiscard;
	void *frameRateControl;
	bool soundVoice;
	i32 unk_0x154;
	i32 unk_0x158;
	i32 unk_0x15C;
	void *unk_0x160;
	Vec3 position_2;

	aetLayer () {}
	aetLayer (i32 sceneId, const char *layerName, i32 layer, AetAction action);
	void with_data (i32 sceneId, const char *layerName, i32 layer, AetAction action);
	void play (i32 *id);
	void setPosition (Vec3 position);
};
#pragma pack(pop)

extern list<i32> *pvs;
extern map<i32, PvSpriteIds> *pvSprites;

using compositionData = map<string, void *>;

FUNCTION_PTR_H (bool, __thiscall, CmnMenuDestroy, u64 This);
FUNCTION_PTR_H (void *, __stdcall, GetInputState, i32 a1);
FUNCTION_PTR_H (bool, __stdcall, IsButtonTapped, void *state, Button button);
FUNCTION_PTR_H (void, __stdcall, GetComposition, compositionData *composition, i32 id);
FUNCTION_PTR_H (float *, __stdcall, GetCompositionLayer, compositionData *composition, const char *layerName);
FUNCTION_PTR_H (void, __stdcall, PlaySoundEffect, const char *name, float volume);
FUNCTION_PTR_H (u64, __stdcall, GetPvLoadData);
FUNCTION_PTR_H (i32, __stdcall, GetCurrentStyle);
FUNCTION_PTR_H (InputType, __stdcall, NormalizeInputType, i32 inputType);
FUNCTION_PTR_H (string *, __stdcall, StringInit, string *to, const char *from, u64 len);
FUNCTION_PTR_H (void, __stdcall, StopAet, i32 *id);

void appendThemeInPlace (char *name);
char *appendTheme (const char *name);
void appendStringInPlace (string *str, const char *append);
void appendThemeInPlaceString (string *name);
InputType getInputType ();
bool isMovieOnly (u64 entry);
u64 getPvDbEntry (i32 id);
Vec4 getPlaceholderRect (float *placeholderData, bool centeredAnchor);
void initCompositionData (compositionData *out);
Vec2 getClickedPos (void *inputState);
std::optional<Vec4> getTouchArea (compositionData compositionData, const char *name, bool centeredAnchor);
} // namespace diva
