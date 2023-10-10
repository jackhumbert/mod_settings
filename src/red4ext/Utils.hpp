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
  RTTITYPE(Bool)->FromString(pointer, buffer);
}

template<> void RedTypeFromString<int8_t>(int8_t * pointer, const RED4ext::CString& str) {
  RED4ext::RawBuffer buffer;
  buffer.data = (void*)str.c_str();
  buffer.size = str.length;
  RTTITYPE(Int8)->FromString(pointer, buffer);
}

template<> void RedTypeFromString<int16_t>(int16_t * pointer, const RED4ext::CString& str) {
  RED4ext::RawBuffer buffer;
  buffer.data = (void*)str.c_str();
  buffer.size = str.length;
  RTTITYPE(Int16)->FromString(pointer, buffer);
}

template<> void RedTypeFromString<int32_t>(int32_t * pointer, const RED4ext::CString& str) {
  RED4ext::RawBuffer buffer;
  buffer.data = (void*)str.c_str();
  buffer.size = str.length;
  RTTITYPE(Int32)->FromString(pointer, buffer);
}

template<> void RedTypeFromString<int64_t>(int64_t * pointer, const RED4ext::CString& str) {
  RED4ext::RawBuffer buffer;
  buffer.data = (void*)str.c_str();
  buffer.size = str.length;
  RTTITYPE(Int64)->FromString(pointer, buffer);
}

template<> void RedTypeFromString<uint8_t>(uint8_t * pointer, const RED4ext::CString& str) {
  RED4ext::RawBuffer buffer;
  buffer.data = (void*)str.c_str();
  buffer.size = str.length;
  RTTITYPE(Uint8)->FromString(pointer, buffer);
}

template<> void RedTypeFromString<uint16_t>(uint16_t * pointer, const RED4ext::CString& str) {
  RED4ext::RawBuffer buffer;
  buffer.data = (void*)str.c_str();
  buffer.size = str.length;
  RTTITYPE(Uint16)->FromString(pointer, buffer);
}

template<> void RedTypeFromString<uint32_t>(uint32_t * pointer, const RED4ext::CString& str) {
  RED4ext::RawBuffer buffer;
  buffer.data = (void*)str.c_str();
  buffer.size = str.length;
  RTTITYPE(Uint32)->FromString(pointer, buffer);
}

template<> void RedTypeFromString<uint64_t>(uint64_t * pointer, const RED4ext::CString& str) {
  RED4ext::RawBuffer buffer;
  buffer.data = (void*)str.c_str();
  buffer.size = str.length;
  RTTITYPE(Uint64)->FromString(pointer, buffer);
}

template<> void RedTypeFromString<float>(float * pointer, const RED4ext::CString& str) {
  RED4ext::RawBuffer buffer;
  buffer.data = (void*)str.c_str();
  buffer.size = str.length;
  RTTITYPE(Float)->FromString(pointer, buffer);
}

template<> void RedTypeFromString<double>(double * pointer, const RED4ext::CString& str) {
  RED4ext::RawBuffer buffer;
  buffer.data = (void*)str.c_str();
  buffer.size = str.length;
  RTTITYPE(Double)->FromString(pointer, buffer);
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
