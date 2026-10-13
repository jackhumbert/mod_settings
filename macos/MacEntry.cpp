// macOS only. The plugin entry point that the Windows build does not need.
//
// Windows: Main(Load) registers the RedLib types (RTTI register callbacks the game runs later), the ArchiveXL archive, and attaches the
// hooks. On the Mac:
//   * the game never calls the RTTI register callbacks (Codeware says so in lib/Support/RedLib/RedLibProvider.cpp and drives its own
//     registration the same way), so queued RedLib types would never register;
//   * RED4ext-macos has no game-state or script hooks ("InitScripts / LoadScripts / CGameApplication hook failed" in every boot log), so
//     there is no RED4ext event after the plugins' Mains either;
//   * what does run after every plugin's Main is the launcher's NATREG step (the loader's Frida gadget script): at the entry of the
//     game's InitScripts (0x3D8C188) it calls <plugin>_registerNatives of TweakXL, ArchiveXL and Codeware, in that order, about two
//     seconds after the last Main (boot 2026-10-12 18:17: "5 plugin(s) loaded" 18:17:17.227, NATREG 18:17:19.303), and before LoadScripts.
// So Mod Settings exports modsettings_registerNatives, and the launcher must call it last (after codeware_registerNatives). It does,
// once: the RedLib types, the ArchiveXL archive, the hooks. The scripts are linked afterwards, and the ScriptBundle destructor (the
// ReleaseScriptData hook) runs after that.
#include "MacShims.hpp"

#include <atomic>

#include <ArchiveXL.hpp>
#include <CyberpunkMod.hpp>
#include <RedLib.hpp>

extern "C" __attribute__((visibility("default"))) void modsettings_registerNatives()
{
    static std::atomic<bool> once{false};
    if (once.exchange(true))
    {
        return;
    }
    const auto* sdk = ModSettingsMac::Sdk();
    const auto handle = ModSettingsMac::Handle();
    if (!sdk)
    {
        ModSettingsMac::LogError("modsettings_registerNatives was called before Main(Load): nothing registered");
        return;
    }
    if (!ModSettingsMac::VaultReady())
    {
        ModSettingsMac::LogError("the game's pool functions are not where this build expects them: nothing registered");
        return;
    }
    // R42c. Every pool the dylib allocates from must work before anything touches engine memory.
    if (!ModSettingsMac::VaultSelfTest())
    {
        ModSettingsMac::LogError("vault self-test failed (see the [vault] self-test lines): nothing registered");
        return;
    }

    const auto pendingRegister = Red::TypeInfoRegistrar::PendingRegisterCount();
    const auto pendingDescribe = Red::TypeInfoRegistrar::PendingDescribeCount();
    ModSettingsMac::Log("modsettings_registerNatives: %zu type registrations and %zu descriptions queued", pendingRegister, pendingDescribe);
    if (pendingRegister == 0)
    {
        ModSettingsMac::LogError("no RedLib type registrar was instantiated (the native classes ModSettings and ModConfigVar* will be missing)");
    }

    // 1. The native classes (ModSettings, ModConfigVar*, the inkActionName.ToName expansion).
    Red::TypeInfoRegistrar::RunPending();
    ModSettingsMac::Log("modsettings_registerNatives: types registered, %zu / %zu callbacks left", Red::TypeInfoRegistrar::PendingRegisterCount(),
                        Red::TypeInfoRegistrar::PendingDescribeCount());

    // 2. The localisation archive. ArchiveXL's own natives were registered by archivexl_registerNatives just before, but by the time this runs
    //    ArchiveXL's resource depot is already initialized and refuses RegisterArchive ("depot is already initialized", boot 2026-10-12). When the
    //    archive is installed in archive/Mac/mod/ (and its .archive.xl beside it), ArchiveXL mounts it itself at startup: nothing to register.
    //    Otherwise (a plugin-folder install) keep the Windows call.
    {
        std::error_code error;
        const auto mounted = ModSettingsMac::GameRoot() / "archive" / "Mac" / "mod" / "ModSettings.archive";
        if (std::filesystem::exists(mounted, error))
        {
            ModSettingsMac::Log("ModSettings.archive is installed in archive/Mac/mod/ (%s): ArchiveXL mounts it at startup, RegisterArchive skipped",
                                mounted.string().c_str());
        }
        else
        {
            ArchiveXL::RegisterArchive(handle, "ModSettings.archive");
        }
    }

    // 3. The hooks, one attach attempt each, after Codeware has installed its own (it does that in its Main).
    ModSettingsMac::Log("SkipKey is not hooked on the Mac: 0x48026F0 has no reference of any kind (R41b), gamepad-range keys are skipped as in vanilla");
    ModModuleFactory::GetInstance().Load(sdk, handle);
}

// Diagnostics for the offline smoke test (a plain process that dlopens the dylib): how many RedLib registrars the dylib queued.
extern "C" __attribute__((visibility("default"))) void modsettings_mac_pending(unsigned long* aRegister, unsigned long* aDescribe)
{
    if (aRegister)
    {
        *aRegister = Red::TypeInfoRegistrar::PendingRegisterCount();
    }
    if (aDescribe)
    {
        *aDescribe = Red::TypeInfoRegistrar::PendingDescribeCount();
    }
}
