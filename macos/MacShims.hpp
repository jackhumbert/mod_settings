#pragma once
// macOS only. What the Windows sources get from Win32 (module handles, paths, wide strings, a logger that can pop a message box,
// a retry loop around Attach), written for the Mac: dlopen(RTLD_NOLOAD)/dlsym, dladdr, UTF-8, log lines only, one attach attempt.
// Everything is in namespace ModSettingsMac so nothing here can collide with, or be mistaken for, the Windows spelling.

#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <string>

#include <RED4ext/Api/PluginHandle.hpp>
#include <RED4ext/Api/Sdk.hpp>

namespace ModSettingsMac
{
// Called by Main (Load) with what RED4ext hands the plugin. Until then logging goes to stderr only.
void Init(const RED4ext::Sdk* aSdk, RED4ext::PluginHandle aHandle);
const RED4ext::Sdk* Sdk();
RED4ext::PluginHandle Handle();

// ---- logging: stderr and the RED4ext logger, UTF-8, never a dialog -------------------------------------------------------------
void Log(const char* aFormat, ...) __attribute__((format(printf, 1, 2)));
void LogWarn(const char* aFormat, ...) __attribute__((format(printf, 1, 2)));
void LogError(const char* aFormat, ...) __attribute__((format(printf, 1, 2)));
// One line per key for the life of the process (a fail-safe placeholder that is hit every frame must not flood the log).
void LogOnce(const char* aKey, const char* aFormat, ...) __attribute__((format(printf, 2, 3)));
// MODSETTINGS_MAC_DIAG=1 in the environment. Read once.
bool DiagEnabled();

// ---- modules and paths (GetModuleHandle / GetModuleFileName / GetProcAddress) -----------------------------------------------------
// dladdr on this dylib: ".../red4ext/plugins/mod_settings/mod_settings.dylib".
std::filesystem::path ModulePath();
// Its folder. user.ini lives here, as on Windows.
std::filesystem::path PluginDir();
// The game root: PluginDir/../../.. (the folder with red4ext/, r6/, archive/ and Cyberpunk2077.app). Windows derives the same folder
// from the exe path (bin/x64/Cyberpunk2077.exe).
std::filesystem::path GameRoot();
// The Mach-O header of the game executable (MH_EXECUTE). Hook offsets are relative to it. Not _dyld_get_image_header(0): under
// DYLD_INSERT_LIBRARIES image 0 is an inserted library.
uintptr_t GameBase();
// GetModuleHandle: a handle for an image that is already loaded (dlopen RTLD_NOLOAD), nullptr when it is not. Never loads anything.
void* LoadedModule(const char* aPath);
// GetProcAddress.
void* Symbol(void* aModule, const char* aName);
// The symbol from any loaded image (RTLD_DEFAULT).
void* AnySymbol(const char* aName);

// ---- memory reads that cannot fault: vm_read_overwrite copies the bytes or reports failure. Used for every read of game memory whose layout
// was not checked field by field (a bundle that is not the game's, the back pointer of a UserMapping).
bool SafeCopy(const void* aSrc, void* aDst, std::size_t aSize);
template<typename T>
bool SafeRead(const void* aSrc, T& aOut)
{
    return SafeCopy(aSrc, &aOut, sizeof(T));
}

// ---- wide strings: the Mac wchar_t is UTF-32; text that goes to a log or a file is UTF-8 ------------------------------------------
std::string WideToUtf8(const wchar_t* aText);
std::wstring Utf8ToWide(const char* aText);

// ---- hooks ---------------------------------------------------------------------------------------------------------------------------
// ONE attach attempt, one log line with the result. No retry loop (the Windows ModModuleHook loops until Attach succeeds).
//   aSharedWithCodeware: the target is also hooked by Codeware (R22: RED4ext allows one hook per target, so Codeware and ArchiveXL
//   chain on one hook through <plugin>_mac_hook_chain_attach). The hook is attached from modsettings_registerNatives, after every
//   plugin's Main, so Codeware has installed its hook by then (it does it in its Main, ScriptingService::OnBootstrap):
//     * codeware_mac_hook_chain_attach exists and accepts the target  -> CHAINED;
//     * it exists and refuses the target (Codeware does not own it)  -> REFUSED, logged, not attached directly;
//     * the symbol does not exist (Codeware is not loaded)           -> a direct RED4ext Attach.
enum class HookResult
{
    Attached,  // RED4ext attached the detour; Detach is possible
    Chained,   // joined another plugin's hook chain; there is no Detach
    NotActive, // Attach reported success but the offset is not on the RED4ext allowlist (the detour never runs)
    Refused,   // not attempted (see the log line)
    Failed,    // RED4ext refused
};
HookResult AttachHook(const char* aName, uintptr_t aAddress, void* aDetour, void** aOriginal, bool aSharedWithCodeware);
void DetachHook(const char* aName, uintptr_t aAddress, HookResult aHow);
const char* HookResultName(HookResult aResult);

// True when the game's PoolStorageProxy functions were found where this build expects them (MacVault.cpp, checked on first use).
bool VaultReady();
// R42c. Allocates and frees 64 bytes in every vault pool (one log line each); false if any pool failed.
bool VaultSelfTest();

// True for the address of a trap stub (macos/MacTrap.cpp): the hash has no Mac address, there is nothing to hook.
bool IsTrapAddress(uintptr_t aAddress);
} // namespace ModSettingsMac
