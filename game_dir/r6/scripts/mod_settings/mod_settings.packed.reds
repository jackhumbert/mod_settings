// Mod Settings v0.0.8
// Licensed under the MIT license. See the license.md in the root project for details.
// https://github.com/jackhumbert/mod_settings

// This file was automatically generated on 2023-04-26 01:33:10 UTC

// mod_settings/ModSettings.reds

public native class ModSettings extends IScriptable {
    public native static func GetInstance() -> ref<ModSettings>;
    public native static func GetMods() -> array<CName>;
    public native static func GetCategories(mod: CName) -> array<CName>;
    public native static func GetVars(mod: CName, category: CName) -> array<ref<ConfigVar>>;
    public native static func AcceptChanges() -> Void;
    public native static func RejectChanges() -> Void;
    public native static func RestoreDefaults(mod: CName) -> Void;
    public native static func RegisterListenerToClass(self: ref<IScriptable>) -> Void;
    public native static func UnregisterListenerToClass(self: ref<IScriptable>) -> Void;
    public native static func RegisterListenerToModifications(self: ref<IScriptable>) -> Void;
    public native static func UnregisterListenerToModifications(self: ref<IScriptable>) -> Void;

    public native let changeMade: Bool;
    
    public let isActive: Bool;
}

public native class ModConfigVarBool extends ConfigVar {
  public native func SetValue(value: Bool) -> Void;
  public native func GetValue() -> Bool;
  public native func GetDefaultValue() -> Bool;
  public func Toggle() -> Void {
    this.SetValue(!this.GetValue());
  }
}

public native class ModConfigVarFloat extends ConfigVar {
  public native func SetValue(value: Float) -> Void;
  public native func GetValue() -> Float;
  public native func GetDefaultValue() -> Float;
  public native func GetMinValue() -> Float;
  public native func GetMaxValue() -> Float;
  public native func GetStepValue() -> Float;
}

public native class ModConfigVarInt32 extends ConfigVar {
  public native func SetValue(value: Int32) -> Void;
  public native func GetValue() -> Int32;
  public native func GetDefaultValue() -> Int32;
  public native func GetMinValue() -> Int32;
  public native func GetMaxValue() -> Int32;
  public native func GetStepValue() -> Int32;
}

public native class ModConfigVarEnum extends ConfigVar {
  public native func GetValueFor(index: Int32) -> Int32;
  public native func GetValue() -> Int32;
  public native func GetDefaultValue() -> Int32;
  public native func GetValues() -> array<Int32>;
  public native func GetIndexFor(value: Int32) -> Int32;
  public native func GetIndex() -> Int32;
  public native func GetDefaultIndex() -> Int32;
  public native func SetIndex(index: Int32) -> Void;
  public native func GetDisplayValue(index: Int32) -> CName;
}

// public class TestClassForCount {

// @runtimeProperty("ModSettings.mod", "Automatic Mod With a really long name, just for fun")
// public let automaticValue: Float = 0.0;

// @runtimeProperty("ModSettings.mod", "Stone Mod")
// public let stoneValue: Float = 0.0;

// @runtimeProperty("ModSettings.mod", "Introduce Mod")
// public let introduceValue: Float = 0.0;

// @runtimeProperty("ModSettings.mod", "Gaudy Mod")
// public let gaudyValue: Float = 0.0;

// @runtimeProperty("ModSettings.mod", "Violent Mod")
// public let violentValue: Float = 0.0;

// @runtimeProperty("ModSettings.mod", "Farm Mod")
// public let farmValue: Float = 0.0;

// @runtimeProperty("ModSettings.mod", "Loutish Mod")
// public let loutishValue: Float = 0.0;

// @runtimeProperty("ModSettings.mod", "Toothsome Mod")
// public let toothsomeValue: Float = 0.0;

// @runtimeProperty("ModSettings.mod", "Check Mod")
// public let checkValue: Float = 0.0;

// @runtimeProperty("ModSettings.mod", "Lavish Mod")
// public let lavishValue: Float = 0.0;

// @runtimeProperty("ModSettings.mod", "Weary Mod")
// public let wearyValue: Float = 0.0;

// @runtimeProperty("ModSettings.mod", "Permit Mod")
// public let permitValue: Float = 0.0;

// @runtimeProperty("ModSettings.mod", "Division Mod")
// public let divisionValue: Float = 0.0;

// @runtimeProperty("ModSettings.mod", "Cake Mod")
// public let cakeValue: Float = 0.0;

// @runtimeProperty("ModSettings.mod", "Possess Mod")
// public let possessValue: Float = 0.0;

// @runtimeProperty("ModSettings.mod", "Pump Mod")
// public let pumpValue: Float = 0.0;

// @runtimeProperty("ModSettings.mod", "Stage Mod")
// public let stageValue: Float = 0.0;

// @runtimeProperty("ModSettings.mod", "Tub Mod")
// public let tubValue: Float = 0.0;

// @runtimeProperty("ModSettings.mod", "Love Mod")
// public let loveValue: Float = 0.0;

// @runtimeProperty("ModSettings.mod", "Known Mod")
// public let knownValue: Float = 0.0;

// @runtimeProperty("ModSettings.mod", "Far Mod")
// public let farValue: Float = 0.0;

// @runtimeProperty("ModSettings.mod", "Bell Mod")
// public let bellValue: Float = 0.0;

// @runtimeProperty("ModSettings.mod", "Waggish Mod")
// public let waggishValue: Float = 0.0;

// @runtimeProperty("ModSettings.mod", "Honorable Mod")
// public let honorableValue: Float = 0.0;

// @runtimeProperty("ModSettings.mod", "Vivacious Mod")
// public let vivaciousValue: Float = 0.0;

// @runtimeProperty("ModSettings.mod", "Offend Mod")
// public let offendValue: Float = 0.0;

// @runtimeProperty("ModSettings.mod", "Call Mod")
// public let callValue: Float = 0.0;

// @runtimeProperty("ModSettings.mod", "Good Mod")
// public let goodValue: Float = 0.0;

// @runtimeProperty("ModSettings.mod", "Tremendous Mod")
// public let tremendousValue: Float = 0.0;

// @runtimeProperty("ModSettings.mod", "Cycle Mod")
// public let cycleValue: Float = 0.0;

// @runtimeProperty("ModSettings.mod", "Provide Mod")
// public let provideValue: Float = 0.0;

// @runtimeProperty("ModSettings.mod", "Price Mod")
// public let priceValue: Float = 0.0;

// @runtimeProperty("ModSettings.mod", "Purpose Mod")
// public let purposeValue: Float = 0.0;

// @runtimeProperty("ModSettings.mod", "Cellar Mod")
// public let cellarValue: Float = 0.0;

// @runtimeProperty("ModSettings.mod", "Icky Mod")
// public let ickyValue: Float = 0.0;

// @runtimeProperty("ModSettings.mod", "Hobbies Mod")
// public let hobbiesValue: Float = 0.0;

// @runtimeProperty("ModSettings.mod", "Sweater Mod")
// public let sweaterValue: Float = 0.0;

// @runtimeProperty("ModSettings.mod", "Detect Mod")
// public let detectValue: Float = 0.0;

// @runtimeProperty("ModSettings.mod", "Knot Mod")
// public let knotValue: Float = 0.0;

// @runtimeProperty("ModSettings.mod", "Visit Mod")
// public let visitValue: Float = 0.0;

// @runtimeProperty("ModSettings.mod", "Double Mod")
// public let doubleValue: Float = 0.0;

// @runtimeProperty("ModSettings.mod", "Please Mod")
// public let pleaseValue: Float = 0.0;

// @runtimeProperty("ModSettings.mod", "Can Mod")
// public let canValue: Float = 0.0;

// @runtimeProperty("ModSettings.mod", "Classy Mod")
// public let classyValue: Float = 0.0;

// @runtimeProperty("ModSettings.mod", "Grieving Mod")
// public let grievingValue: Float = 0.0;

// @runtimeProperty("ModSettings.mod", "Gullible Mod")
// public let gullibleValue: Float = 0.0;

// @runtimeProperty("ModSettings.mod", "Inexpensive Mod")
// public let inexpensiveValue: Float = 0.0;

// @runtimeProperty("ModSettings.mod", "Reflect Mod")
// public let reflectValue: Float = 0.0;

// }

// mod_settings/ModSettingsMainGameController.reds

public class ModStngsMainGameController extends gameuiSettingsMenuGameController {

  private let m_scrollPanel: inkWidgetRef;
  private let m_selectorWidget: inkWidgetRef;
  private let m_buttonHintsManagerRef: inkWidgetRef;
  private let m_settingsOptionsList: inkCompoundRef;
  private let m_applyButton: inkWidgetRef;
  private let m_resetButton: inkWidgetRef;
  private let m_defaultButton: inkWidgetRef;
  private let m_brightnessButton: inkWidgetRef;
  private let m_hdrButton: inkWidgetRef;
  private let m_controllerButton: inkWidgetRef;
  private let m_benchmarkButton: inkWidgetRef;
  private let m_descriptionText: inkTextRef;
  private let m_previousButtonHint: inkWidgetRef;
  private let m_nextButtonHint: inkWidgetRef;
  private let m_languageInstallProgressBarRoot: inkWidgetRef;
  
  private let m_languageInstallProgressBar: wref<SettingsLanguageInstallProgressBar>;
  private let m_menuEventDispatcher: wref<inkMenuEventDispatcher>;
  private let m_settingsElements: array<wref<SettingsSelectorController>>;
  private let m_buttonHintsController: wref<ButtonHints>;
  private let m_data: array<SettingsCategory>;
  private let m_menusList: array<CName>;
  private let m_eventsList: array<CName>;
  private let m_settingsListener: ref<ModSettingsVarListener>;
  private let m_settingsNotificationListener: ref<ModSettingsNotificationListener>;
  private let m_settings: ref<UserSettings>;
  private let m_isPreGame: Bool;
  private let m_benchmarkNotificationToken: ref<inkGameNotificationToken>;
  private let m_isKeybindingAlertEnabled: Bool;
  private let m_applyButtonEnabled: Bool;
  private let m_resetButtonEnabled: Bool;
  private let m_closeSettingsRequest: Bool;
  private let m_resetSettingsRequest: Bool;
  private let m_isDlcSettings: Bool;
  private let m_selectorCtrl: wref<ListController>;

  protected cb func OnInitialize() -> Void {
    ModSettings.GetInstance().isActive = true;
    inkWidgetRef.SetVisible(this.m_hdrButton, false);
    inkWidgetRef.SetVisible(this.m_controllerButton, false);
    inkWidgetRef.SetVisible(this.m_brightnessButton, false);
    inkWidgetRef.SetVisible(this.m_benchmarkButton, false);

    this.m_settings = this.GetSystemRequestsHandler().GetUserSettings();
    this.m_isPreGame = this.GetSystemRequestsHandler().IsPreGame();
    // this.m_settingsListener = new ModSettingsVarListener();
    // this.m_settingsListener.RegisterController(this);
    // this.m_settingsNotificationListener = new ModSettingsNotificationListener();
    // this.m_settingsNotificationListener.RegisterController(this);
    // this.m_settingsNotificationListener.Register();
    // this.m_languageInstallProgressBar = inkWidgetRef.GetControllerByType(this.m_languageInstallProgressBarRoot, n"SettingsLanguageInstallProgressBar") as SettingsLanguageInstallProgressBar;
    this.RegisterToGlobalInputCallback(n"OnPostOnRelease", this, n"OnButtonRelease");
    inkWidgetRef.GetControllerByType(this.m_applyButton, n"inkButtonController").RegisterToCallback(n"OnButtonClick", this, n"OnApplyButtonReleased");
    inkWidgetRef.GetControllerByType(this.m_resetButton, n"inkButtonController").RegisterToCallback(n"OnButtonClick", this, n"OnResetButtonReleased");
    inkWidgetRef.GetControllerByType(this.m_controllerButton, n"inkButtonController").RegisterToCallback(n"OnButtonClick", this, n"OnControllerButtonReleased");
    inkWidgetRef.GetControllerByType(this.m_defaultButton, n"inkButtonController").RegisterToCallback(n"OnButtonClick", this, n"OnDefaultButtonReleased");
    this.m_selectorCtrl = inkWidgetRef.GetController(this.m_selectorWidget) as ListController;
    this.m_selectorCtrl.RegisterToCallback(n"OnItemActivated", this, n"OnMenuChanged");
    this.m_buttonHintsController = this.SpawnFromExternal(inkWidgetRef.Get(this.m_buttonHintsManagerRef), r"base\\gameplay\\gui\\common\\buttonhints.inkwidget", n"Root").GetController() as ButtonHints;
    this.PopulateHints();
    this.PopulateSettingsData();
    // this.PopulateCategories(this.m_settings.GetMenuIndex());
    this.PopulateCategories(0);
    this.CheckButtons();
    this.PlayLibraryAnimation(n"intro");
    this.m_closeSettingsRequest = false;
    this.m_resetSettingsRequest = false;

    ModSettings.RegisterListenerToModifications(this);

    //super.OnInitialize();
  }

  public func OnModSettingsChange() -> Void {
    this.CheckButtons();
    this.PopulateSettingsData();
    this.PopulateCategorySettingsOptions(-1);
    this.RefreshInputIcons();
  }

  protected cb func OnSetUserData(userData: ref<IScriptable>) -> Bool {
    let settingsUserData: ref<SettingsMenuUserData> = userData as SettingsMenuUserData;
    if IsDefined(settingsUserData) {
      this.m_isDlcSettings = settingsUserData.m_isDlcSettings;
    };
  }

  protected cb func OnUninitialize() -> Bool {
    ModSettings.GetInstance().isActive = false;
    this.UnregisterFromGlobalInputCallback(n"OnPostOnRelease", this, n"OnButtonRelease");
    this.m_menuEventDispatcher.UnregisterFromEvent(n"OnBack", this, n"OnBack");
    this.m_selectorCtrl.UnregisterFromCallback(n"OnItemActivated", this, n"OnMenuChanged");
    inkWidgetRef.GetControllerByType(this.m_applyButton, n"inkButtonController").UnregisterFromCallback(n"OnButtonClick", this, n"OnApplyButtonReleased");
    inkWidgetRef.GetControllerByType(this.m_resetButton, n"inkButtonController").UnregisterFromCallback(n"OnButtonClick", this, n"OnResetButtonReleased");
    inkWidgetRef.GetControllerByType(this.m_controllerButton, n"inkButtonController").UnregisterFromCallback(n"OnButtonClick", this, n"OnControllerButtonReleased");
    inkWidgetRef.GetControllerByType(this.m_defaultButton, n"inkButtonController").UnregisterFromCallback(n"OnButtonClick", this, n"OnDefaultButtonReleased");

    ModSettings.UnregisterListenerToModifications(this);

    //super.OnUninitialize();
  }

  public final func EnableApplyButton() -> Void {
    inkWidgetRef.SetVisible(this.m_applyButton, true);
    this.m_applyButtonEnabled = true;
  }

  public final func DisableApplyButton() -> Void {
    this.m_applyButtonEnabled = false;
    inkWidgetRef.SetVisible(this.m_applyButton, false);
  }

  public final func IsApplyButtonEnabled() -> Bool {
    return this.m_applyButtonEnabled;
  }

  public final func EnableResetButton() -> Void {
    this.m_resetButtonEnabled = true;
    inkWidgetRef.SetVisible(this.m_resetButton, true);
  }

  public final func DisableResetButton() -> Void {
    this.m_resetButtonEnabled = false;
    inkWidgetRef.SetVisible(this.m_resetButton, false);
  }

  public final func IsResetButtonEnabled() -> Bool {
    return this.m_resetButtonEnabled;
  }

  public final func CheckButtons() -> Void {
    // if !this.m_isDlcSettings && (this.m_settings.NeedsConfirmation() || this.m_settings.NeedsRestartToApply() || this.m_settings.NeedsLoadLastCheckpoint()) {
    let ms = ModSettings.GetInstance();
    if ms.changeMade {
      this.EnableApplyButton();
      this.EnableResetButton();
    } else {
      this.DisableApplyButton();
      this.DisableResetButton();
    }
  }

  public final func OnVarModified(groupPath: CName, varName: CName, varType: ConfigVarType, reason: ConfigChangeReason) -> Void {
    let i: Int32;
    let item: ref<SettingsSelectorController>;
    let size: Int32;
    Log("[VAR] modified groupPath: " + NameToString(groupPath) + " varName: " + NameToString(varName));
    size = ArraySize(this.m_settingsElements);
    this.CheckButtons();
    i = 0;
    while i < size {
      item = this.m_settingsElements[i];
      if Equals(item.GetGroupPath(), groupPath) && Equals(item.GetVarName(), varName) {
        item.Refresh();
      };
      i += 1;
    };
  }

  // public final func WarnAboutEmptyKeyBindingValue() -> Void {
  //   if this.m_isKeybindingAlertEnabled && this.IsAnyActionWithoutAssignedKey() {
  //     this.PushNotification();
  //     this.m_isKeybindingAlertEnabled = false;
  //   };
  // }

  public final func OnSettingsNotify(status: ConfigNotificationType) -> Void {
    // this.WarnAboutEmptyKeyBindingValue();
    switch status {
      case ConfigNotificationType.RestartRequiredConfirmed:
      case ConfigNotificationType.ChangesApplied:
      case ConfigNotificationType.Saved:
        this.CheckSettings();
        this.PopulateSettingsData();
        this.PopulateCategorySettingsOptions(-1);
        this.RefreshInputIcons();
        break;
      case ConfigNotificationType.ChangesLoadLastCheckpointApplied:
        this.CheckSettings();
        this.PopulateSettingsData();
        this.PopulateCategorySettingsOptions(-1);
        GameInstance.GetTelemetrySystem(this.GetPlayerControlledObject().GetGame()).LogLastCheckpointLoaded();
        this.GetSystemRequestsHandler().LoadLastCheckpoint(true);
        this.RefreshInputIcons();
        break;
      case ConfigNotificationType.ChangesLoadLastCheckpointRejected:
      case ConfigNotificationType.RestartRequiredRejected:
      case ConfigNotificationType.ChangesRejected:
        this.m_closeSettingsRequest = false;
        this.CheckSettings();
        this.PopulateSettingsData();
        this.PopulateCategorySettingsOptions(-1);
        this.RefreshInputIcons();
        break;
      case ConfigNotificationType.ErrorSaving:
        this.RequestClose();
        break;
      case ConfigNotificationType.Refresh:
        this.PopulateSettingsData();
        this.PopulateCategorySettingsOptions(-1);
        this.RefreshInputIcons();
    };
  }

  protected func SetLanguagePackageInstallProgress(progress: Float) -> Void {
    this.m_languageInstallProgressBar.SetProgress(progress);
  }

  protected func SetLanguagePackageInstallProgressBar(progress: Float, completed: Bool, started: Bool) -> Void {
    this.m_languageInstallProgressBar.SetProgressBarVisiblity(!completed && started);
    this.m_languageInstallProgressBar.SetProgress(progress);
  }

  // private final func AddSettingsGroup(settingsGroup: ref<ConfigGroup>) -> Void {
  //   let category: SettingsCategory;
  //   let currentSettingsGroup: ref<ConfigGroup>;
  //   let currentSubcategory: SettingsCategory;
  //   let i: Int32;
  //   let settingsGroups: array<ref<ConfigGroup>>;
  //   category.label = settingsGroup.GetDisplayName();
  //   category.groupPath = settingsGroup.GetPath();
  //   if settingsGroup.HasVars(this.m_isPreGame) {
  //     category.options = settingsGroup.GetVars(this.m_isPreGame);
  //     category.isEmpty = false;
  //   };
  //   settingsGroups = settingsGroup.GetGroups(this.m_isPreGame);
  //   i = 0;
  //   while i < ArraySize(settingsGroups) {
  //     currentSettingsGroup = settingsGroups[i];
  //     if currentSettingsGroup.IsEmpty(this.m_isPreGame) {
  //     } else {
  //       if currentSettingsGroup.HasVars(this.m_isPreGame) {
  //         currentSubcategory.label = currentSettingsGroup.GetDisplayName();
  //         currentSubcategory.options = currentSettingsGroup.GetVars(this.m_isPreGame);
  //         currentSubcategory.isEmpty = false;
  //         ArrayPush(category.subcategories, currentSubcategory);
  //         category.isEmpty = false;
  //         this.m_settingsListener.Register(currentSettingsGroup.GetPath());
  //       };
  //     };
  //     i += 1;
  //   };
  //   if Equals(category.label, n"UI-Settings-KeyBindings") && !this.IsKeyboardConnected() {
  //     category.isEmpty = true;
  //   };
  //   if !category.isEmpty {
  //     ArrayPush(this.m_data, category);
  //     this.m_settingsListener.Register(settingsGroup.GetPath());
  //   };
  // }

  private final func PopulateSettingsData() -> Void {
    // this.m_settingsListener.Register(n"/mods");
    ArrayClear(this.m_data);
    let mods = ModSettings.GetMods();
    let i = 0;
    while i < ArraySize(mods) {
    let category: SettingsCategory;
      category.label = mods[i];
      category.options = ModSettings.GetVars(mods[i], n"None");
      category.isEmpty = false;
      let categories = ModSettings.GetCategories(mods[i]);
      let j = 0;
      while j < ArraySize(categories) {
        let currentSubcategory: SettingsCategory;
        currentSubcategory.label = categories[j];
        currentSubcategory.options = ModSettings.GetVars(mods[i], categories[j]);
        currentSubcategory.isEmpty = false;
        ArrayPush(category.subcategories, currentSubcategory);
        j += 1;
      }
      // FlightLog.Info("[ModStngsMainGameController] " + NameToString(mods[i]) + ": " + i);
      // let j = 0;
      // while j < ArraySize(currentSubcategory.options) {
      //     FlightLog.Info("var: " + ToString(currentSubcategory.options[j].GetDisplayName()));
      //     j += 1;
      // }
      ArrayPush(this.m_data, category);
      i += 1;
    }
  }

  private final func PopulateCategories(idx: Int32) -> Void {
    let curCategory: SettingsCategory;
    let newData: ref<ListItemData>;
    this.m_selectorCtrl.Clear();
    let i = 0;
    while i < ArraySize(this.m_data) {
        curCategory = this.m_data[i];
        if !curCategory.isEmpty {
            newData = new ListItemData();
            newData.label = GetLocalizedTextByKey(curCategory.label);
            if StrLen(newData.label) == 0 {
                newData.label = ToString(curCategory.label);
            };
            this.m_selectorCtrl.PushData(newData);
        };
        i += 1;
    };
    this.m_selectorCtrl.Refresh();
    if idx >= 0 && idx < ArraySize(this.m_data) {
        this.m_selectorCtrl.SetToggledIndex(idx);
    } else {
        this.m_selectorCtrl.SetToggledIndex(0);
    };
  }

  protected cb func OnSetMenuEventDispatcher(menuEventDispatcher: wref<inkMenuEventDispatcher>) -> Bool {
    this.m_menuEventDispatcher = menuEventDispatcher;
    this.m_menuEventDispatcher.RegisterToEvent(n"OnBack", this, n"OnBack");
  }

  protected cb func OnBack(userData: ref<IScriptable>) -> Bool {
    // if !StatusEffectSystem.ObjectHasStatusEffectWithTag(this.GetPlayerControlledObject(), n"LockInHubMenu") {
      this.m_closeSettingsRequest = true;
      this.CheckSettings();
      ModSettings.AcceptChanges();
    // };
  }

  private final func RequestClose() -> Void {
    this.m_menuEventDispatcher.SpawnEvent(n"OnCloseModSettingsScreen");
  }

  private final func RequestRestoreDefaults() -> Void {
    let index: Int32 = this.m_selectorCtrl.GetToggledIndex();
    let mod: CName = this.m_data[index].label;
    ModSettings.RestoreDefaults(mod);
    // this.m_settings.RequestRestoreDefaultDialog(this.m_isPreGame, false, groupPath);
  }

  private final func CheckSettings() -> Void {
    if this.m_resetSettingsRequest {
      this.CheckRejectSettings();
    } else {
      this.CheckAcceptSettings();
    };
  }

  private final func CheckRejectSettings() -> Void {
    ModSettings.RejectChanges();
    this.m_resetSettingsRequest = false;
    // if this.m_settings.NeedsConfirmation() {
    //   this.m_settings.RejectChanges();
    // } else {
    //   if this.m_settings.NeedsRestartToApply() {
    //     this.m_settings.RejectRestartToApply();
    //   } else {
    //     if this.m_settings.NeedsLoadLastCheckpoint() {
    //       this.m_settings.RejectLoadLastCheckpointChanges();
    //     } else {
    //       this.m_resetSettingsRequest = false;
          if this.m_closeSettingsRequest {
            this.m_closeSettingsRequest = false;
            this.RequestClose();
          };
    //     };
    //   };
    // };
  }

  private final func CheckAcceptSettings() -> Void {
    ModSettings.AcceptChanges();
    // if this.m_settings.WasModifiedSinceLastSave() {
    //   if this.m_settings.NeedsConfirmation() {
    //     this.m_settings.RequestConfirmationDialog();
    //   } else {
    //     if this.m_settings.NeedsRestartToApply() {
    //       this.m_settings.RequestNeedsRestartDialog();
    //     } else {
    //       if this.m_settings.NeedsLoadLastCheckpoint() {
    //         this.m_settings.RequestLoadLastCheckpointDialog();
    //       } else {
    //         this.GetSystemRequestsHandler().RequestSaveUserSettings();
            if this.m_closeSettingsRequest {
              this.m_closeSettingsRequest = false;
              this.RequestClose();
            };
    //       };
    //     };
    //   };
    // } else {
    //   if this.m_closeSettingsRequest {
    //     this.m_closeSettingsRequest = false;
    //     this.RequestClose();
    //   };
    // };
  }

  protected cb func OnMenuChanged(index: Int32, target: ref<ListItemController>) -> Bool {
    this.PlaySound(n"Button", n"OnPress");
    this.PopulateCategorySettingsOptions(index);
    (inkWidgetRef.GetController(this.m_scrollPanel) as inkScrollController).SetScrollPosition(0.00);
    // this.m_settings.SetMenuIndex(index);
  }

  protected cb func OnApplyButtonReleased(controller: wref<inkButtonController>) -> Bool {
    this.OnApplyButton();
  }

  protected cb func OnResetButtonReleased(controller: wref<inkButtonController>) -> Bool {
    this.OnResetButton();
  }

  protected cb func OnBenchmarkButtonReleased(controller: wref<inkButtonController>) -> Bool {
    if this.IsBenchmarkPossible() {
      this.RunGraphicsBenchmark();
    };
  }

  protected cb func OnDefaultButtonReleased(controller: wref<inkButtonController>) -> Bool {
    this.RequestRestoreDefaults();
  }

  protected cb func OnLocalizationChanged(evt: ref<inkLocalizationChangedEvent>) -> Bool {
    let idx: Int32 = this.m_selectorCtrl.GetToggledIndex();
    this.PopulateCategories(idx);
    this.PopulateCategorySettingsOptions(idx);
    this.PopulateHints();
  }

  private final func PopulateHints() -> Void {
    this.m_buttonHintsController.ClearButtonHints();
    this.m_buttonHintsController.AddButtonHint(n"select", "UI-UserActions-Select");
    this.m_buttonHintsController.AddButtonHint(n"back", "Common-Access-Close");
    if !this.m_isDlcSettings {
      this.m_buttonHintsController.AddButtonHint(n"restore_default_settings", "UI-UserActions-RestoreDefaults");
    };
  }

  private final func OnApplyButton() -> Void {
    this.m_isKeybindingAlertEnabled = this.m_settings.GetMenuIndex() == 8;
    if !this.IsApplyButtonEnabled() {
      return;
    };
    Log("OnApplyButton");
    if this.m_settings.NeedsConfirmation() {
      this.m_settings.ConfirmChanges();
    } else {
      this.CheckSettings();
    };
  }

  private final func OnResetButton() -> Void {
    if !this.IsResetButtonEnabled() {
      return;
    };
    Log("OnResetButton");
    this.m_resetSettingsRequest = true;
    this.CheckSettings();
  }
  
  protected func ScrollToMod(ratio: Float) {
    let mod_flex = this.GetWidget(n"mod_settings/mod_flex");
    let mod_scroller = mod_flex.GetController() as inkScrollController;
    if IsDefined(mod_scroller) {
      mod_scroller.SetScrollPosition(ratio);
    }
  }

  protected cb func OnButtonRelease(evt: ref<inkPointerEvent>) -> Bool {
    let currentToggledIndex: Int32;
    let listSize: Int32 = this.m_selectorCtrl.Size();
    if evt.IsAction(n"prior_menu") {
      currentToggledIndex = this.m_selectorCtrl.GetToggledIndex();
      if currentToggledIndex < 1 {
        this.m_selectorCtrl.SetToggledIndex(listSize - 1);
        this.ScrollToMod(1.0);
      } else {
        this.m_selectorCtrl.SetToggledIndex(currentToggledIndex - 1);
        this.ScrollToMod(Cast<Float>(currentToggledIndex - 1) / Cast<Float>(listSize - 1));
      };
    } else {
      if evt.IsAction(n"next_menu") {
        currentToggledIndex = this.m_selectorCtrl.GetToggledIndex();
        if currentToggledIndex >= this.m_selectorCtrl.Size() - 1 {
          this.m_selectorCtrl.SetToggledIndex(0);
          this.ScrollToMod(0.0);
        } else {
          this.m_selectorCtrl.SetToggledIndex(currentToggledIndex + 1);
          this.ScrollToMod(Cast<Float>(currentToggledIndex + 1) / Cast<Float>(listSize - 1));
        };
      } else {
        if evt.IsAction(n"restore_default_settings") {
          this.RequestRestoreDefaults();
        } else {
            return false;
        };
      };
    };
  }

  protected cb func OnSettingHoverOver(evt: ref<inkPointerEvent>) -> Bool {
    let descriptionName: CName;
    let description: String;
    let params: ref<inkTextParams>;
    let updatePolicy: ConfigVarUpdatePolicy;
    let currentItem: wref<SettingsSelectorController> = evt.GetCurrentTarget().GetController() as SettingsSelectorController;
    if IsDefined(currentItem) {
      descriptionName = currentItem.GetDescription();
      description = GetLocalizedTextByKey(descriptionName);
      if StrLen(description) == 0 {
          if !Equals(descriptionName, n"None") {
            description = ToString(descriptionName);
          } else {
            description = "";
          }
      };
      updatePolicy = currentItem.GetVarUpdatePolicy();
      if Equals(updatePolicy, ConfigVarUpdatePolicy.ConfirmationRequired) {
        params = new inkTextParams();
        params.AddString("description", description);
        params.AddLocalizedString("additional_text", "LocKey#76947");
        inkTextRef.SetLocalizedTextScript(this.m_descriptionText, "LocKey#76949", params);
      } else {
        if Equals(updatePolicy, ConfigVarUpdatePolicy.RestartRequired) {
          params = new inkTextParams();
          params.AddString("description", description);
          params.AddLocalizedString("additional_text", "LocKey#76948");
          inkTextRef.SetLocalizedTextScript(this.m_descriptionText, "LocKey#76949", params);
        } else {
          inkTextRef.SetText(this.m_descriptionText, description, params);
        };
      };
      inkWidgetRef.SetVisible(this.m_descriptionText, true);
    };
  }

  protected cb func OnSettingHoverOut(evt: ref<inkPointerEvent>) -> Bool {
    inkWidgetRef.SetVisible(this.m_descriptionText, false);
  }

  private final func PopulateOptions(options: array<ref<ConfigVar>>) -> Void {
    let currentItem: wref<SettingsSelectorController>;
    let currentSettingsItem: ref<ConfigVar>;
    let currentSettingsItemType: ConfigVarType;
    let size: Int32 = ArraySize(options);
    let i: Int32 = 0;
    while i < size {
      currentSettingsItem = options[i];
      if IsDefined(currentSettingsItem) {
        if currentSettingsItem.IsVisible() {
          currentSettingsItemType = currentSettingsItem.GetType();
          switch currentSettingsItemType {
            case ConfigVarType.Bool:
              currentItem = this.SpawnFromLocal(inkWidgetRef.Get(this.m_settingsOptionsList), n"settingsSelectorBool").GetController() as SettingsSelectorController;
              break;
            case ConfigVarType.Int:
              currentItem = this.SpawnFromLocal(inkWidgetRef.Get(this.m_settingsOptionsList), n"settingsSelectorInt").GetController() as SettingsSelectorController;
              break;
            case ConfigVarType.Float:
              currentItem = this.SpawnFromLocal(inkWidgetRef.Get(this.m_settingsOptionsList), n"settingsSelectorFloat").GetController() as SettingsSelectorController;
              break;
            case ConfigVarType.Name:
              currentItem = this.SpawnFromLocal(inkWidgetRef.Get(this.m_settingsOptionsList), n"settingsSelectorKeyBinding").GetController() as SettingsSelectorController;
              break;
            case ConfigVarType.IntList:
              currentItem = this.SpawnFromLocal(inkWidgetRef.Get(this.m_settingsOptionsList), n"settingsSelectorIntList").GetController() as SettingsSelectorController;
              break;
            case ConfigVarType.FloatList:
              currentItem = this.SpawnFromLocal(inkWidgetRef.Get(this.m_settingsOptionsList), n"settingsSelectorFloatList").GetController() as SettingsSelectorController;
              break;
            case ConfigVarType.StringList:
              currentItem = this.SpawnFromLocal(inkWidgetRef.Get(this.m_settingsOptionsList), n"settingsSelectorStringList").GetController() as SettingsSelectorController;
              break;
            case ConfigVarType.NameList:
              currentItem = this.SpawnFromLocal(inkWidgetRef.Get(this.m_settingsOptionsList), n"settingsSelectorNameList").GetController() as SettingsSelectorController;
              break;
            default:
              LogUIWarning("Cannot create UI settings drawer for " + NameToString(currentSettingsItem.GetDisplayName()));
          };
          if IsDefined(currentItem) {
            currentItem.Setup(currentSettingsItem, this.m_isPreGame);
            currentItem.RegisterToCallback(n"OnHoverOver", this, n"OnSettingHoverOver");
            currentItem.RegisterToCallback(n"OnHoverOut", this, n"OnSettingHoverOut");
            currentItem.Refresh();
            ArrayPush(this.m_settingsElements, currentItem);
          };
        };
      };
      i += 1;
    };
  }

  private final func PopulateCategorySettingsOptions(idx: Int32) -> Void {
    let categoryController: ref<SettingsCategoryController>;
    let i: Int32;
    let settingsCategory: SettingsCategory;
    let settingsSubCategory: SettingsCategory;
    ArrayClear(this.m_settingsElements);
    inkCompoundRef.RemoveAllChildren(this.m_settingsOptionsList);
    inkWidgetRef.SetVisible(this.m_descriptionText, false);
    if idx < 0 {
      idx = this.m_selectorCtrl.GetToggledIndex();
    };
    settingsCategory = this.m_data[idx];
    this.PopulateOptions(settingsCategory.options);
    i = 0;
    while i < ArraySize(settingsCategory.subcategories) {
      settingsSubCategory = settingsCategory.subcategories[i];
      categoryController = this.SpawnFromLocal(inkWidgetRef.Get(this.m_settingsOptionsList), n"settingsCategory").GetController() as SettingsCategoryController;
      if IsDefined(categoryController) {
        categoryController.Setup(settingsSubCategory.label);
      };
      this.PopulateOptions(settingsSubCategory.options);
      i += 1;
    };
    this.m_selectorCtrl.SetSelectedIndex(idx);
  }

}

// mod_settings/ModSettingsModule.reds

module ModSettingsModule

// mod_settings/ModSettingsNotificationListener.reds


public class ModSettingsNotificationListener extends ConfigNotificationListener {

  private let m_ctrl: wref<ModStngsMainGameController>;

  public final func RegisterController(ctrl: ref<ModStngsMainGameController>) -> Void {
    this.m_ctrl = ctrl;
  }

  public func OnNotify(status: ConfigNotificationType) -> Void {
    Log("SettingsNotificationListener::OnNotify");
    this.m_ctrl.OnSettingsNotify(status);
  }
}

public class ModSettingsVarListener extends ConfigVarListener {

  private let m_ctrl: wref<ModStngsMainGameController>;

  public final func RegisterController(ctrl: ref<ModStngsMainGameController>) -> Void {
    this.m_ctrl = ctrl;
  }

  public func OnVarModified(groupPath: CName, varName: CName, varType: ConfigVarType, reason: ConfigChangeReason) -> Void {
    Log("ModSettingsVarListener::OnVarModified");
    this.m_ctrl.OnVarModified(groupPath, varName, varType, reason);
  }
}

// mod_settings/_SettingsCategoryController.reds

@wrapMethod(SettingsCategoryController)
public final func Setup(label: CName) -> Void {
  if ModSettings.GetInstance().isActive {
    let labelString: String = GetLocalizedTextByKey(label);
    if StrLen(labelString) == 0 {
        labelString = ToString(label);
    };
    inkTextRef.SetText(this.m_label, labelString);
  } else {
    wrappedMethod(label);
  }
}

// mod_settings/_SettingsSelectorControllers.reds

// @wrapMethod(SettingsSelectorControllerBool)
// private func AcceptValue(forward: Bool) -> Void {
//     let ns = NativeSettings.GetInstance();
//     if ns.fromMods {
//         let ot = ns.GetOptionTable(this) as ConfigVarBool;
//         inkWidgetRef.SetVisible(this.m_onState, ot.GetValue());
//         inkWidgetRef.SetVisible(this.m_offState, !ot.GetValue());
//         // data.callback(data.state)
//     }
//     wrappedMethod(forward);
// }

// @wrapMethod(SettingsSelectorControllerInt)
// public func Refresh() -> Void {
//     let ns = NativeSettings.GetInstance();
//     if ns.fromMods {
//         let ot = ns.GetOptionTable(this) as ConfigVarInt;
//         if ot.GetValue() != this.m_newValue {
//             ot.SetValue(this.m_newValue);
//             inkTextRef.SetText(this.m_ValueText, ToString(this.m_newValue));
//             this.m_sliderController.ChangeValue(Cast<Float>(this.m_newValue));
//         }
//     } else {
//         wrappedMethod();
//     }
// }

// @wrapMethod(SettingsSelectorControllerInt)
// public func ChangeValue(forward: Bool) -> Void {
//     let ns = NativeSettings.GetInstance();
//     if ns.fromMods {
//         let ot = ns.GetOptionTable(this) as ConfigVarInt;
//         let step: Int32 = forward ? ot.GetStepValue() : -ot.GetStepValue();
//         this.m_newValue = Clamp(this.m_newValue + step, ot.GetMinValue(), ot.GetMaxValue());
//         this.Refresh();
//     } else {
//         wrappedMethod(forward);
//     }
// }

// @addMethod(SettingsSelectorController)
// public func SetupMod(entry: ref<ConfigVar>, isPreGame: Bool) -> Void {
//   this.m_SettingsEntry = entry;
//   this.m_IsPreGame = isPreGame;
//   this.m_varGroupPath = this.m_SettingsEntry.GetGroupPath();
//   this.m_varName = this.m_SettingsEntry.GetName();
//   this.BindSettings(entry);
// }

@wrapMethod(SettingsSelectorController)
public func Refresh() -> Void {
  if ModSettings.GetInstance().isActive {
    let i: Int32;
    let languageProvider: ref<inkLanguageOverrideProvider>;
    let modifiedSymbol: String;
    let text: String;
    let updatePolicy: ConfigVarUpdatePolicy;
    let wasModified: Bool;
    let size: Int32 = this.m_SettingsEntry.GetDisplayNameKeysSize();
    if size > 0 {
      text = NameToString(this.m_SettingsEntry.GetDisplayName());
      i = 0;
      while i < size {
        text = StrReplace(text, "%", GetLocalizedTextByKey(this.m_SettingsEntry.GetDisplayNameKey(i)));
        i += 1;
      };
    } else {
      text = GetLocalizedTextByKey(this.m_SettingsEntry.GetDisplayName());
    };
    updatePolicy = this.m_SettingsEntry.GetUpdatePolicy();
    if Equals(text, "") {
      text = NameToString(this.m_SettingsEntry.GetDisplayName());
    };
    if Equals(updatePolicy, ConfigVarUpdatePolicy.ConfirmationRequired) {
      modifiedSymbol = "*";
      wasModified = this.m_SettingsEntry.HasRequestedValue();
    } else {
      if Equals(updatePolicy, ConfigVarUpdatePolicy.RestartRequired) || Equals(updatePolicy, ConfigVarUpdatePolicy.LoadLastCheckpointRequired) {
        modifiedSymbol = "!";
        wasModified = this.m_SettingsEntry.HasRequestedValue() || this.m_SettingsEntry.WasModifiedSinceLastSave();
      } else {
        modifiedSymbol = "";
        wasModified = false;
      };
    };
    languageProvider = inkWidgetRef.GetUserData(this.m_LabelText, n"inkLanguageOverrideProvider") as inkLanguageOverrideProvider;
    languageProvider.SetLanguage(scnDialogLineLanguage.Origin);
    inkTextRef.UpdateLanguageResources(this.m_LabelText, false);
    inkTextRef.SetText(this.m_LabelText, text);
    inkWidgetRef.SetVisible(this.m_ModifiedFlag, wasModified);
    inkTextRef.SetText(this.m_ModifiedFlag, modifiedSymbol);
  } else {
    wrappedMethod();
  }
}

// @wrapMethod(SettingsSelectorControllerBool)
// private func AcceptValue(forward: Bool) -> Void {
//   wrappedMethod(forward);
//   this.Refresh();
// }

@wrapMethod(SettingsSelectorController)
protected cb func OnLeft(e: ref<inkPointerEvent>) -> Bool {
  let og = wrappedMethod(e);
  if ModSettings.GetInstance().isActive {
    this.Refresh();
  }
  return og;
}

@wrapMethod(SettingsSelectorController)
protected cb func OnRight(e: ref<inkPointerEvent>) -> Bool {
  let og = wrappedMethod(e);
  if ModSettings.GetInstance().isActive {
    this.Refresh();
  }
  return og;
}

// @wrapMethod(SettingsSelectorControllerListName)
// public func Refresh() -> Void {
//   if ModSettings.GetInstance().isActive {
//     let params: ref<inkTextParams>;
//     super.Refresh();
//     this.UpdateValueTextLanguageResources();
//     if !this.m_realValue.ListHasDisplayValues() {
//       inkTextRef.SetText(this.m_ValueText, ToString(this.m_realValue.GetValueFor(this.m_currentIndex)));
//     } else {
//       if Equals(this.m_additionalText, n"") {
//         inkTextRef.SetText(this.m_ValueText, ToString(this.m_realValue.GetDisplayValue(this.m_currentIndex)));
//       } else {
//         params = new inkTextParams();
//         // params.AddLocalizedString("description", GetLocalizedTextByKey(this.m_realValue.GetDisplayValue(this.m_currentIndex)));
//         params.AddString("description", ToString(this.m_realValue.GetDisplayValue(this.m_currentIndex)));
//         params.AddLocalizedString("additional_text", ToString(this.m_additionalText));
//         inkTextRef.SetLocalizedTextScript(this.m_ValueText, "LocKey#76949", params);
//       };
//     };
//     this.SelectDot(this.m_currentIndex);
//   } else {
//     wrappedMethod();
//   }
// }






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

// mod_settings/_deathMenu.reds

@replaceMethod(DeathMenuGameController)
private func PopulateMenuItemList() -> Void {
    if this.GetSystemRequestsHandler().HasLastCheckpoint() {
        this.AddMenuItem(GetLocalizedText("UI-ScriptExports-LoadLastSavegame"), PauseMenuAction.QuickLoad);
    };
    this.AddMenuItem(GetLocalizedText("UI-ScriptExports-LoadGame0"), n"OnSwitchToLoadGame");
    this.AddMenuItem(GetLocalizedText("UI-Labels-Settings"), n"OnSwitchToSettings");
    this.AddMenuItem("Mod Settings", n"OnSwitchToModSettings");
    this.AddMenuItem(GetLocalizedText("UI-Labels-ExitToMenu"), PauseMenuAction.ExitToMainMenu);
    this.m_menuListController.Refresh();
    this.SetCursorOverWidget(inkCompoundRef.GetWidgetByIndex(this.m_menuList, 0));
}

// mod_settings/_pauseMenu.reds

@replaceMethod(PauseMenuGameController)
private func PopulateMenuItemList() -> Void {
    this.AddMenuItem(GetLocalizedText("UI-Labels-Resume"), n"OnClosePauseMenu");
    if !IsFinal() || UseProfiler() {
        this.AddMenuItem("OPEN DEBUG MENU", n"OnOpenDebugHubMenu");
    };
    this.AddMenuItem(GetLocalizedText("UI-ResourceExports-SaveGame"), PauseMenuAction.Save);
    if this.m_savesCount > 0 {
        this.AddMenuItem(GetLocalizedText("UI-ScriptExports-LoadGame0"), n"OnSwitchToLoadGame");
    };
    this.AddMenuItem(GetLocalizedText("UI-Labels-Settings"), n"OnSwitchToSettings");
    this.AddMenuItem("Mod Settings", n"OnSwitchToModSettings");
    this.AddMenuItem(GetLocalizedText("UI-DLC-MenuTitle"), n"OnSwitchToDlc");
    this.AddMenuItem(GetLocalizedText("UI-Labels-Credits"), n"OnSwitchToCredits");
    if TrialHelper.IsInPS5TrialMode() {
        this.AddMenuItem(GetLocalizedText("UI-Notifications-Ps5TrialBuyMenuItem"), n"OnBuyGame");
    };
    this.AddMenuItem(GetLocalizedText("UI-Labels-ExitToMenu"), PauseMenuAction.ExitToMainMenu);
    this.m_menuListController.Refresh();
    this.SetCursorOverWidget(inkCompoundRef.GetWidgetByIndex(this.m_menuList, 0));
}

// mod_settings/_pauseScenario.reds

@addMethod(MenuScenario_PauseMenu )
protected cb func OnSwitchToModSettings() -> Bool {
    this.SwitchMenu(n"mod_settings_main");
}

// mod_settings/_preGameScenarios.reds

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



public class MenuScenario_ModSettings extends MenuScenario_PreGameSubMenu {

  protected cb func OnEnterScenario(prevScenario: CName, userData: ref<IScriptable>) -> Bool {
    super.OnEnterScenario(prevScenario, userData);
    this.GetMenusState().OpenMenu(n"mod_settings_main", userData);
  }

  protected cb func OnLeaveScenario(nextScenario: CName) -> Bool {
    super.OnLeaveScenario(nextScenario);
    this.GetMenusState().CloseMenu(n"mod_settings_main");
  }

  protected func OnSubmenuOpen() -> Void {
    this.GetMenusState().CloseMenu(n"mod_settings_main");
  }

  protected cb func OnSettingsBack() -> Bool {
    if NotEquals(this.m_currSubMenuName, n"") {
      this.CloseSubMenu();
      this.GetMenusState().OpenMenu(n"mod_settings_main");
    } else {
      this.CloseSettings(false);
    };
  }

  protected cb func OnCloseModSettingsScreen() -> Bool {
    this.CloseSettings(true);
  }

  private final func CloseSettings(forceCloseSettings: Bool) -> Void {
    let menuState: wref<inkMenusState> = this.GetMenusState();
    if forceCloseSettings {
      menuState.CloseMenu(n"mod_settings_main");
      if NotEquals(this.m_currSubMenuName, n"") {
        if !menuState.DispatchEvent(this.m_currSubMenuName, n"OnBack") {
          this.CloseSubMenu();
        };
      } else {
        this.SwitchToScenario(this.m_prevScenario);
      };
    } else {
      menuState.DispatchEvent(n"mod_settings_main", n"OnBack");
    };
  }

  protected cb func OnMainMenuBack() -> Bool {
    this.SwitchToScenario(this.m_prevScenario);
  }
}

// mod_settings/_singleplayerMenu.reds

@replaceMethod(SingleplayerMenuGameController)
private func PopulateMenuItemList() -> Void {
    if this.m_savesCount > 0 {
      this.AddMenuItem(GetLocalizedText("UI-ScriptExports-Continue0"), PauseMenuAction.QuickLoad);
    };
    this.AddMenuItem(GetLocalizedText("UI-ScriptExports-NewGame0"), n"OnNewGame");
    this.AddMenuItem(GetLocalizedText("UI-ScriptExports-LoadGame0"), n"OnLoadGame");
    this.AddMenuItem(GetLocalizedText("UI-Labels-Settings"), n"OnSwitchToSettings");
    this.AddMenuItem("Mod Settings", n"OnSwitchToModSettings");
    this.AddMenuItem(GetLocalizedText("UI-DLC-MenuTitle"), n"OnSwitchToDlc");
    this.AddMenuItem(GetLocalizedText("UI-Labels-Credits"), n"OnSwitchToCredits");
    if TrialHelper.IsInPS5TrialMode() {
      this.AddMenuItem(GetLocalizedText("UI-Notifications-Ps5TrialBuyMenuItem"), n"OnBuyGame");
    };
    if !IsFinal() || UseProfiler() {
      this.AddMenuItem("DEBUG NEW GAME", n"OnDebug");
    };
    this.m_menuListController.Refresh();
    this.SetCursorOverWidget(inkCompoundRef.GetWidgetByIndex(this.m_menuList, 0));
}

