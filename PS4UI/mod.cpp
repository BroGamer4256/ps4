#include <algorithm>
#include <iostream>

#include <MinHook.h>
#include <isteamfriends.h>
#include <Sig/Sig.hpp>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <Psapi.h>


// Define types
using TypePrintURL = unsigned char(__stdcall*)(void*);
using TypePrintOpenDialog = unsigned char(__stdcall*)();
using TypeChangeSubGameState = void(__stdcall*)(int, int);
using TypeGalleryLoop = size_t(__fastcall*)(int*);
using TypeToggleTextBox = void(__fastcall*)(size_t obj, int id);


// Store original addresses
static TypePrintURL original_print_url = nullptr;
static TypePrintOpenDialog original_print_open_dialog = nullptr;
static TypeChangeSubGameState original_change_sub_game_state = nullptr;
static TypeGalleryLoop original_gallery_loop = nullptr;
static TypeToggleTextBox toggle_text_box = nullptr;


// Store globals
constexpr const char* store_url = "http://programming-motherfucker.com/become.html#C%20/%20C++";
constexpr const char* credits_url = "https://learncodethehardway.org/c/";
constexpr const char* pattern_text_box_toggle = "48 89 6C 24 18 56 48 83 EC 20 8B 81 0C 3F 00 00";
constexpr const char* pattern_print_url = "48 83 EC 28 48 83 79 18 10 72 03 48 8B 09 48 8B D1";
constexpr const char* pattern_open_dialog = "48 83 EC 28 48 8D 0D ?? ?? ?? ?? E8 ?? ?? ?? ?? 33 C0 48 83 C4 28 C3 CC CC CC CC CC CC CC CC CC";
constexpr const char* pattern_game_state = "48 89 5C 24 08 48 89 74 24 10 57 48 83 EC 20 89 D6";
constexpr const char* pattern_gallery_loop = "48 89 5C 24 10 48 89 6C 24 18 48 89 74 24 20 57 48 83 EC 40 8B 41 68 48 8B F1 FF C8 83 F8 0D";
constexpr const char* pattern_toggle_textbox_call1 = "E8 ?? ?? ?? ?? E9 F9 04 00 00 48 8D 4E 70 E8";
constexpr const char* pattern_toggle_textbox_call2 = "E8 ?? ?? ?? ?? E8 ?? ?? ?? ?? F3 0F 10 0D ?? ?? ?? ?? 45 33 C0";
constexpr const char* pattern_toggle_textbox_call3 = "E8 ?? ?? ?? ?? 80 BE 98 45 00 00 01 76 0A E8 ?? ?? ?? ?? E8 ?? ?? ?? ?? 48 8D 4E 70 C7 46 68 05 00 00 00";
constexpr const char* pattern_ui = "38 05 ?? ?? ?? ?? 74 05 48 83 C4 28 C3 B0 01 48 83 C4 28 C3 CC CC CC CC CC CC CC CC CC CC CC CC";


// Define common memory writing functions
namespace megamixplus {
    void write_memory(void* address, void* data, const int& size, const int& offset) {
        void* addr = (void*)((size_t)address + offset);
        DWORD old_protection;
        VirtualProtect(addr, size, PAGE_EXECUTE_READWRITE, &old_protection);
        memcpy(addr, data, size);
        VirtualProtect(addr, size, old_protection, &old_protection);
    }

    template<typename T> inline const void write(void* address, const std::initializer_list<T>& data, const int& offset = 0) {
        write_memory(address, (void*)data.begin(), (int)(data.size() * sizeof(T)), offset);
    }
    template<typename T> inline const void write(void* address, const T* data, const int& size, const int& offset = 0) {
        write_memory(address, (void*)data, size * sizeof(T), offset);
    }
    template<typename T> inline const void write_scalar(void* address, T data, const int& offset = 0) {
        write_memory(address, &data, sizeof(T), offset);
    }

    const MODULEINFO& module_info() {
        static MODULEINFO divamegamixplus;
        if (divamegamixplus.SizeOfImage) { return divamegamixplus; }

        ZeroMemory(&divamegamixplus, sizeof(MODULEINFO));
        GetModuleInformation(GetCurrentProcess(), GetModuleHandle(nullptr), &divamegamixplus, sizeof(MODULEINFO));

        return divamegamixplus;
    }
}


// Implementations of hooked functions
unsigned char __stdcall print_url(void* res) {
    // a1 here is a string*
    // From credits and manual
    auto flag = k_EActivateGameOverlayToWebPageMode_Modal;
    SteamFriends()->ActivateGameOverlayToWebPage(credits_url, flag);
    return 0;
}
unsigned char __stdcall print_open_dialog() {
    // From store button
    auto flag = k_EActivateGameOverlayToWebPageMode_Modal;
    SteamFriends()->ActivateGameOverlayToWebPage(store_url, flag);
    return 0;
}
void __stdcall change_sub_game_state(int state, int subState) {
    if (state == 9) { state = 6; } // MAINMENU_SWITCH -> CS_MENU
    if (state == 10) { state = 2; } // GAME_SWITCH -> GAME

    return original_change_sub_game_state(state, subState);
}
size_t __fastcall gallery_loop(int* addr) {
    static int previous = 4;

    if (addr[26] == 3 && previous != addr[28]) {
        toggle_text_box(addr[292], previous);
        toggle_text_box(addr[292], addr[28]);
        previous = addr[28];

    } else if (addr[26] == 6) {
        addr[27] = 1;
        addr[4454] = 5;
        addr[26] = 14;
        previous = 4;

    } else if (addr[26] == 4) {
        previous = addr[28] + 1;
        if (previous == 5) { previous = 3; }

    }

    return original_gallery_loop(addr);
}


extern "C" __declspec(dllexport) void pre_init() {
    const MODULEINFO& divamegamix = megamixplus::module_info();

    // 1.00 => 140CC5108
    // 1.01 => 140CBF190
    // 1.02 => 140CBE1F0
    const void* address_pv_db_switch0 = Sig::find<Sig::StrEq<"PV_DB_SWITCH">>(divamegamix.lpBaseOfDll, divamegamix.SizeOfImage);
    // 1.00 => 140CC50F0
    // 1.01 => 140CBF178
    // 1.02 => 140CBE200
    const void* address_pv_db_switch1 = Sig::find<Sig::StrEq<"pv_db_switch.txt">>(divamegamix.lpBaseOfDll, divamegamix.SizeOfImage);

    megamixplus::write_scalar<unsigned char>((void*)address_pv_db_switch0, 0);
    megamixplus::write_scalar<unsigned char>((void*)address_pv_db_switch1, 0);
}

// Main entry-point of the mod
extern "C" __declspec(dllexport) void init() {
    const MODULEINFO& divamegamix = megamixplus::module_info();

    // 1.00 => 1401ACA10
    // 1.01 => 1401ACBC0
    // 1.02 => 1401ACAD0
    toggle_text_box = (TypeToggleTextBox)Sig::find(divamegamix.lpBaseOfDll, divamegamix.SizeOfImage, pattern_text_box_toggle);
    // 1.00 => 1401DE890
    // 1.01 => 1401DEB00
    // 1.02 => 1401DE9F0
    const void* address_print_url = (TypePrintURL)Sig::find(divamegamix.lpBaseOfDll, divamegamix.SizeOfImage, pattern_print_url);
    // 1.00 => 1401DE8C0
    // 1.01 => 1401DEB30
    // 1.02 => 1401DEA20
    const void* address_open_dialog = (TypePrintOpenDialog)Sig::find(divamegamix.lpBaseOfDll, divamegamix.SizeOfImage, pattern_open_dialog);
    // 1.00 => 15241DA80
    // 1.01 => 152082F60
    // 1.02 => 152C49DD0
    const void* address_change_sub_game_state = (TypeChangeSubGameState)Sig::find(divamegamix.lpBaseOfDll, divamegamix.SizeOfImage, pattern_game_state);
    // 1.00 => 1401AD4D0
    // 1.01 => 1401AD680
    // 1.02 => 1401AD590
    const void* address_gallery_loop = (TypeGalleryLoop)Sig::find(divamegamix.lpBaseOfDll, divamegamix.SizeOfImage, pattern_gallery_loop);
    // 1.00 => 1401AD58C
    // 1.01 => 1401AD73C
    // 1.02 => 1401AD64C
    const void* address_toggle_textbox_call1 = Sig::find(divamegamix.lpBaseOfDll, divamegamix.SizeOfImage, pattern_toggle_textbox_call1);
    // 1.00 => 1401AD653
    // 1.01 => 1401AD803
    // 1.02 => 1401AD713
    const void* address_toggle_textbox_call2 = Sig::find(divamegamix.lpBaseOfDll, divamegamix.SizeOfImage, pattern_toggle_textbox_call2);
    // 1.00 => 1401AD799
    // 1.01 => 1401AD949
    // 1.02 => 1401AD859
    const void* address_toggle_textbox_call3 = Sig::find(divamegamix.lpBaseOfDll, divamegamix.SizeOfImage, pattern_toggle_textbox_call3);
    // 1.00 => 1414B28A3
    // 1.01 => 1414AC893
    // 1.02 => 1414AB9E3
    const void* address_ui = Sig::find(divamegamix.lpBaseOfDll, divamegamix.SizeOfImage, pattern_ui);


    bool is_any_null = address_pv_db_switch0 == nullptr ||
        address_pv_db_switch1 == nullptr ||
        toggle_text_box == nullptr ||
        address_print_url == nullptr ||
        address_open_dialog == nullptr ||
        address_change_sub_game_state == nullptr ||
        address_gallery_loop == nullptr ||
        address_toggle_textbox_call1 == nullptr ||
        address_toggle_textbox_call2 == nullptr ||
        address_toggle_textbox_call3 == nullptr ||
        address_ui == nullptr;

    if (is_any_null) {
        if (MessageBoxA(nullptr, "Something went wrong. Exit the game?", "PS4 UI", MB_OKCANCEL) == IDCANCEL) {
            std::exit(0);
        }
        return;
    }

    // Resolve address by retrieving the offset and adding it to the address where the data is accessed.
    int* offset = (int*)((size_t)address_ui + 2);
    address_ui = (void*)((size_t)address_ui + *offset + 6);

    // 1.00 Samyuu, 1.02 BroGamer <-- Scrubs. Jay now has ONE solution for all versions.
    // Thanks for the signatures Jay, you are the best :)
    megamixplus::write_scalar<unsigned char>((void*)address_ui, 1);

    // Stop calls to ToggleTextBox
    unsigned char nops[] = { 0x90, 0x90, 0x90, 0x90, 0x90 };
    megamixplus::write<unsigned char>((void*)address_toggle_textbox_call1, nops, sizeof(nops));
    megamixplus::write<unsigned char>((void*)address_toggle_textbox_call2, nops, sizeof(nops));
    megamixplus::write<unsigned char>((void*)address_toggle_textbox_call3, nops, sizeof(nops));

    MH_Initialize();
    MH_CreateHook((void*)address_print_url, (void*)print_url, (void**)&original_print_url);
    MH_CreateHook((void*)address_open_dialog, (void*)print_open_dialog, (void**)&original_print_open_dialog);
    MH_CreateHook((void*)address_change_sub_game_state, (void*)change_sub_game_state, (void**)&original_change_sub_game_state);
    MH_CreateHook((void*)address_gallery_loop, (void*)gallery_loop, (void**)&original_gallery_loop);

    MH_EnableHook((void*)address_print_url);
    MH_EnableHook((void*)address_open_dialog);
    MH_EnableHook((void*)address_change_sub_game_state);
    MH_EnableHook((void*)address_gallery_loop);
}
