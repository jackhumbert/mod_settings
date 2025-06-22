@addField(SettingsSelectorControllerKeyBinding)
protected let m_listeningForModSettingsInput: Bool;

@wrapMethod(SettingsSelectorControllerKeyBinding)
protected cb func OnKeyBindingEvent(e: ref<KeyBindingEvent>) -> Bool {
  if ModSettings.GetInstance().isActive {
    if this.m_listeningForModSettingsInput {
      // LogChannel(n"DEBUG", NameToString(e.keyName));
      if !this.IsCancel(e.keyName) {
        this.SetValue(e.keyName);
      } else {
        this.Refresh();
      };
      this.m_listeningForModSettingsInput = false;
      this.StopListeningForInput();
      inkWidgetRef.SetOpacity(this.m_editView, 0.00);
    }
  } else {
    return wrappedMethod(e);
  }
}

@wrapMethod(SettingsSelectorControllerKeyBinding)
protected cb func OnRelease(e: ref<inkPointerEvent>) -> Bool {
  if ModSettings.GetInstance().isActive {
    // LogChannel(n"DEBUG", NameToString(e.GetActionName().ToName()));
    if e.IsAction(n"click") {
      inkTextRef.SetLocalizedText(this.m_text, n"UI-Settings-ButtonMappings-Misc-KeyBind");
      inkWidgetRef.SetOpacity(this.m_editView, this.m_editOpacity);
      this.ListenForInput();
      this.m_listeningForModSettingsInput = true;
    } else {
      if e.IsAction(n"unequip_item") {
        this.ResetValue();
      };
    };
  } else {
    return wrappedMethod(e);
  }
}

@wrapMethod(SettingsSelectorControllerKeyBinding)
public func Refresh() -> Void {
  if ModSettings.GetInstance().isActive {
    let varName: ref<ModConfigVarKeyBinding>;
    super.Refresh();
    varName = this.m_SettingsEntry as ModConfigVarKeyBinding;
    // inkTextRef.SetText(this.m_text, SettingsSelectorControllerKeyBinding.PrepareInputTag(varName.GetValue(), varName.GetGroup().GetName(), varName.GetName()));
    inkTextRef.SetText(this.m_text, SettingsSelectorControllerKeyBinding.PrepareInputTag(varName.GetValueName(), n"/key_bindings", varName.GetName()));
    this.TriggerActionFeedback();
  } else {
    wrappedMethod();
  }
}

@wrapMethod(SettingsSelectorControllerKeyBinding)
private final func SetValue(key: CName) -> Void {
  if ModSettings.GetInstance().isActive {
    let value: ref<ModConfigVarKeyBinding> = this.m_SettingsEntry as ModConfigVarKeyBinding;
    value.SetValueName(key);
  } else {
    wrappedMethod(key);
  }
}