#include "ModConfigVar.hpp"

//RED4ext::TTypedClass<ModConfigVarBool> modConfigVarBool(RED4ext::CNamePool::Add("modSettingsVarBool"), {.isNative = true});
//
//RED4ext::CClass *ModConfigVarBool::GetNativeType() { return &modConfigVarBool; }

//RED4ext::TTypedClass<ModConfigVarInt> modConfigVarInt(RED4ext::CNamePool::Add("modSettingsVarInt"), {.isNative = true});
//
//RED4ext::CClass *ModConfigVarInt::GetNativeType() { return &modConfigVarInt; }
//
//RED4ext::TTypedClass<ModConfigVarFloat> modConfigVarFloat(RED4ext::CNamePool::Add("modSettingsVarFloat"),
//                                                          {.isNative = true});
//
//RED4ext::CClass *ModConfigVarFloat::GetNativeType() { return &modConfigVarFloat; }
//
//RED4ext::TTypedClass<ModConfigVarListInt> modConfigVarListInt(RED4ext::CNamePool::Add("modSettingsVarListInt"),
//                                                              {.isNative = true});
//
//RED4ext::CClass *ModConfigVarListInt::GetNativeType() { return &modConfigVarListInt; }
//
//void SetModConfigVar(RED4ext::IScriptable *aContext, RED4ext::CStackFrame *aFrame, void *aOut, int64_t a4) { 
//	uint64_t value;
//	RED4ext::GetParameter(aFrame, &value);
//	aFrame->code++;
//
//	auto configVar = reinterpret_cast<RED4ext::user::SettingsVar *>(aContext);
//	configVar->runtimeVar->UpdateValue(&value);
//}

//#define XSTR(s) STR(s)
//#define STR(s) #s

//#define SCRIPT_FUNCTION_WITH_FLAGS(cls, name, function, flags)                                                         \
//  auto cls##_##name =                                                                                                  \
//      +[](RED4ext::IScriptable * aContext, RED4ext::CStackFrame * aFrame, void *aOut, int64_t aType) function;         \
//  cls.RegisterFunction(RED4ext::CClassFunction::Create<void *>(&cls, STR(name), STR(name), cls##_##name))

//#define SCRIPT_FUNCTION_WITH_FLAGS(cls, name, function, flags)                                                         \
//  cls.RegisterFunction(RED4ext::CClassFunction::Create<void *>(                                                        \
//      &cls, name, name,                                                                                      \
//      +[](RED4ext::IScriptable *aContext, RED4ext::CStackFrame *aFrame, void *aOut, int64_t aType) function, flags))
//
//#define SCRIPT_FUNCTION_FOR_CLASS(cls, name, function) SCRIPT_FUNCTION_WITH_FLAGS(cls, name, function, {})
//
//#define SCRIPT_FUNCTION_NAME_WITH_FLAGS(clsName, funcName, function, flags)                                                        \
//  { auto cls = RED4ext::CRTTISystem::Get()->GetClass(clsName);                                                                  \
//  cls->RegisterFunction(RED4ext::CClassFunction::Create<void *>(                                                        \
//      cls, funcName, funcName,                                                                                                \
//      +[](RED4ext::IScriptable * aContext, RED4ext::CStackFrame * aFrame, void *aOut, int64_t aType) function)); }

//template <typename T>
//using PostRegisterTypes = RED4ext::TTypedClass<T>::PostRegisterTypes;

//using ScriptingFunction_t = void (*)(IScriptable*, CStackFrame*, T, int64_t);

//void RED4ext::TTypedClass<ModConfigVarBool>::RegisterScriptFunctions() {
//  SCRIPT_FUNCTION_FOR_CLASS(modConfigVarBool, "SetValue", { auto a = aContext->GetType(); });
//
//  SCRIPT_FUNCTION_NAME_WITH_FLAGS("modSettingsVarFloat", "SetValue", {
//
//  }, {});
//}

//#define SCRIPT_FUNCTION_RETURN_TYPE(name, flags, type, function)                                                       \
//  this->RegisterFunction(RED4ext::CClassFunction::Create<type>(                                                        \
//      this, name, name,                                                                                                \
//      +[](RED4ext::IScriptable * aContext, RED4ext::CStackFrame * aFrame, type aOut, int64_t aType) function, flags))
//
//#define SCRIPT_FUNCTION(name, flags, function) SCRIPT_FUNCTION_RETURN_TYPE(name, flags, void *, function)
//
//void RED4ext::TTypedClass<ModConfigVarBool>::RegisterScriptFunctions() {
//  SCRIPT_FUNCTION_RETURN_TYPE("SetValue", {.isNative = true}, void *, {
//    bool value;
//    RED4ext::GetParameter(aFrame, &value);
//    aFrame->code++;
//    auto instance = reinterpret_cast<ModConfigVarBool *>(aContext);
//    auto runtime = reinterpret_cast<RuntimeVariableBool *>(instance->runtimeVar);
//    if (runtime) {
//      runtime->UpdateValue(&value);
//    }
//  });
//  SCRIPT_FUNCTION_RETURN_TYPE("GetValue", {.isNative = true}, bool *, {
//    aFrame->code++;
//    auto instance = reinterpret_cast<ModConfigVarBool *>(aContext);
//    auto runtime = reinterpret_cast<RuntimeVariableBool *>(instance->runtimeVar);
//    if (runtime && aOut) {
//      *aOut = runtime->valueInput;
//    }
//  });
//  SCRIPT_FUNCTION_RETURN_TYPE("GetDefaultValue", {.isNative = true}, bool *, {
//    aFrame->code++;
//    auto instance = reinterpret_cast<ModConfigVarBool *>(aContext);
//    auto runtime = reinterpret_cast<RuntimeVariableBool *>(instance->runtimeVar);
//    if (runtime && aOut) {
//      *aOut = runtime->defaultValue;
//    }
//  });
//}