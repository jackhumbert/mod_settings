#pragma once
#include <filesystem>

namespace Utils
{
//void CreateLogger();
std::filesystem::path GetRootDir();
std::wstring ToWString(const char* aText);
}

#include "RED4ext/CName.hpp"
#include <RED4ext/RED4ext.hpp>

template<typename T>
static void RedTypeFromString(T * pointer, const RED4ext::CString& str) {

}

template<> void RedTypeFromString<bool>(bool * pointer, const RED4ext::CString& str) {
  RED4ext::RawBuffer buffer;
  buffer.data = (void*)str.c_str();
  buffer.size = str.length;
  RTTITYPE(3443526637)->FromString(pointer, buffer);
}

template<> void RedTypeFromString<int8_t>(int8_t * pointer, const RED4ext::CString& str) {
  RED4ext::RawBuffer buffer;
  buffer.data = (void*)str.c_str();
  buffer.size = str.length;
  RTTITYPE(3415542724)->FromString(pointer, buffer);
}

template<> void RedTypeFromString<int16_t>(int16_t * pointer, const RED4ext::CString& str) {
  RED4ext::RawBuffer buffer;
  buffer.data = (void*)str.c_str();
  buffer.size = str.length;
  RTTITYPE(3592162291)->FromString(pointer, buffer);
}

template<> void RedTypeFromString<int32_t>(int32_t * pointer, const RED4ext::CString& str) {
  RED4ext::RawBuffer buffer;
  buffer.data = (void*)str.c_str();
  buffer.size = str.length;
  RTTITYPE(3590851569)->FromString(pointer, buffer);
}

template<> void RedTypeFromString<int64_t>(int64_t * pointer, const RED4ext::CString& str) {
  RED4ext::RawBuffer buffer;
  buffer.data = (void*)str.c_str();
  buffer.size = str.length;
  RTTITYPE(3594652662)->FromString(pointer, buffer);
}

template<> void RedTypeFromString<uint8_t>(uint8_t * pointer, const RED4ext::CString& str) {
  RED4ext::RawBuffer buffer;
  buffer.data = (void*)str.c_str();
  buffer.size = str.length;
  RTTITYPE(3648392249)->FromString(pointer, buffer);
}

template<> void RedTypeFromString<uint16_t>(uint16_t * pointer, const RED4ext::CString& str) {
  RED4ext::RawBuffer buffer;
  buffer.data = (void*)str.c_str();
  buffer.size = str.length;
  RTTITYPE(3832679528)->FromString(pointer, buffer);
}

template<> void RedTypeFromString<uint32_t>(uint32_t * pointer, const RED4ext::CString& str) {
  RED4ext::RawBuffer buffer;
  buffer.data = (void*)str.c_str();
  buffer.size = str.length;
  RTTITYPE(3831368806)->FromString(pointer, buffer);
}

template<> void RedTypeFromString<uint64_t>(uint64_t * pointer, const RED4ext::CString& str) {
  RED4ext::RawBuffer buffer;
  buffer.data = (void*)str.c_str();
  buffer.size = str.length;
  RTTITYPE(3835169899)->FromString(pointer, buffer);
}

template<> void RedTypeFromString<float>(float * pointer, const RED4ext::CString& str) {
  RED4ext::RawBuffer buffer;
  buffer.data = (void*)str.c_str();
  buffer.size = str.length;
  RTTITYPE(3665562711)->FromString(pointer, buffer);
}

template<> void RedTypeFromString<double>(double * pointer, const RED4ext::CString& str) {
  RED4ext::RawBuffer buffer;
  buffer.data = (void*)str.c_str();
  buffer.size = str.length;
  RTTITYPE(3892120764)->FromString(pointer, buffer);
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
