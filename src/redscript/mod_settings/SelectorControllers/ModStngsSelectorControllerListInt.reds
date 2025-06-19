public class ModStngsSelectorControllerListInt extends SettingsSelectorControllerList {
  public func Setup(entry: ref<ConfigVar>, isPreGame: Bool) -> Void {
    let data: array<Int32>;
    let value: ref<ModConfigVarEnum>;
    super.Setup(entry, isPreGame);
    value = this.m_SettingsEntry as ModConfigVarEnum;
    data = value.GetValues();
    this.PopulateDots(ArraySize(data));
    this.SelectDot(value.GetIndex());
  }

  private func ChangeValue(forward: Bool) -> Void {
    let value: ref<ModConfigVarEnum> = this.m_SettingsEntry as ModConfigVarEnum;
    let listElements: array<Int32> = value.GetValues();
    let index: Int32 = value.GetIndex();
    let newIndex: Int32 = index + (forward ? 1 : -1);
    if newIndex < 0 {
      newIndex = ArraySize(listElements) - 1;
    } else {
      if newIndex >= ArraySize(listElements) {
        newIndex = 0;
      };
    };
    if index != newIndex {
      value.SetIndex(newIndex);
    };
  }

  public func Refresh() -> Void {
    let index: Int32;
    let value: ref<ModConfigVarEnum>;
    super.Refresh();
    value = this.m_SettingsEntry as ModConfigVarEnum;
    index = value.GetIndex();
    this.UpdateValueTextLanguageResources();
    if !value.ListHasDisplayValues() {
      inkTextRef.SetText(this.m_ValueText, IntToString(value.GetValue()));
    } else {
      let text = GetLocalizedTextByKey(value.GetDisplayValue(index));
      if StrLen(text) == 0 {
        text = ToString(value.GetDisplayValue(index));
      };
      inkTextRef.SetText(this.m_ValueText, text);
    };
    this.SelectDot(index);
  }
}