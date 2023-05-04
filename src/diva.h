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

FUNCTION_PTR_H (void *, operatorNew, u64);
FUNCTION_PTR_H (void *, operatorDelete, void *);
struct string;
FUNCTION_PTR_H (void, FreeString, string *);
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
		if (this->capacity > 15) return this->ptr;
		else return this->data;
	}

	string () {}
	string (const char *cstr) {
		u64 len = strlen (cstr);
		if (len > 15) {
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
		if (len > 15) {
			u64 new_len = (len + 1) | 0xF;
			this->ptr   = allocate<char> (new_len);
			strcpy (this->ptr, cstr);
			this->length   = len;
			this->capacity = len;
		} else {
			strcpy_s (this->data, cstr);
			this->length   = len;
			this->capacity = 15;
		}
	}

	~string () { FreeString (this); }

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

	std::optional<V *> find (K key) {
		auto ptr = this->root->parent;
		while (!ptr->isNull) {
			if (key == ptr->key) return std::optional (&ptr->value);
			if (key > ptr->key) ptr = ptr->right;
			if (key < ptr->key) ptr = ptr->left;
		}
		return std::nullopt;
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

template <typename T>
struct _stringRangeBase {
	T *data;
	T *end;

	// Technically incorrect but always seems to work
	T *c_str () { return data; }

	_stringRangeBase () {
		data = 0;
		end  = 0;
	}
	_stringRangeBase (const T *str);
	_stringRangeBase (const T *str, size_t length) {
		data = allocate<T> (length);
		end  = data + length;
		memcpy (data, str, length);
	}
};
using stringRange  = _stringRangeBase<char>;
using wstringRange = _stringRangeBase<wchar_t>;

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

struct AetLayerArgs {
	i32 sceneId;
	char *layerName;
	string StartMarker;
	string EndMarker;
	string LoopMarker;
	f32 startTime;
	f32 endTime;
	i32 flags;
	i32 unk_0x7C;
	i32 unk_0x80;
	i32 priority;
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
	i32 id;
	i32 id2;
	i32 unk_0x164;
	Vec3 position_2;

	AetLayerArgs () {}
	AetLayerArgs (const char *sceneName, const char *layerName, i32 priority, AetAction action);
	void create (const char *sceneName, const char *layerName, i32 priority, AetAction action);
	void play (i32 *id);
	void setPosition (Vec3 position);
};

struct AetLayerData {
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
	f32 opacity;
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

struct AetLayer {
	char *name;
	f32 startTime;
	f32 endTime;
	f32 offsetTime;
	f32 timeScale;
	i32 flags;
	void *unk_0x30;
	u64 unk_0x38;
	u32 markerCount;
	void *markers;
	void *video;
};

struct AetData {
	void *vftable;
	void *scene;
	void *composition;
	AetLayer *layer;
	f32 startTime;
	f32 endTime;
	i32 flags;
	i32 layerNo;
	i32 unk_0x30;
	f32 loopStart;
	f32 loopEnd;
	i32 unk_0x3C;
	i32 unk_0x40;
	i32 resolutionMode;
	Vec3 position;
	Vec3 rotation;
	Vec3 scale;
	Vec3 anchor;
	f32 frameSpeed;
	Vec4 color;
	f32 currentFrame;
};

template <typename T>
struct PvDbIndexedValue {
	i32 index;
	T value;
};

template <typename T>
struct PvDbIdValue {
	i32 index;
	i32 id;
	T value;
};

struct PvDbPlaceholder {};

struct PvDbExInfo {
	string key;
	string value;
};

struct PvDbDifficulty {
	i32 difficulty;
	i32 edition;
	i32 isExtra;
	// Theres something at +0x10 but I cannot figure it out and I don't need that info right now
	i32 unk[5];
	string scriptFile;
	i32 level; // Stars * 2;
	string buttonSoundEffect;
	string successSoundEffect;
	string slideSoundEffect;
	string slideChainStartSoundEffect;
	string slideChainSoundEffect;
	string slideChainSuccessSoundEffect;
	string slideChainFailureSoundEffect;
	string slideTouchSoundEffect;
	vector<PvDbIdValue<string>> motion[6];
	vector<PvDbPlaceholder> field;
	bool exStage;
	vector<PvDbIndexedValue<string>> items;
	vector<PvDbIdValue<string>> handItems;
	vector<PvDbIndexedValue<string>> editEffects;
	vector<PvDbPlaceholder> unk_240;
	vector<PvDbPlaceholder> unk_258;
	vector<PvDbPlaceholder> unk_270;
	f32 unk_288;
	f32 unk_28C;
	string unk_290;
	string music;
	string illustrator;
	string arranger;
	string manipulator;
	string editor;
	string guitar;
	PvDbExInfo exInfo[4];
	string unk_470;
	vector<PvDbIndexedValue<string>> movies;
	i32 movieSurface;
	bool isMovieOnly;
	string effectSoundEffect;
	vector<string> effectSoundEffectList;
	i32 version;
	i32 scriptFormat;
	i32 highSpeedRate;
	f32 hiddenTiming;
	f32 suddenTiming;
	bool editCharaScale;
	string unk_0x500;
	u64 unk_0x520;
	u64 unk_0x528;

	~PvDbDifficulty () = delete;
};

struct PvDbEntry {
	i32 id;
	i32 date;
	string name;
	string nameReading;
	i32 unk_48;
	i32 bpm;
	string soundFile;
	vector<PvDbIndexedValue<string>> lyrics;
	float sabiStartTime;
	float sabiPlayTime;
	u64 unk_90;
	vector<PvDbPlaceholder> performers;
	vector<PvDbDifficulty> easy;
	vector<PvDbDifficulty> normal;
	vector<PvDbDifficulty> hard;
	vector<PvDbDifficulty> extreme;
	vector<PvDbDifficulty> encore;
	// Theres more stuff here but I do not care

	~PvDbEntry () = delete;
};

struct AetDbSceneEntry {
	stringRange name;
	i32 id;
};

class TaskInterface {
public:
	virtual ~TaskInterface () {}
	virtual bool Init ()    = 0;
	virtual bool Loop ()    = 0;
	virtual bool Destroy () = 0;
	virtual bool Display () = 0;
	virtual bool Basic ()   = 0;
};

enum class TaskOp : i32 {
	NONE = 0,
	INIT,
	LOOP,
	DESTROY,
	MAX,
};

enum class TaskState : i32 {
	NONE = 0,
	RUNNING,
	SUSPENDED,
	HIDDEN,
};

enum class TaskRequest : i32 {
	NONE = 0,
	INIT,
	DESTROY,
	SUSPEND,
	HIDE,
	RUN,
};

struct Task : public TaskInterface {
	i32 priority;
	Task *parent;
	TaskOp op;
	TaskState state;
	TaskRequest request;
	TaskOp nextOp;
	TaskState nextState;
	bool unk;
	bool isFrameDependent;
	char name[32];
};

typedef bool (*taskFunction) (u64 Task);
struct taskAddition {
	std::optional<taskFunction> init;
	std::optional<taskFunction> loop;
	std::optional<taskFunction> destroy;
};
#pragma pack(pop)

extern vector<PvDbEntry *> *pvs;
extern map<i32, AetData> *aets;

using AetComposition = map<string, AetLayerData>;
template <>
AetComposition::~map ();

template <>
stringRange::_stringRangeBase (const char *str);
template <>
wstringRange::_stringRangeBase (const wchar_t *str);

FUNCTION_PTR_H (bool, CmnMenuDestroy, u64 This);
FUNCTION_PTR_H (void *, GetInputState, i32 a1);
FUNCTION_PTR_H (bool, IsButtonTapped, void *state, Button button);
FUNCTION_PTR_H (void, GetComposition, AetComposition *composition, i32 id);
FUNCTION_PTR_H (void, PlaySoundEffect, const char *name, float volume);
FUNCTION_PTR_H (u64, GetPvLoadData);
FUNCTION_PTR_H (i32, GetCurrentStyle);
FUNCTION_PTR_H (InputType, NormalizeInputType, i32 inputType);
FUNCTION_PTR_H (void, StopAet, i32 *id);
FUNCTION_PTR_H (void, GetFSCTRankData, i32 *fsRank, i32 *ctRank, i32 *fsPoints, i32 *ctPoints);
FUNCTION_PTR_H (bool, IsSurvival);
FUNCTION_PTR_H (bool, SurvivalCleared);
FUNCTION_PTR_H (i32, LifeGauge);

void appendThemeInPlace (char *name);
char *appendTheme (const char *name);
void appendStringInPlace (string *str, const char *append);
void appendThemeInPlaceString (string *name);
InputType getInputType ();
bool isMovieOnly (PvDbEntry *entry);
std::optional<PvDbEntry *> getPvDbEntry (i32 id);
Vec4 getPlaceholderRect (AetLayerData layer);
Vec2 getClickedPos (void *inputState);
void addTaskAddition (const char *name, taskAddition addition);
void init ();
} // namespace diva
