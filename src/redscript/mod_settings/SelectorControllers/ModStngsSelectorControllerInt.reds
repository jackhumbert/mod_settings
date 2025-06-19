public class ModStngsSelectorControllerInt extends SettingsSelectorControllerRange {
  private let m_newValue: Int32;
  private let m_sliderWidget: inkWidgetRef;
  private let m_sliderController: wref<inkSliderController>;

  public func Setup(entry: ref<ConfigVar>, isPreGame: Bool) -> Void {
    let value: ref<ModConfigVarInt32>;
    super.Setup(entry, isPreGame);
    value = this.m_SettingsEntry as ModConfigVarInt32;
    this.m_sliderController = inkWidgetRef.GetControllerByType(this.m_sliderWidget, n"inkSliderController") as inkSliderController;
    this.m_sliderController.Setup(Cast<Float>(value.GetMinValue()), Cast<Float>(value.GetMaxValue()), Cast<Float>(this.m_newValue), Cast<Float>(value.GetStepValue()));
    this.m_sliderController.RegisterToCallback(n"OnSliderValueChanged", this, n"OnSliderValueChanged");
    this.m_sliderController.RegisterToCallback(n"OnSliderHandleReleased", this, n"OnHandleReleased");
  }

  protected cb func OnSliderValueChanged(sliderController: wref<inkSliderController>, progress: Float, value: Float) -> Bool {
    this.m_newValue = Cast<Int32>(value);
    this.Refresh();
  }

  protected cb func OnHandleReleased() -> Bool {
    let value: ref<ModConfigVarInt32> = this.m_SettingsEntry as ModConfigVarInt32;
    value.SetValue(this.m_newValue);
  }

  private func RegisterShortcutCallbacks() -> Void {
    super.RegisterShortcutCallbacks();
    this.RegisterToCallback(n"OnRepeat", this, n"OnShortcutRepeat");
  }

  private func ChangeValue(forward: Bool) -> Void {
    let value: ref<ModConfigVarInt32> = this.m_SettingsEntry as ModConfigVarInt32;
    let step: Int32 = forward ? value.GetStepValue() : -value.GetStepValue();
    this.m_newValue = Clamp(this.m_newValue + step, value.GetMinValue(), value.GetMaxValue());
    this.Refresh();
  }

  private func AcceptValue(forward: Bool) -> Void {
    let value: ref<ModConfigVarInt32> = this.m_SettingsEntry as ModConfigVarInt32;
    if value.GetValue() == this.m_newValue {
      this.ChangeValue(forward);
    };
    value.SetValue(this.m_newValue);
  }

  public func Refresh() -> Void {
    super.Refresh();
    this.UpdateValueTextLanguageResources();
    inkTextRef.SetText(this.m_ValueText, IntToString(this.m_newValue));
    this.m_sliderController.ChangeValue(Cast<Float>(this.m_newValue));
  }

  protected cb func OnUpdateValue() -> Bool {
    let value: ref<ModConfigVarInt32> = this.m_SettingsEntry as ModConfigVarInt32;
    this.m_newValue = value.GetValue();
    super.OnUpdateValue();
  }
}