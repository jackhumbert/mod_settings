#pragma once

#include "RED4ext/RTTITypes.hpp"
#include <RED4ext/Common.hpp>
#include <RED4ext/Scripting/Natives/userRuntimeSettingsVar.hpp>
#include <RED4ext/Scripting/Natives/userSettingsVar.hpp>

using RED4ext::CName;
using RED4ext::WeakHandle;
using RED4ext::Handle;
struct IModRuntimeSettingsVar;
struct ScriptProperty;

// enum class ModSettingType {
//   Unknown = 0,
//   Toggle,
//   IntRange,
//   FloatRange,
//   Enum,
//   Name
// };

// struct CName2 : CName {
//   constexpr CName2(const char * aName) noexcept : CName(aName) {

//   }
//   // operator ModSettingType() const {
//   //   switch ((CName)*this) {
//   //     case CName("Bool"):
//   //       return ModSettingType::Toggle;
//   //     case CName("Int32"):
//   //       return ModSettingType::IntRange;
//   //     case CName("Float"):
//   //       return ModSettingType::FloatRange;
//   //     case CName("Enum"):
//   //       return ModSettingType::Enum;
//   //     default: 
//   //       return ModSettingType::Unknown;
//   //   }
//   // }

  
//     constexpr operator ModSettingType() const noexcept {
//       switch ((CName)*this) {
//         case CName("Bool"):
//           return ModSettingType::Toggle;
//         case CName("Int32"):
//           return ModSettingType::IntRange;
//         case CName("Float"):
//           return ModSettingType::FloatRange;
//         case CName("Enum"):
//           return ModSettingType::Enum;
//         default: 
//           return ModSettingType::Unknown;
//       }
//     }
// };

// template <ModSettingType Type>
// struct ModSettingsVariable2 {

// };

// template<CName2 TypeName>
// struct ModSettingsVariable2 : ModSettingsVariable2<TypeName> {

// };

// inline void test() {
//   auto c = new ModSettingsVariable2<"Bool">();
// } 

// template<>
// struct ModSettingsVariable2<CName("Bool")> : ModSettingsVariable2<ModSettingType::Toggle> {
  
// };

struct ModSettingDependency {
  RED4ext::CName className;
  RED4ext::CName propertyName;
  RED4ext::CString value;
};

class ModSettingsVariable {
public:
  explicit ModSettingsVariable();
  explicit ModSettingsVariable(ScriptProperty* prop, const CName scriptClass);
  // ModSettingsVariable(const CName aMod, const CName typeName, const CName className, const CName category);

  // template<typename VariableType, class ClassName = RED4ext::IScriptable>
  // inline ModSettingsVariable(const CName mod, const WeakHandle<ClassName> listener, const CName category, VariableType defaultValue) 
  //   : ModSettingsVariable(mod, GetTypeCName<VariableType>(), listener->GetNative()->GetName(), category) {
  //   static_assert(std::is_convertible<ClassName*, RED4ext::IScriptable*>::value, "listener must inherit IScriptable");  
  //   this->listeners.EmplaceBack(listener);
  // }

  // template<typename VariableType>
  // inline ModSettingsVariable(const CName mod, const CName className, const CName category, VariableType defaultValue) :
  //   ModSettingsVariable(mod, GetTypeCName<VariableType>(), className, category) {
  // }
  
  void UpdateValues();
  Handle<RED4ext::user::SettingsVar> CreateConfigVar();
  void Write(std::ofstream& stream);

  inline const RED4ext::CName& Mod() const { return mod; }
  inline const RED4ext::CName& ClassName() const { return className; }
  inline const RED4ext::CName& Category() const { return category; }

  bool RestoreDefault();
  void RejectChange();

  void RegisterListener(Handle<RED4ext::IScriptable> handle);
  void UnregisterListener(Handle<RED4ext::IScriptable> handle);

  inline const bool IsValid() const { return this->settingsVar != nullptr; }

// private:
  template<typename VariableType>
  constexpr const CName GetTypeCName() noexcept;

  bool SetRuntimeSetting(ScriptProperty * prop);

  CName mod = CName();
  CName className = CName();
  CName category = CName();

  RED4ext::CBaseRTTIType *type;
  RED4ext::CClass *configVarType;
  IModRuntimeSettingsVar *settingsVar = nullptr;
  RED4ext::DynArray<RED4ext::WeakHandle<RED4ext::IScriptable>> listeners;
  // std::mutex listeners_lock;
  ModSettingDependency dependency;
};