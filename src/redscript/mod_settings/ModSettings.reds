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