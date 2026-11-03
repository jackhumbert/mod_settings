#pragma once

#include <RED4ext/Common.hpp>
#include <RED4ext/RED4ext.hpp>
#include <RED4ext/Scripting/Natives/Generated/ink/ActionName.hpp>
#include <RedLib.hpp>

struct ActionNameEx : Red::ink::ActionName
{
    Red::CName ToName()
    {
#ifdef __APPLE__
        // The Mac SDK's ink::ActionName is opaque (uint8_t unk40[]). The reflection dump of the Mac game (layout/reflection-2026-10-08.tsv)
        // has the CName property "action" at offset 64 (0x40): the field the fork calls `name`.
        return *reinterpret_cast<const Red::CName*>(reinterpret_cast<const uint8_t*>(this) + 0x40);
#else
        return this->name;
#endif
    }
};

RTTI_EXPAND_CLASS(Red::ink::ActionName, ActionNameEx, {
    RTTI_METHOD(ToName);
});