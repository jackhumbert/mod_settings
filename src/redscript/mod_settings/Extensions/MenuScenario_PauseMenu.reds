@addMethod(MenuScenario_PauseMenu)
protected cb func OnSwitchToModSettings() -> Bool {
    this.SwitchMenu(n"mod_settings_main");
}

// The game hands Back to settings_main only and closes every other page itself, so Back in Mod Settings closed
// the page without saving or dropping its unapplied changes. The page takes Back, saves, and closes itself
// (OnCloseSettingsScreen); when nothing takes it (the page never came up), the game's Back goes on as before.
@wrapMethod(MenuScenario_PauseMenu)
protected cb func OnBack() -> Bool {
    if Equals(this.m_currMenuName, n"mod_settings_main") && Equals(this.m_prevMenuName, n"None") {
        if this.GetMenusState().DispatchEvent(n"mod_settings_main", n"OnBack") {
            return true;
        }
    }
    return wrappedMethod();
}

// the page asks to close once it has saved (RequestClose); back to the pause menu, as the game's settings do
@addMethod(MenuScenario_PauseMenu)
protected cb func OnCloseModSettingsScreen() -> Bool {
    this.OnCloseSettingsScreen();
}
