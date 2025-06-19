#pragma once

#include <RED4ext/Common.hpp>
#include <RED4ext/RED4ext.hpp>
#include <RED4ext/Scripting/Natives/Generated/ink/ActionName.hpp>
#include <RedLib.hpp>

struct ActionNameEx : Red::ink::ActionName
{
    Red::CName ToName()
    {
        return this->name;
    }
};

RTTI_EXPAND_CLASS(Red::ink::ActionName, ActionNameEx, {
    RTTI_METHOD(ToName);
});