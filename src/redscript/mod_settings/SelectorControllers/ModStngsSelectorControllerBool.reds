public class ModStngsSelectorControllerBool extends SettingsSelectorController {
  protected let m_onState: inkWidgetRef;
  protected let m_offState: inkWidgetRef;
  protected let m_onStateBody: inkWidgetRef;
  protected let m_offStateBody: inkWidgetRef;

  public func Setup(entry: ref<ConfigVar>, isPreGame: Bool) -> Void {
    super.Setup(entry, isPreGame);
  }

  public func Refresh() -> Void {
    let buttonLogic: ref<inkButtonController>;
    let value: Bool;
    let realValue: ref<ModConfigVarBool> = this.m_SettingsEntry as ModConfigVarBool;
    super.Refresh();
    value = realValue.GetValue();
    inkWidgetRef.SetVisible(this.m_onState, value);
    inkWidgetRef.SetVisible(this.m_offState, !value);
    buttonLogic = inkWidgetRef.GetControllerByType(this.m_onState, n"inkButtonController") as inkButtonController;
    if IsDefined(buttonLogic) {
      buttonLogic.SetEnabled(!this.m_SettingsEntry.IsDisabled());
    };
    buttonLogic = inkWidgetRef.GetControllerByType(this.m_offState, n"inkButtonController") as inkButtonController;
    if IsDefined(buttonLogic) {
      buttonLogic.SetEnabled(!this.m_SettingsEntry.IsDisabled());
    };
  }

  protected cb func OnInitialize() -> Bool {
    super.OnInitialize();
    if inkWidgetRef.IsValid(this.m_offStateBody) {
      inkWidgetRef.RegisterToCallback(this.m_offStateBody, n"OnRelease", this, n"OnLeft");
    };
    if inkWidgetRef.IsValid(this.m_onStateBody) {
      inkWidgetRef.RegisterToCallback(this.m_onStateBody, n"OnRelease", this, n"OnRight");
    };
    if inkWidgetRef.IsValid(this.m_Raycaster) {
      this.RegisterToCallback(n"OnRelease", this, n"OnShortcutPress");
    };
  }

  private func AcceptValue(forward: Bool) -> Void {
    let boolValue: ref<ModConfigVarBool> = this.m_SettingsEntry as ModConfigVarBool;
    boolValue.Toggle();
    this.Refresh();
  }
}