#pragma once
#include <filesystem>

namespace Utils
{
//void CreateLogger();
std::filesystem::path GetRootDir();
#ifdef __APPLE__
std::filesystem::path GetPluginDir();
#endif
std::wstring ToWString(const char* aText);
}

#include "RED4ext/CName.hpp"
#include <RED4ext/RED4ext.hpp>

namespace Utils {
// ITypeInfo::FromString. The Windows (fork) SDK takes the text as a RawBuffer; the Mac SDK takes the CString itself, so call sites
// go through here. The Windows branch is the original code, unchanged.
inline bool FromString(RED4ext::CBaseRTTIType * type, RED4ext::ScriptInstance instance, const RED4ext::CString& str) {
#ifdef __APPLE__
  return type->FromString(instance, str);
#else
  RED4ext::RawBuffer buffer;
  buffer.data = (void*)str.c_str();
  buffer.size = str.length;
  return type->FromString(instance, buffer);
#endif
}
}

#ifdef __APPLE__
// The Windows build gets the fundamental types from fixed SDK address hashes (RTTITYPE(hash)). Those hashes have no Mac address, so the
// Mac build asks the RTTI system for the type by its script name; the hash argument is kept so both spellings read alike.
#define MS_FUNDAMENTAL_TYPE(hash, name) (RED4ext::CRTTISystem::Get()->GetType(RED4ext::CName(name)))
#else
#define MS_FUNDAMENTAL_TYPE(hash, name) RTTITYPE(hash)
#endif

template<typename T>
static void RedTypeFromString(T * pointer, const RED4ext::CString& str) {

}

template<> void RedTypeFromString<bool>(bool * pointer, const RED4ext::CString& str) {
  Utils::FromString(MS_FUNDAMENTAL_TYPE(3443526637, "Bool"), pointer, str);
}

template<> void RedTypeFromString<int8_t>(int8_t * pointer, const RED4ext::CString& str) {
  Utils::FromString(MS_FUNDAMENTAL_TYPE(3415542724, "Int8"), pointer, str);
}

template<> void RedTypeFromString<int16_t>(int16_t * pointer, const RED4ext::CString& str) {
  Utils::FromString(MS_FUNDAMENTAL_TYPE(3592162291, "Int16"), pointer, str);
}

template<> void RedTypeFromString<int32_t>(int32_t * pointer, const RED4ext::CString& str) {
  Utils::FromString(MS_FUNDAMENTAL_TYPE(3590851569, "Int32"), pointer, str);
}

template<> void RedTypeFromString<int64_t>(int64_t * pointer, const RED4ext::CString& str) {
  Utils::FromString(MS_FUNDAMENTAL_TYPE(3594652662, "Int64"), pointer, str);
}

template<> void RedTypeFromString<uint8_t>(uint8_t * pointer, const RED4ext::CString& str) {
  Utils::FromString(MS_FUNDAMENTAL_TYPE(3648392249, "Uint8"), pointer, str);
}

template<> void RedTypeFromString<uint16_t>(uint16_t * pointer, const RED4ext::CString& str) {
  Utils::FromString(MS_FUNDAMENTAL_TYPE(3832679528, "Uint16"), pointer, str);
}

template<> void RedTypeFromString<uint32_t>(uint32_t * pointer, const RED4ext::CString& str) {
  Utils::FromString(MS_FUNDAMENTAL_TYPE(3831368806, "Uint32"), pointer, str);
}

template<> void RedTypeFromString<uint64_t>(uint64_t * pointer, const RED4ext::CString& str) {
  Utils::FromString(MS_FUNDAMENTAL_TYPE(3835169899, "Uint64"), pointer, str);
}

template<> void RedTypeFromString<float>(float * pointer, const RED4ext::CString& str) {
  Utils::FromString(MS_FUNDAMENTAL_TYPE(3665562711, "Float"), pointer, str);
}

template<> void RedTypeFromString<double>(double * pointer, const RED4ext::CString& str) {
  Utils::FromString(MS_FUNDAMENTAL_TYPE(3892120764, "Double"), pointer, str);
}

template<> void RedTypeFromString<RED4ext::CName>(RED4ext::CName * pointer, const RED4ext::CString& str) {
  *pointer = RED4ext::CNamePool::Add(str.c_str());
}

inline RED4ext::CBaseRTTIType* ToType(const RED4ext::CName name) {
  return RED4ext::CRTTISystem::Get()->GetType(name);
}

inline RED4ext::CClass* ToClass(const RED4ext::CName name) {
  return RED4ext::CRTTISystem::Get()->GetClass(name);
}
