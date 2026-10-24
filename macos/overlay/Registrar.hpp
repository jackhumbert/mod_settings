#pragma once
// macOS replacement for deps/cpcmake/include/Registrar.hpp (not on the Mac include path). Same names and same macros, so the
// hooks and modules in src/red4ext register themselves unchanged. What differs:
//   * the address is resolved to an absolute game address (the Mac SDK table plus ModSettingsSdkRows.inc), not an offset from
//     GetModuleHandle(nullptr): under DYLD_INSERT_LIBRARIES image 0 is not the game;
//   * Load attaches every hook exactly once through ModSettingsMac::AttachHook (one log line each). The Windows loop
//     `while (!Attach(...)) {}` retried forever;
//   * a hash that has no Mac address (a trap stub) is skipped with a log line, never attached;
//   * Unload detaches only what this plugin attached itself (a hook chained on Codeware's stays in its chain).
#include <RED4ext/RED4ext.hpp>
#include <functional>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "MacShims.hpp"

struct ModModule {
  virtual void Load(const RED4ext::Sdk *aSdk, RED4ext::PluginHandle aHandle){};
  virtual void RegisterTypes(){};
  virtual void PostRegisterTypes(){};
  virtual void Unload(const RED4ext::Sdk *aSdk, RED4ext::PluginHandle aHandle){};
};

class IModModuleHook : ModModule {
public:
  std::string m_name;
  uintptr_t m_address;     // absolute game address, 0 when unresolved
  void *m_hook;
  void **m_original;
  bool m_sharedWithCodeware = false;
  ModSettingsMac::HookResult m_result = ModSettingsMac::HookResult::Refused;

  virtual void Load(const RED4ext::Sdk *aSdk, RED4ext::PluginHandle aHandle) override {
    (void)aSdk;
    (void)aHandle;
    m_result = ModSettingsMac::AttachHook(m_name.c_str(), m_address, m_hook, m_original, m_sharedWithCodeware);
  };

  virtual void Unload(const RED4ext::Sdk *aSdk, RED4ext::PluginHandle aHandle) override {
    (void)aSdk;
    (void)aHandle;
    ModSettingsMac::DetachHook(m_name.c_str(), m_address, m_result);
  };
};

class ModModuleFactory {
  std::vector<std::function<void(const RED4ext::Sdk *, RED4ext::PluginHandle)>> s_loads;
  std::vector<std::function<void(const RED4ext::Sdk *, RED4ext::PluginHandle)>> s_unloads;
  std::vector<std::function<void()>> s_registers;
  std::vector<std::function<void()>> s_postRegisters;
  std::vector<IModModuleHook *> s_hooks;

public:
  static ModModuleFactory &GetInstance() {
    static ModModuleFactory s_instance;
    return s_instance;
  }

  template <class T> void registerClass(const std::string &name) {
    s_loads.emplace_back(
        [](const RED4ext::Sdk *aSdk, RED4ext::PluginHandle aHandle) -> void { (new T())->Load(aSdk, aHandle); });
    s_unloads.emplace_back(
        [](const RED4ext::Sdk *aSdk, RED4ext::PluginHandle aHandle) -> void { (new T())->Unload(aSdk, aHandle); });
    s_registers.emplace_back([]() -> void { (new T())->RegisterTypes(); });
    s_postRegisters.emplace_back([]() -> void { (new T())->PostRegisterTypes(); });
  }

  void registerHook(IModModuleHook *moduleHook) {
    s_hooks.emplace_back(moduleHook);
  }

  void Load(const RED4ext::Sdk *aSdk, RED4ext::PluginHandle aHandle) {
    for (const auto &load : s_loads) {
      load(aSdk, aHandle);
    }
    ModSettingsMac::Log("attaching %zu hook(s)", s_hooks.size());
    for (const auto &hook : s_hooks) {
      hook->Load(aSdk, aHandle);
    }
  }

  void Unload(const RED4ext::Sdk *aSdk, RED4ext::PluginHandle aHandle) {
    for (const auto &unload : s_unloads) {
      unload(aSdk, aHandle);
    }
    for (const auto &hook : s_hooks) {
      hook->Unload(aSdk, aHandle);
    }
  }

  void RegisterTypes() {
    for (const auto &s_register : s_registers) {
      s_register();
    }
  }

  void PostRegisterTypes() {
    for (const auto &postRegister : s_postRegisters) {
      postRegister();
    }
  }
};

template <class T> class ModModuleRegister {
public:
  explicit ModModuleRegister(const std::string &name) { ModModuleFactory::GetInstance().registerClass<T>(name); }
};

#define REGISTER_MODULE(derived_class)                                                                            \
  ModModuleRegister<derived_class> s_##derived_class##Creator(#derived_class);

// By fixed image offset (RVA from the game base). Not used by Mod Settings; kept for the macro set.
class ModModuleHook : IModModuleHook {
public:
  explicit ModModuleHook(std::string name, uintptr_t address, void * hook, void ** original) {
    this->m_name = name;
    this->m_address = ModSettingsMac::GameBase() + address;
    this->m_hook = hook;
    this->m_original = original;
    ModModuleFactory::GetInstance().registerHook(this);
  }
};

// By address hash. The hash is resolved now (static initialization) to an absolute address through the Mac SDK table: a mapped
// hash gives imageBase + RVA, an unmapped one gives a trap stub that AttachHook refuses.
class ModModuleHookHash : IModModuleHook {
public:
  explicit ModModuleHookHash(std::string name, uint32_t hash, void * hook, void ** original, bool sharedWithCodeware = false) {
    this->m_name = name;
    this->m_address = RED4ext::UniversalRelocBase::Resolve(hash);
    this->m_hook = hook;
    this->m_original = original;
    this->m_sharedWithCodeware = sharedWithCodeware;
    ModModuleFactory::GetInstance().registerHook(this);
  }
};

#define REGISTER_HOOK(retType, func, ...) \
  retType func(__VA_ARGS__); \
  decltype(&func) func##_Original; \
  ModModuleHook s_##func##_Hook(#func, func##_Addr, reinterpret_cast<void*>(&func), reinterpret_cast<void**>(&func##_Original)); \
  retType func(__VA_ARGS__)

#define REGISTER_HOOK_HASH(retType, hash, func, ...) \
  retType func(__VA_ARGS__); \
  decltype(&func) func##_Original; \
  ModModuleHookHash s_##func##_Hook(#func, hash, reinterpret_cast<void*>(&func), reinterpret_cast<void**>(&func##_Original)); \
  retType func(__VA_ARGS__)

// Mac only: the target is also hooked by Codeware, so this plugin joins Codeware's hook chain instead of attaching itself.
#define REGISTER_HOOK_HASH_SHARED(retType, hash, func, ...) \
  retType func(__VA_ARGS__); \
  decltype(&func) func##_Original; \
  ModModuleHookHash s_##func##_Hook(#func, hash, reinterpret_cast<void*>(&func), reinterpret_cast<void**>(&func##_Original), true); \
  retType func(__VA_ARGS__)
