@replaceMethod(SingleplayerMenuGameController)
private func PopulateMenuItemList() -> Void {
  if this.m_savesCount > 0 {
    this.AddMenuItem(GetLocalizedText("UI-ScriptExports-Continue0"), PauseMenuAction.QuickLoad);
  };
  this.AddMenuItem(GetLocalizedText("UI-ScriptExports-NewGame0"), n"OnNewGame");
  this.AddMenuItem(GetLocalizedText("UI-ScriptExports-LoadGame0"), n"OnLoadGame");
  this.AddMenuItem(GetLocalizedText("UI-Labels-Settings"), n"OnSwitchToSettings");
  this.AddMenuItem(GetLocalizedText("UI-Labels-ModSettings"), n"OnSwitchToModSettings");
  this.AddMenuItem(GetLocalizedText("UI-Labels-Credits"), n"OnCreditsPicker");
  if TrialHelper.IsInPS5TrialMode() {
    this.AddMenuItem(GetLocalizedText("UI-Notifications-Ps5TrialBuyMenuItem"), n"OnBuyGame");
  };
  this.m_menuListController.Refresh();
  this.SetCursorOverWidget(inkCompoundRef.GetWidgetByIndex(this.m_menuList, 0));
}