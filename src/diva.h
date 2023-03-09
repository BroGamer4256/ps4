namespace diva {
struct Vec2 {
	f32 x;
	f32 y;

	Vec2 () {
		this->x = 0;
		this->y = 0;
	}

	Vec2 (f32 x, f32 y) {
		this->x = x;
		this->y = y;
	}
};

struct Vec3 {
	f32 x;
	f32 y;
	f32 z;

	Vec3 () {
		this->x = 0;
		this->y = 0;
		this->z = 0;
	}

	Vec3 (f32 x, f32 y, f32 z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}
};

struct Vec4 {
	f32 x;
	f32 y;
	f32 z;
	f32 w;

	Vec4 () {
		this->x = 0;
		this->y = 0;
		this->z = 0;
		this->w = 0;
	}

	Vec4 (f32 x, f32 y, f32 z, f32 w) {
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}

	bool contains (Vec2 location) { return location.x > this->x && location.x < this->y && location.y > this->z && location.y < this->w; }
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

	string () {}
	string (const char *cstr) {
		u64 len = strlen (cstr);
		if (len > 16) {
			u64 new_len = (len + 1) | 0xF;
			this->ptr   = allocate<char> (new_len);
			strcpy (this->ptr, cstr);
			this->length   = len;
			this->capacity = new_len;
		} else {
			strcpy_s (this->data, cstr);
			this->length   = len;
			this->capacity = 15;
		}
	}
	string (char *cstr) {
		u64 len = strlen (cstr);
		if (len > 16) {
			this->ptr      = cstr;
			this->length   = len;
			this->capacity = len;
		} else {
			strcpy_s (this->data, cstr);
			this->length   = len;
			this->capacity = 15;
		}
	}

	~string () {
		FUNCTION_PTR_H (void, __stdcall, FreeString, string *);
		FreeString (this);
	}

	bool operator== (string &rhs) { return strcmp (this->c_str (), rhs.c_str ()) == 0; }
	bool operator== (char *rhs) { return strcmp (this->c_str (), rhs) == 0; }
	auto operator<=> (string &rhs) { return strcmp (this->c_str (), rhs.c_str ()); }
	auto operator<=> (char *rhs) { return strcmp (this->c_str (), rhs); }
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
		this->root          = allocate<mapElement<K, V>> (1);
		this->root->left    = this->root;
		this->root->parent  = this->root;
		this->root->right   = this->root;
		this->root->isBlack = true;
		this->root->isNull  = true;
		this->length        = 0;
	}

	~map () {
		for (auto it = this->begin (); it != this->end (); it = it->next ())
			deallocate (it);
		deallocate (this->root);
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

	std::optional<V> find_val (K key) {
		auto found = this->find (key);
		if (!found || found == this->end ()) return std::nullopt;
		return std::optional (found->value);
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

struct aetLayerArgs {
	i32 sceneId;
	char *layerName;
	string StartMarker;
	string EndMarker;
	string LoopMarker;
	f32 start_time;
	float end_time;
	i32 flags;
	i32 unk_0x7C;
	i32 unk_0x80;
	i32 layer;
	i32 resolutionMode;
	Vec3 position;
	Vec3 rotation;
	Vec3 scale;
	Vec3 anchor;
	f32 frameSpeed;
	Vec4 color;
	map<string, i32> layerSprite;
	string sound_path;
	map<string, string> soundReplace;
	i32 soundQueueIndex;
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

	aetLayerArgs () {}
	aetLayerArgs (i32 sceneId, const char *layerName, i32 layer, AetAction action);
	void with_data (i32 sceneId, const char *layerName, i32 layer, AetAction action);
	void play (i32 *id);
	void setPosition (Vec3 position);
};

struct aetLayerData {
	struct {
		Vec4 x;
		Vec4 y;
		Vec4 z;
		Vec4 w;
	} matrix;
	Vec3 position;
	Vec3 anchor;
	f32 width;
	f32 height;
	u32 unk_60;
	u32 unk_64;
	i32 resolutionMode;
	u32 unk_6C;
	i32 unk_70;
	u8 blendMode;
	u8 transferFlags;
	u8 trackMatte;
	i32 unk_78;
	i32 unk_7C;
};
#pragma pack(pop)

extern list<i32> *pvs;
extern map<i32, PvSpriteIds> *pvSprites;

using aetComposition = map<string, aetLayerData>;
template <>
aetComposition::~map ();

FUNCTION_PTR_H (bool, __thiscall, CmnMenuDestroy, u64 This);
FUNCTION_PTR_H (void *, __stdcall, GetInputState, i32 a1);
FUNCTION_PTR_H (bool, __stdcall, IsButtonTapped, void *state, Button button);
FUNCTION_PTR_H (void, __stdcall, GetComposition, aetComposition *composition, i32 id);
FUNCTION_PTR_H (void, __stdcall, PlaySoundEffect, const char *name, float volume);
FUNCTION_PTR_H (u64, __stdcall, GetPvLoadData);
FUNCTION_PTR_H (i32, __stdcall, GetCurrentStyle);
FUNCTION_PTR_H (InputType, __stdcall, NormalizeInputType, i32 inputType);
FUNCTION_PTR_H (void, __stdcall, StopAet, i32 *id);

void appendThemeInPlace (char *name);
char *appendTheme (const char *name);
void appendStringInPlace (string *str, const char *append);
void appendThemeInPlaceString (string *name);
InputType getInputType ();
bool isMovieOnly (u64 entry);
u64 getPvDbEntry (i32 id);
Vec4 getPlaceholderRect (aetLayerData layer);
Vec2 getClickedPos (void *inputState);
std::optional<Vec4> getTouchArea (aetComposition compositionData, const char *name);
} // namespace diva
