// Adds the Mod Settings entry next to one of the game's own entries instead of
// replacing the menus' PopulateMenuItemList, so entries a patch adds survive and
// the indices the game hardcodes stay put (the 2.5 main menu attaches its mod.io
// tooltip to whatever item sits where its Mods entry should be).
@addField(gameuiMenuItemListGameController)
private let m_modSettingsAdded: Bool;

@wrapMethod(gameuiMenuItemListGameController)
protected func AddMenuItem(const label: script_ref<String>, spawnEvent: CName) {
  wrappedMethod(label, spawnEvent);
  // Main and pause menus: right after the game's Mods entry. The death menu has none.
  if Equals(spawnEvent, n"OnSwitchToModioMenu")
      || (Equals(spawnEvent, n"OnSwitchToSettings") && this.IsA(n"DeathMenuGameController")) {
    wrappedMethod(GetLocalizedText("UI-Labels-ModSettings"), n"OnSwitchToModSettings");
    this.m_modSettingsAdded = true;
  };
}

// Fallback for when the Mods entry never shows up (another mod hides the mod.io
// entry or replaces PopulateMenuItemList without it): append at the end.
@addMethod(gameuiMenuItemListGameController)
protected func AddModSettingsIfMissing() {
  if !this.m_modSettingsAdded {
    this.AddMenuItem(GetLocalizedText("UI-Labels-ModSettings"), n"OnSwitchToModSettings");
    this.m_modSettingsAdded = true;
    this.m_menuListController.Refresh();
  };
}

@wrapMethod(PauseMenuGameController)
private func PopulateMenuItemList() {
  this.m_modSettingsAdded = false;
  wrappedMethod();
  this.AddModSettingsIfMissing();
}

@wrapMethod(SingleplayerMenuGameController)
private func PopulateMenuItemList() {
  this.m_modSettingsAdded = false;
  wrappedMethod();
  this.AddModSettingsIfMissing();
}
