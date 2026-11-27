// queued by OnModVariableChangeRequested: the rows a mod lists may have changed (ModSettings.dependency)
public class ModStngsRowsChangedEvent extends Event {}

// queued while a page or the search results still have rows to spawn: the next batch, a frame later. A page shown
// since then has its own generation, and the old page's events do nothing.
public class ModStngsSpawnRowsEvent extends Event {
  public let generation: Int32;
}

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
  // private let m_settingsListener: ref<ModSettingsVarListener>;
  // private let m_settingsNotificationListener: ref<ModSettingsNotificationListener>;
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
  private let m_confirmationToken: ref<inkGameNotificationToken>;
  private let m_pendingDefaultsMod: CName;

  // the search bar above the mod list, copied from 2.5's mod.io browser (modio_browser_menu.inkwidget)
  // (found by path: the page's widget binds only the fields of the game's own settings controller)
  private let m_searchBar: wref<inkWidget>;
  private let m_searchInput: wref<inkTextInput>;
  private let m_searchPlaceholder: wref<inkText>;
  private let m_searchFocused: Bool;
  // the click that focused the bar also reaches OnButtonRelease, which would unfocus it again
  private let m_ignoreNextUnfocus: Bool;
  // the lowercased words of the search, all of which a setting must match; empty when not searching
  private let m_searchTerms: array<String>;
  // whether each setting in m_data, in order, matches the search text (group path and name alongside): reading the
  // localized text of every setting again on each value change cost 25 ms with 2,400 settings
  private let m_matchGroups: array<CName>;
  private let m_matchNames: array<CName>;
  private let m_matchFlags: array<Bool>;
  private let m_matchWalk: Int32;
  private let m_nextGroups: array<CName>;
  private let m_nextNames: array<CName>;
  private let m_nextFlags: array<Bool>;

  // the rows still to spawn, in order: a header (m_pendingHeaders) where the option is null. A row costs about
  // 0.3 ms to spawn, so a long page spawns over several frames instead of stopping one
  private let m_pendingHeaders: array<CName>;
  private let m_pendingOptions: array<ref<ConfigVar>>;
  private let m_pendingNext: Int32;
  private let m_spawnGeneration: Int32;
  // the previous page's rows, first in the list: hidden at once and removed over the next frames, as removing 240
  // rows at once took about 40 ms
  private let m_staleRows: Int32;

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
    this.FindSearchWidgets();
    this.m_searchPlaceholder.SetText(GetLocalizedText("UI-Modio-FiltersSorting-Search"));
    this.m_searchInput.SetMaxChars(40u);
    this.m_searchInput.RegisterToCallback(n"OnTextChanged", this, n"OnSearchTextChanged");
    this.m_searchBar.RegisterToCallback(n"OnRelease", this, n"OnSearchBarRelease");
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
    this.m_searchInput.UnregisterFromCallback(n"OnTextChanged", this, n"OnSearchTextChanged");
    this.m_searchBar.UnregisterFromCallback(n"OnRelease", this, n"OnSearchBarRelease");

    ModSettings.UnregisterListenerToModifications(this);
    this.m_confirmationToken = null;

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
    if ms.changesRequested {
      this.EnableApplyButton();
      this.EnableResetButton();
    } else {
      this.DisableApplyButton();
      this.DisableResetButton();
    }
  }

  public func OnModVariableChangeRequested(groupPath: CName, varName: CName) -> Void {
    let i: Int32;
    let item: ref<SettingsSelectorController>;
    let size: Int32;
    // LogChannel(n"DEBUG", "[VAR] modified groupPath: " + NameToString(groupPath) + " varName: " + NameToString(varName));
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
    // the change can show or hide rows that depend on it; rebuilt a frame later, not under the row's own callback
    this.QueueEvent(new ModStngsRowsChangedEvent());
  }

  protected cb func OnRowsChanged(evt: ref<ModStngsRowsChangedEvent>) -> Bool {
    let idx = this.m_selectorCtrl.GetToggledIndex();
    if !this.IsSearching() && (idx < 0 || idx >= ArraySize(this.m_data)) {
      return false;
    }
    // the variables are read again (a hidden dependent is left out of GetVars); the rows are rebuilt only when
    // one showed or hid. While searching, settings matched before keep their match: no localized text is read
    let shown = this.ShownKeys(idx);
    this.PopulateSettingsData();
    if this.IsSearching() {
      this.ComputeSearchMatches();
    }
    if !ModStngsMainGameController.SameNames(this.ShownKeys(idx), shown) {
      this.PopulateCategorySettingsOptions(this.IsSearching() ? -1 : idx);
    }
  }

  // the rows the page lists, group path and name in turn: mod page idx's, or the search's
  private final func ShownKeys(idx: Int32) -> array<CName> {
    let out: array<CName>;
    if this.IsSearching() {
      if ArraySize(this.m_matchNames) != ModStngsMainGameController.VarCount(this.m_data) {
        this.ComputeSearchMatches();
      }
      let k = 0;
      for mod in this.m_data {
        for v in mod.options {
          if this.m_matchFlags[k] && IsDefined(v) && v.IsVisible() {
            ArrayPush(out, v.GetGroupPath());
            ArrayPush(out, v.GetName());
          }
          k += 1;
        }
        for sub in mod.subcategories {
          for v in sub.options {
            if this.m_matchFlags[k] && IsDefined(v) && v.IsVisible() {
              ArrayPush(out, v.GetGroupPath());
              ArrayPush(out, v.GetName());
            }
            k += 1;
          }
        }
      }
      return out;
    }
    if idx < 0 || idx >= ArraySize(this.m_data) {
      return out;
    }
    let category = this.m_data[idx];
    for v in category.options {
      if IsDefined(v) && v.IsVisible() {
        ArrayPush(out, v.GetGroupPath());
        ArrayPush(out, v.GetName());
      }
    }
    for sub in category.subcategories {
      for v in sub.options {
        if IsDefined(v) && v.IsVisible() {
          ArrayPush(out, v.GetGroupPath());
          ArrayPush(out, v.GetName());
        }
      }
    }
    return out;
  }

  private static func SameNames(a: array<CName>, b: array<CName>) -> Bool {
    if ArraySize(a) != ArraySize(b) {
      return false;
    }
    let i = 0;
    while i < ArraySize(a) {
      if NotEquals(a[i], b[i]) {
        return false;
      }
      i += 1;
    }
    return true;
  }

  private static func VarCount(data: array<SettingsCategory>) -> Int32 {
    let count = 0;
    for mod in data {
      count += ArraySize(mod.options);
      for sub in mod.subcategories {
        count += ArraySize(sub.options);
      }
    }
    return count;
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
    // Back first leaves the search bar, as in the mod.io browser; the next one closes the page
    if this.m_searchFocused {
      this.LoseSearchFocus();
      return true;
    }
    // if !StatusEffectSystem.ObjectHasStatusEffectWithTag(this.GetPlayerControlledObject(), n"LockInHubMenu") {
      this.m_closeSettingsRequest = true;
      this.CheckSettings();
    // };
  }

  private final func RequestClose() -> Void {
    this.m_menuEventDispatcher.SpawnEvent(n"OnCloseModSettingsScreen");
  }

  // Asks before restoring defaults; only the currently selected mod is affected
  private final func RequestRestoreDefaults() -> Void {
    let index: Int32 = this.m_selectorCtrl.GetToggledIndex();
    if IsDefined(this.m_confirmationToken) || index < 0 || index >= ArraySize(this.m_data) {
      return;
    };
    this.m_pendingDefaultsMod = this.m_data[index].label;
    let modLabel: String = GetLocalizedTextByKey(this.m_pendingDefaultsMod);
    if StrLen(modLabel) == 0 {
      modLabel = ToString(this.m_pendingDefaultsMod);
    };
    let title: String = GetLocalizedTextByKey(n"UI-UserActions-RestoreDefaults");
    if StrLen(title) == 0 {
      title = "Restore Defaults";
    };
    this.m_confirmationToken = GenericMessageNotification.Show(this, title, "Restore the default settings for " + modLabel + "? Other mods are not affected.", GenericMessageNotificationType.YesNo);
    this.m_confirmationToken.RegisterListener(this, n"OnRestoreDefaultsConfirmed");
    // this.m_settings.RequestRestoreDefaultDialog(this.m_isPreGame, false, groupPath);
  }

  protected cb func OnRestoreDefaultsConfirmed(data: ref<inkGameNotificationData>) -> Bool {
    let result: ref<GenericMessageNotificationCloseData> = data as GenericMessageNotificationCloseData;
    this.m_confirmationToken = null;
    if IsDefined(result) && Equals(result.result, GenericMessageNotificationResult.Yes) {
      ModSettings.RestoreDefaults(this.m_pendingDefaultsMod);
    };
  }

  protected cb func OnResetConfirmed(data: ref<inkGameNotificationData>) -> Bool {
    let result: ref<GenericMessageNotificationCloseData> = data as GenericMessageNotificationCloseData;
    this.m_confirmationToken = null;
    if IsDefined(result) && Equals(result.result, GenericMessageNotificationResult.Yes) {
      this.m_resetSettingsRequest = true;
      this.CheckSettings();
    };
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
    // picking a mod leaves the search for that mod's page
    this.ClearSearch();
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
    this.ForgetSearchMatches();
    this.PopulateCategories(idx);
    this.PopulateCategorySettingsOptions(idx);
    this.PopulateHints();
  }

  private final func PopulateHints() -> Void {
    this.m_buttonHintsController.ClearButtonHints();
    this.m_buttonHintsController.AddButtonHint(n"select", "UI-UserActions-Select");
    this.m_buttonHintsController.AddButtonHint(n"back", "Common-Access-Close");
    if !this.m_searchFocused {
      this.m_buttonHintsController.AddButtonHint(n"controller_settings", "UI-Modio-FiltersSorting-Search");
    };
    // Defaults resets the selected mod, which is unclear while the results span several
    if !this.m_isDlcSettings && !this.IsSearching() {
      this.m_buttonHintsController.AddButtonHint(n"restore_default_settings", "UI-UserActions-RestoreDefaults");
    };
  }

  private final func OnApplyButton() -> Void {
    this.m_isKeybindingAlertEnabled = this.m_settings.GetMenuIndex() == 8;
    if !this.IsApplyButtonEnabled() {
      return;
    };
    // Log("OnApplyButton");
    if this.m_settings.NeedsConfirmation() {
      this.m_settings.ConfirmChanges();
    } else {
      this.CheckSettings();
    };
  }

  // Asks before discarding the unsaved changes
  private final func OnResetButton() -> Void {
    if !this.IsResetButtonEnabled() || IsDefined(this.m_confirmationToken) {
      return;
    };
    // Log("OnResetButton");
    this.m_confirmationToken = GenericMessageNotification.Show(this, "Reset", "Discard the unsaved changes to your mod settings?", GenericMessageNotificationType.YesNo);
    this.m_confirmationToken.RegisterListener(this, n"OnResetConfirmed");
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
    if this.m_searchFocused {
      // keys typed into the bar are not the page's shortcuts; a click elsewhere leaves the bar
      if (evt.IsAction(n"click") || evt.IsAction(n"tap")) && evt.GetCurrentTarget() != this.m_searchInput {
        if this.m_ignoreNextUnfocus {
          this.m_ignoreNextUnfocus = false;
        } else {
          this.LoseSearchFocus();
        };
      };
      return false;
    };
    if evt.IsAction(n"controller_settings") {
      this.FocusSearch(false);
    } else if evt.IsAction(n"prior_menu") {
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
          if !this.IsSearching() {
            this.RequestRestoreDefaults();
          };
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
    if !IsDefined(evt) {
      return false;
    }
    let target = evt.GetCurrentTarget();
    if !IsDefined(target) {
      return false;
    }
    let currentItem = target.GetController() as SettingsSelectorController;
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

  // rows spawned in the first frame, and in each frame after until the page is complete: enough to fill the screen
  private static func RowsPerFrame() -> Int32 = 30;

  private final func QueueHeader(label: CName) -> Void {
    ArrayPush(this.m_pendingHeaders, label);
    ArrayPush(this.m_pendingOptions, null);
  }

  // the options a mod shows, as rows to spawn
  private final func PopulateOptions(options: array<ref<ConfigVar>>) -> Void {
    for option in options {
      if IsDefined(option) && option.IsVisible() {
        ArrayPush(this.m_pendingHeaders, n"None");
        ArrayPush(this.m_pendingOptions, option);
      };
    };
  }

  private final func SpawnHeader(label: CName) -> Void {
    let categoryController = this.SpawnFromLocal(inkWidgetRef.Get(this.m_settingsOptionsList), n"settingsCategory").GetController() as SettingsCategoryController;
    if IsDefined(categoryController) {
      categoryController.Setup(label);
    };
  }

  private final func SpawnOption(currentSettingsItem: ref<ConfigVar>) -> Void {
    let currentItem: wref<SettingsSelectorController>;
    let list = inkWidgetRef.Get(this.m_settingsOptionsList);
    switch currentSettingsItem.GetType() {
      case ConfigVarType.Bool:
        currentItem = this.SpawnFromLocal(list, n"settingsSelectorBool").GetController() as SettingsSelectorController;
        break;
      case ConfigVarType.Int:
        currentItem = this.SpawnFromLocal(list, n"settingsSelectorInt").GetController() as SettingsSelectorController;
        break;
      case ConfigVarType.Float:
        currentItem = this.SpawnFromLocal(list, n"settingsSelectorFloat").GetController() as SettingsSelectorController;
        break;
      case ConfigVarType.Name:
        currentItem = this.SpawnFromLocal(list, n"settingsSelectorKeyBinding").GetController() as SettingsSelectorController;
        break;
      case ConfigVarType.IntList:
        currentItem = this.SpawnFromLocal(list, n"settingsSelectorIntList").GetController() as SettingsSelectorController;
        break;
      case ConfigVarType.FloatList:
        currentItem = this.SpawnFromLocal(list, n"settingsSelectorFloatList").GetController() as SettingsSelectorController;
        break;
      case ConfigVarType.StringList:
        currentItem = this.SpawnFromLocal(list, n"settingsSelectorStringList").GetController() as SettingsSelectorController;
        break;
      case ConfigVarType.NameList:
        currentItem = this.SpawnFromLocal(list, n"settingsSelectorNameList").GetController() as SettingsSelectorController;
        break;
      default:
    };
    if IsDefined(currentItem) {
      currentItem.Setup(currentSettingsItem, this.m_isPreGame);
      currentItem.RegisterToCallback(n"OnHoverOver", this, n"OnSettingHoverOver");
      currentItem.RegisterToCallback(n"OnHoverOut", this, n"OnSettingHoverOut");
      ArrayPush(this.m_settingsElements, currentItem);
    };
  }

  private static func StaleRowsPerFrame() -> Int32 = 60;

  // hides every row the list holds; SpawnQueuedRows removes them a batch a frame
  private final func HideRows() -> Void {
    let list = inkWidgetRef.Get(this.m_settingsOptionsList) as inkCompoundWidget;
    this.m_staleRows = list.GetNumChildren();
    let i = 0;
    while i < this.m_staleRows {
      list.GetWidgetByIndex(i).SetVisible(false);
      i += 1;
    };
  }

  // removes a batch of the previous page's rows, the last first, so the rows after them move least
  private final func RemoveStaleRows() -> Void {
    let list = inkWidgetRef.Get(this.m_settingsOptionsList) as inkCompoundWidget;
    let n = Min(ModStngsMainGameController.StaleRowsPerFrame(), this.m_staleRows);
    while n > 0 {
      this.m_staleRows -= 1;
      list.RemoveChildByIndex(this.m_staleRows);
      n -= 1;
    };
  }

  // spawns the next batch of queued rows (and from the second frame on removes a batch of the previous page's), and
  // queues the batch after for the next frame
  private final func SpawnQueuedRows() -> Void {
    let end = Min(this.m_pendingNext + ModStngsMainGameController.RowsPerFrame(), ArraySize(this.m_pendingOptions));
    while this.m_pendingNext < end {
      let option = this.m_pendingOptions[this.m_pendingNext];
      if IsDefined(option) {
        this.SpawnOption(option);
      } else {
        this.SpawnHeader(this.m_pendingHeaders[this.m_pendingNext]);
      };
      this.m_pendingNext += 1;
    };
    if this.m_pendingNext < ArraySize(this.m_pendingOptions) || this.m_staleRows > 0 {
      let evt = new ModStngsSpawnRowsEvent();
      evt.generation = this.m_spawnGeneration;
      this.QueueEvent(evt);
    } else {
      ArrayClear(this.m_pendingHeaders);
      ArrayClear(this.m_pendingOptions);
      this.m_pendingNext = 0;
    };
  }

  protected cb func OnSpawnRows(evt: ref<ModStngsSpawnRowsEvent>) -> Bool {
    if evt.generation == this.m_spawnGeneration {
      this.RemoveStaleRows();
      this.SpawnQueuedRows();
    };
  }

  // whether rows are still spawning (for the test tools)
  public final func IsSpawningRows() -> Bool = ArraySize(this.m_pendingOptions) > 0 || this.m_staleRows > 0;

  private final func PopulateCategorySettingsOptions(idx: Int32) -> Void {
    let i: Int32;
    let settingsCategory: SettingsCategory;
    ArrayClear(this.m_settingsElements);
    ArrayClear(this.m_pendingHeaders);
    ArrayClear(this.m_pendingOptions);
    this.m_pendingNext = 0;
    this.m_spawnGeneration += 1;
    this.HideRows();
    inkTextRef.SetText(this.m_descriptionText, "");
    inkWidgetRef.SetVisible(this.m_descriptionText, false);
    if this.IsSearching() {
      this.PopulateSearchResults();
      this.SpawnQueuedRows();
      return;
    };
    if idx < 0 {
      idx = this.m_selectorCtrl.GetToggledIndex();
    };
    // no mod registers settings, or the list is not toggled yet
    if idx < 0 || idx >= ArraySize(this.m_data) {
      return;
    };
    settingsCategory = this.m_data[idx];
    this.PopulateOptions(settingsCategory.options);
    i = 0;
    while i < ArraySize(settingsCategory.subcategories) {
      this.QueueHeader(settingsCategory.subcategories[i].label);
      this.PopulateOptions(settingsCategory.subcategories[i].options);
      i += 1;
    };
    this.SpawnQueuedRows();
    this.m_selectorCtrl.SetSelectedIndex(idx);
  }

  // Search: every mod's settings whose name, description, mod or category hold all the words typed, under a
  // "Mod · Category" header each, as live rows (changes go through Apply, Reset and Back as on a mod's page).

  public final func IsSearching() -> Bool = ArraySize(this.m_searchTerms) > 0;

  public final func IsSearchFocused() -> Bool = this.m_searchFocused;

  private final func FindSearchWidgets() -> Void {
    let root = this.GetRootCompoundWidget();
    this.m_searchBar = root.GetWidget(n"mod_settings/search_bar");
    this.m_searchInput = root.GetWidget(n"mod_settings/search_bar/SearchContent/TextContent/search_input") as inkTextInput;
    this.m_searchPlaceholder = root.GetWidget(n"mod_settings/search_bar/SearchContent/TextContent/search_placeholder") as inkText;
  }

  protected cb func OnSearchBarRelease(evt: ref<inkPointerEvent>) -> Bool {
    if evt.IsAction(n"click") || evt.IsAction(n"tap") {
      this.FocusSearch(true);
    };
  }

  private final func FocusSearch(fromClick: Bool) -> Void {
    if this.m_searchFocused {
      return;
    };
    if !IsDefined(this.m_searchInput) {
      this.FindSearchWidgets();
    };
    this.m_searchInput.RequestFocus();
    this.m_searchFocused = true;
    this.m_ignoreNextUnfocus = fromClick;
    this.m_searchPlaceholder.SetVisible(false);
    // a hovered row takes A/D and the arrows as its own shortcuts: none while typing
    inkWidgetRef.SetInteractive(this.m_settingsOptionsList, false);
    this.PopulateHints();
    this.PlaySound(n"Button", n"OnPress");
  }

  private final func LoseSearchFocus() -> Void {
    if !this.m_searchFocused {
      return;
    };
    this.m_searchInput.RequestUnfocus();
    this.m_searchFocused = false;
    this.m_ignoreNextUnfocus = false;
    this.m_searchPlaceholder.SetVisible(StrLen(this.SearchText()) == 0);
    inkWidgetRef.SetInteractive(this.m_settingsOptionsList, true);
    this.PopulateHints();
  }

  private final func SearchText() -> String = this.m_searchInput.GetText();

  // empties the bar and goes back to the mod pages
  private final func ClearSearch() -> Void {
    this.LoseSearchFocus();
    if StrLen(this.SearchText()) > 0 {
      this.m_searchInput.SetText("");
    };
    this.m_searchPlaceholder.SetVisible(true);
    this.SetSearchTerms([]);
  }

  protected cb func OnSearchTextChanged(text: String) -> Bool {
    let terms: array<String>;
    if !this.m_searchFocused {
      this.m_searchPlaceholder.SetVisible(StrLen(text) == 0);
    };
    // one letter matches nearly every setting and spawns a row for each: results start at two
    if StrLen(text) >= 2 {
      for word in StrSplit(StrLower(text), " ") {
        if StrLen(word) > 0 {
          ArrayPush(terms, word);
        };
      };
    };
    this.SetSearchTerms(terms);
  }

  private final func SetSearchTerms(terms: array<String>) -> Void {
    let wasSearching = this.IsSearching();
    if Equals(ModStngsMainGameController.Joined(terms), ModStngsMainGameController.Joined(this.m_searchTerms)) {
      return;
    };
    this.m_searchTerms = terms;
    this.ForgetSearchMatches();
    // Defaults resets the selected mod, which is unclear while the results span several
    inkWidgetRef.SetVisible(this.m_defaultButton, !this.IsSearching());
    if NotEquals(wasSearching, this.IsSearching()) {
      this.PopulateHints();
      this.DimModList(this.IsSearching());
    };
    this.PopulateCategorySettingsOptions(-1);
    (inkWidgetRef.GetController(this.m_scrollPanel) as inkScrollController).SetScrollPosition(0.00);
  }

  // While results show, the mod list stands aside: no entry looks like the open page (SettingsCategoryItem's
  // "Selected" state), and the names turn grey and fade. A name's colour and opacity are bound to the style
  // (MenuLabel.MainColor and MainOpacity in hub_menu_style.inkstyle), which wins over opacity set on the entry, the
  // list or its scroll area, and its HDR red still read bright at 0.3: the bindings are swapped while searching and
  // put back after, as the game's own sliders rebind their colours.
  private final func DimModList(dim: Bool) -> Void {
    let list = this.GetRootCompoundWidget().GetWidget(n"mod_settings/mod_flex/mod_scroller/mods") as inkCompoundWidget;
    if !IsDefined(list) {
      return;
    };
    let toggled = this.m_selectorCtrl.GetToggledIndex();
    let i = 0;
    while i < list.GetNumChildren() {
      let item = list.GetWidgetByIndex(i) as inkCompoundWidget;
      item.SetState(!dim && i == toggled ? n"Selected" : n"Default");
      let name = item.GetWidget(n"txtValueHighlight");
      if dim {
        name.BindProperty(n"tintColor", n"MainColors.Grey");
        name.UnbindProperty(n"opacity");
        name.SetOpacity(0.4);
      } else {
        name.BindProperty(n"tintColor", n"MenuLabel.MainColor");
        name.BindProperty(n"opacity", n"MenuLabel.MainOpacity");
      };
      i += 1;
    };
  }

  private static func Joined(words: array<String>) -> String {
    let out = "";
    for word in words {
      out += word + " ";
    };
    return out;
  }

  // a name or key as the page shows it: its localized text, or the name itself when it is not a key
  private static func Shown(label: CName) -> String {
    let text = GetLocalizedTextByKey(label);
    return StrLen(text) > 0 ? text : NameToString(label);
  }

  private final func Matches(v: ref<ConfigVar>, context: String) -> Bool {
    if !IsDefined(v) {
      return false;
    };
    let description = Equals(v.GetDescription(), n"None") ? "" : ModStngsMainGameController.Shown(v.GetDescription());
    let text = StrLower(ModStngsMainGameController.Shown(v.GetDisplayName()) + " " + description + " " + context);
    for term in this.m_searchTerms {
      if StrFindFirst(text, term) < 0 {
        return false;
      };
    };
    return true;
  }

  private final func ForgetSearchMatches() -> Void {
    ArrayClear(this.m_matchGroups);
    ArrayClear(this.m_matchNames);
    ArrayClear(this.m_matchFlags);
  }

  // whether each setting in m_data matches the search text. A setting matched before keeps its result: m_data is
  // in the same order each time it is read, with only hidden dependents left out, so the previous results are
  // walked alongside, a short way ahead for the ones that were left out
  private final func ComputeSearchMatches() -> Void {
    ArrayClear(this.m_nextGroups);
    ArrayClear(this.m_nextNames);
    ArrayClear(this.m_nextFlags);
    this.m_matchWalk = 0;
    for mod in this.m_data {
      let modLabel = ModStngsMainGameController.Shown(mod.label);
      for v in mod.options {
        this.MatchOne(v, modLabel);
      };
      for sub in mod.subcategories {
        let context = modLabel + " " + ModStngsMainGameController.Shown(sub.label);
        for v in sub.options {
          this.MatchOne(v, context);
        };
      };
    };
    this.m_matchGroups = this.m_nextGroups;
    this.m_matchNames = this.m_nextNames;
    this.m_matchFlags = this.m_nextFlags;
  }

  private final func MatchOne(v: ref<ConfigVar>, context: String) -> Void {
    let group = IsDefined(v) ? v.GetGroupPath() : n"None";
    let name = IsDefined(v) ? v.GetName() : n"None";
    let end = Min(this.m_matchWalk + 64, ArraySize(this.m_matchNames));
    let j = this.m_matchWalk;
    while j < end && !(Equals(this.m_matchNames[j], name) && Equals(this.m_matchGroups[j], group)) {
      j += 1;
    };
    let matched: Bool;
    if j < end {
      matched = this.m_matchFlags[j];
      this.m_matchWalk = j + 1;
    } else {
      matched = this.Matches(v, context);
    };
    ArrayPush(this.m_nextGroups, group);
    ArrayPush(this.m_nextNames, name);
    ArrayPush(this.m_nextFlags, matched);
  }

  // past this many results the rest are counted, not shown: spawning and clearing every row of a search that
  // matched 2,400 settings stopped the game for over a second
  private static func SearchRowCap() -> Int32 = 100;

  private final func PopulateSearchResults() -> Void {
    this.ComputeSearchMatches();
    let cap = ModStngsMainGameController.SearchRowCap();
    let count = 0;
    let shown = 0;
    let k = 0;
    for mod in this.m_data {
      let modLabel = ModStngsMainGameController.Shown(mod.label);
      let found: array<ref<ConfigVar>>;
      for v in mod.options {
        if this.m_matchFlags[k] && IsDefined(v) && v.IsVisible() {
          ArrayPush(found, v);
        };
        k += 1;
      };
      shown += this.QueueResults(modLabel, found, cap - shown);
      count += ArraySize(found);
      for sub in mod.subcategories {
        ArrayClear(found);
        for v in sub.options {
          if this.m_matchFlags[k] && IsDefined(v) && v.IsVisible() {
            ArrayPush(found, v);
          };
          k += 1;
        };
        shown += this.QueueResults(modLabel + " · " + ModStngsMainGameController.Shown(sub.label), found, cap - shown);
        count += ArraySize(found);
      };
    };
    if count == 0 {
      this.QueueHeader(StringToName(GetLocalizedText("UI-ModSettings-NoResults")));
    } else if count > shown {
      // first, where it is seen: that more match than are shown
      let more = GetLocalizedText("UI-ModSettings-MoreResults");
      more = StrReplace(more, "{shown}", ToString(shown));
      more = StrReplace(more, "{total}", ToString(count));
      ArrayInsert(this.m_pendingHeaders, 0, StringToName(more));
      ArrayInsert(this.m_pendingOptions, 0, null);
    };
  }

  // queues up to room of the found settings under a header (not a localization key, so the page's
  // SettingsCategoryController shows it as it is); how many it queued
  private final func QueueResults(label: String, found: array<ref<ConfigVar>>, room: Int32) -> Int32 {
    let n = Min(room, ArraySize(found));
    if n <= 0 {
      return 0;
    };
    this.QueueHeader(StringToName(label));
    let i = 0;
    while i < n {
      ArrayPush(this.m_pendingHeaders, n"None");
      ArrayPush(this.m_pendingOptions, found[i]);
      i += 1;
    };
    return n;
  }

}