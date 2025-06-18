@replaceMethod(PauseMenuGameController)
private func PopulateMenuItemList() -> Void {
    this.AddMenuItem(GetLocalizedText("UI-Labels-Resume"), n"OnClosePauseMenu");
    this.AddMenuItem(GetLocalizedText("UI-ResourceExports-SaveGame"), PauseMenuAction.Save);
    if this.m_savesCount > 0 {
        this.AddMenuItem(GetLocalizedText("UI-ScriptExports-LoadGame0"), n"OnSwitchToLoadGame");
    };
    this.AddMenuItem(GetLocalizedText("UI-Labels-Settings"), n"OnSwitchToSettings");
    this.AddMenuItem(GetLocalizedText("UI-Labels-ModSettings"), n"OnSwitchToModSettings");
    this.AddMenuItem(GetLocalizedText("UI-Labels-Credits"), n"OnCreditsPicker");
    if TrialHelper.IsInPS5TrialMode() {
        this.AddMenuItem(GetLocalizedText("UI-Notifications-Ps5TrialBuyMenuItem"), n"OnBuyGame");
    };
    this.AddMenuItem(GetLocalizedText("UI-Labels-ExitToMenu"), PauseMenuAction.ExitToMainMenu);
    inkWidgetRef.SetVisible(this.m_expansionNotyficationRef, GameInstance.GetUISystem(this.GetPlayerControlledObject().GetGame()).IsPatchIntroNeeded(gameuiPatchIntro.Patch2000_EP1) && this.GetSystemRequestsHandler().IsAdditionalContentInstalled(n"EP1"));
    this.m_menuListController.Refresh();
}