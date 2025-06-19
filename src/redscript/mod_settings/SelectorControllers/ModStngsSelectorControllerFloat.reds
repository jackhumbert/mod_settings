public class ModStngsSelectorControllerFloat extends SettingsSelectorControllerRange {
  public let m_newValue: Float;
  private let m_sliderWidget: inkWidgetRef;
  private let m_sliderController: wref<inkSliderController>;

  public func Setup(entry: ref<ConfigVar>, isPreGame: Bool) -> Void {
    let value: ref<ModConfigVarFloat>;
    super.Setup(entry, isPreGame);
    value = this.m_SettingsEntry as ModConfigVarFloat;
    this.m_sliderController = inkWidgetRef.GetControllerByType(this.m_sliderWidget, n"inkSliderController") as inkSliderController;
    this.m_sliderController.Setup(value.GetMinValue(), value.GetMaxValue(), this.m_newValue, value.GetStepValue());
    this.m_sliderController.RegisterToCallback(n"OnSliderValueChanged", this, n"OnSliderValueChanged");
    this.m_sliderController.RegisterToCallback(n"OnSliderHandleReleased", this, n"OnHandleReleased");
  }

  protected cb func OnSliderValueChanged(sliderController: wref<inkSliderController>, progress: Float, value: Float) -> Bool {
    this.m_newValue = value;
    this.Refresh();
  }

  protected cb func OnHandleReleased() -> Bool {
    let value: ref<ModConfigVarFloat> = this.m_SettingsEntry as ModConfigVarFloat;
    value.SetValue(this.m_newValue);
  }

  private func RegisterShortcutCallbacks() -> Void {
    super.RegisterShortcutCallbacks();
    this.RegisterToCallback(n"OnRepeat", this, n"OnShortcutRepeat");
  }

  private func ChangeValue(forward: Bool) -> Void {
    let value: ref<ModConfigVarFloat> = this.m_SettingsEntry as ModConfigVarFloat;
    let step: Float = forward ? value.GetStepValue() : -value.GetStepValue();
    this.m_newValue = ClampF(this.m_newValue + step, value.GetMinValue(), value.GetMaxValue());
    this.Refresh();
  }

  private func AcceptValue(forward: Bool) -> Void {
    let value: ref<ModConfigVarFloat> = this.m_SettingsEntry as ModConfigVarFloat;
    if value.GetValue() == this.m_newValue {
      this.ChangeValue(forward);
    };
    value.SetValue(this.m_newValue);
    this.Refresh();
  }

  public func Refresh() -> Void {
    super.Refresh();
    this.UpdateValueTextLanguageResources();
    let value = this.m_SettingsEntry as ModConfigVarFloat;
    let step = value.GetStepValue();
    let prec = 1;
    if (step < 0.1) {
      prec = 2;
    }
    if (step < 0.01) {
      prec = 3;
    }
    if (step < 0.001) {
      prec = 4;
    }
    if (step < 0.0001) {
      prec = 5;
    }
    inkTextRef.SetText(this.m_ValueText, FloatToStringPrec(this.m_newValue, prec));
    this.m_sliderController.ChangeValue(this.m_newValue);
  }

  protected cb func OnUpdateValue() -> Bool {
    let value: ref<ModConfigVarFloat> = this.m_SettingsEntry as ModConfigVarFloat;
    this.m_newValue = value.GetValue();
    super.OnUpdateValue();
  }
}