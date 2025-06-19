@addMethod(MenuScenario_SingleplayerMenu)
protected cb func OnSwitchToModSettings() -> Bool {
    this.CloseSubMenu();
    this.SwitchToScenario(n"MenuScenario_ModSettings");
}

@addMethod(MenuScenario_SingleplayerMenu)
protected cb func OnCloseModSettings() -> Bool {
    if Equals(this.m_currSubMenuName, n"mod_settings_main") {
        this.CloseSubMenu();
    };
}
