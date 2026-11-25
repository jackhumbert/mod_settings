#include "ModSettings.hpp"
#include <Registrar.hpp>
#include <RED4ext/ISerializable.hpp>
#include <RED4ext/ResourceLoader.hpp>
#include <RED4ext/ResourcePath.hpp>
#include <RED4ext/Scripting/Natives/Generated/ink/MenuResource.hpp>
#include <RED4ext/Scripting/Natives/Generated/ink/WidgetLibraryResource.hpp>

// Adds the Mod Settings page to the game's two menu registries as they load, in place of shipping copies of them:
// a copy goes stale with each patch that adds a menu (2.5's glamdoc screen came up empty with the 2.3 copies), and
// two mods shipping copies of one file leave only one of them working.

using namespace RED4ext;
using ModSettings::sdk;
using ModSettings::pluginHandle;

namespace {
// the serializer's state for one resource load; its finished objects at 0x230 (ArchiveXL's and Codeware's layout)
struct ResourceSerializerContext {
  uint8_t unk00[0x230];
  DynArray<Handle<ISerializable>> serializables; // 230
};

constexpr ResourcePath kMenu = "base\\gameplay\\gui\\fullscreen\\menu.inkmenu";
constexpr ResourcePath kPregameMenu = "base\\gameplay\\gui\\fullscreen\\main_menu\\pregame_menu.inkmenu";
constexpr ResourcePath kPageWidget = "base\\gameplay\\gui\\fullscreen\\settings\\mod_settings_main.inkwidget";

void AddModSettings(ink::MenuResource *aMenu, bool aPregame) {
  static const CName page = "mod_settings_main";
  static const CName settings = "settings_main";
  static const CName scenario = "MenuScenario_ModSettings";
  const ink::MenuEntry *settingsEntry = nullptr;
  for (const auto &entry : aMenu->menusEntries) {
    if (entry.name == page) {
      return; // already there (an old ModSettings.archive with its own copy, or another load of the same file)
    }
    if (entry.name == settings) {
      settingsEntry = &entry;
    }
  }
  if (!settingsEntry) {
    sdk->logger->Warn(pluginHandle, "The menu registry has no settings_main to copy: Mod Settings page not added");
    return;
  }
  // the game's settings page entry, with our page's widget: same depth, input context and spawn mode
  ink::MenuEntry entry = *settingsEntry;
  entry.name = page;
  entry.menuWidget = Ref<ink::WidgetLibraryResource>(kPageWidget);
  // the loader fetched the hard references of the entries it read before this point; ours is started here
  entry.menuWidget.token = ResourceLoader::Get()->LoadAsync<ink::WidgetLibraryResource>(kPageWidget);
  aMenu->menusEntries.PushBack(entry);
  if (aPregame) {
    aMenu->scenariosNames.PushBack(scenario);
  }
  sdk->logger->InfoF(pluginHandle, "Added the Mod Settings page to %s (%u menus)",
                     aPregame ? "pregame_menu.inkmenu" : "menu.inkmenu", aMenu->menusEntries.size);
}
} // namespace

// bool ResourceSerializer::OnDependenciesReady(ResourceSerializerContext *)
REGISTER_HOOK_HASH(bool, 3892979844, ResourceSerializer_OnDependenciesReady, ResourceSerializerContext *aContext) {
  static auto *menuType = CRTTISystem::Get()->GetClass(ink::MenuResource::NAME);
  for (const auto &serializable : aContext->serializables) {
    if (serializable && serializable->GetNativeType() == menuType) {
      auto *menu = reinterpret_cast<ink::MenuResource *>(serializable.GetPtr());
      if (menu->path == kMenu) {
        AddModSettings(menu, false);
      } else if (menu->path == kPregameMenu) {
        AddModSettings(menu, true);
      }
    }
  }
  return ResourceSerializer_OnDependenciesReady_Original(aContext);
}
