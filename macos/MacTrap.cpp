// macOS only. An address hash with no Mac address must never resolve to 0: a call through it would jump to null. Pattern:
// cp2077-codeware/macos/MacTrap.cpp. Codeware generates one stub per known hash from a table; this plugin has no table (it uses
// only a few dozen SDK hashes), so a stub is handed out the first time an unmapped hash is resolved.
//
//   * The Mac SDK's Resolve calls modsettings_mac_trap_for_hash(hash) for every hash that is in neither the SDK's Mac table nor
//     ModSettingsSdkRows.inc (macos/CMakeLists.txt rewrites the tail of Resolve).
//   * The returned address is a stub that, on its first call, logs
//       [mod_settings] not on Mac yet: hash <n> called from <image>+0x<off>
//     and returns zero in x0 and x1. A hash the SDK reads as data (a RelocPtr) gets a zeroed buffer instead of code, so a read
//     through it yields null instead of executing.
//   * ModSettingsMac::AttachHook refuses a trap address as a hook target.
//
// Limits (also in NOT_YET.md): a function that returns a struct through x8 (CString, Handle, DynArray) is not filled in by the
// generic stub; a function that returns a float gets whatever is in s0.
#include "MacShims.hpp"

#include <array>
#include <atomic>
#include <cstdio>
#include <cstring>
#include <mutex>
#include <utility>

#include <dlfcn.h>

namespace
{
struct TrapRet
{
    uint64_t x0;
    uint64_t x1;
};

constexpr size_t kStubs = 96;

std::array<std::atomic<uint32_t>, kStubs> g_stubHash{};   // hash served by stub I (0 = free)
std::array<std::atomic<bool>, kStubs> g_stubFired{};

void Describe(const void* aAddress, char* aOut, size_t aSize)
{
    Dl_info info{};
    if (aAddress && dladdr(aAddress, &info) && info.dli_fbase)
    {
        const char* file = info.dli_fname ? std::strrchr(info.dli_fname, '/') : nullptr;
        file = file ? file + 1 : (info.dli_fname ? info.dli_fname : "?");
        std::snprintf(aOut, aSize, "%s+0x%llx", file,
                      static_cast<unsigned long long>(reinterpret_cast<uintptr_t>(aAddress) -
                                                      reinterpret_cast<uintptr_t>(info.dli_fbase)));
        return;
    }
    std::snprintf(aOut, aSize, "?%p", aAddress);
}

void TrapHit(size_t aIndex, const void* aCaller)
{
    if (g_stubFired[aIndex].exchange(true))
    {
        return;
    }
    char where[160];
    Describe(aCaller, where, sizeof(where));
    ModSettingsMac::LogWarn("not on Mac yet: hash %u called from %s (feature skipped, returns 0)", g_stubHash[aIndex].load(), where);
}

// One instantiation per slot. The arguments of the call are ignored; x0 and x1 come back as zero.
template<size_t I>
__attribute__((noinline)) TrapRet TrapStub()
{
    TrapHit(I, __builtin_return_address(0));
    return TrapRet{0, 0};
}

template<size_t... I>
constexpr std::array<void*, sizeof...(I)> MakeStubs(std::index_sequence<I...>)
{
    return {reinterpret_cast<void*>(&TrapStub<I>)...};
}

const std::array<void*, kStubs> kStubFns = MakeStubs(std::make_index_sequence<kStubs>{});

// The SDK hashes that are data cells, not functions (from Codeware's trap table, MacTrapTable.inc TRAP_DATA rows). They read as zeros.
constexpr uint32_t kDataHashes[] = {458427884u,  677908004u,  961751128u,  1239944840u, 1508445968u, 1518151849u, 1704595399u,
                                    2017202228u, 2549221846u, 2630817091u, 2635337807u, 3151104823u, 3720157672u};
constexpr size_t kDataSlots = sizeof(kDataHashes) / sizeof(kDataHashes[0]);
alignas(16) uint8_t g_zeroData[kDataSlots][512];

std::mutex g_lock;
unsigned g_used = 0;

// A stub for hashes beyond kStubs: one shared stub that cannot name the hash.
__attribute__((noinline)) TrapRet OverflowStub()
{
    char where[160];
    Describe(__builtin_return_address(0), where, sizeof(where));
    ModSettingsMac::LogOnce("trap-overflow", "not on Mac yet: an address hash beyond the %zu trap stubs, called from %s", kStubs, where);
    return TrapRet{0, 0};
}
} // namespace

extern "C" uintptr_t modsettings_mac_trap_for_hash(uint32_t aHash)
{
    for (size_t i = 0; i < kDataSlots; ++i)
    {
        if (kDataHashes[i] == aHash)
        {
            ModSettingsMac::LogOnce((std::string("trap-data-") + std::to_string(aHash)).c_str(),
                                    "not on Mac yet: data hash %u resolved to a zeroed buffer (reads as zeros)", aHash);
            return reinterpret_cast<uintptr_t>(g_zeroData[i]);
        }
    }

    std::scoped_lock lock(g_lock);
    for (unsigned i = 0; i < g_used; ++i)
    {
        if (g_stubHash[i].load() == aHash)
        {
            return reinterpret_cast<uintptr_t>(kStubFns[i]);
        }
    }
    if (g_used >= kStubs)
    {
        return reinterpret_cast<uintptr_t>(&OverflowStub);
    }
    const unsigned slot = g_used++;
    g_stubHash[slot].store(aHash);
    return reinterpret_cast<uintptr_t>(kStubFns[slot]);
}

bool ModSettingsMac::IsTrapAddress(uintptr_t aAddress)
{
    if (aAddress == reinterpret_cast<uintptr_t>(&OverflowStub))
    {
        return true;
    }
    for (void* fn : kStubFns)
    {
        if (reinterpret_cast<uintptr_t>(fn) == aAddress)
        {
            return true;
        }
    }
    for (size_t i = 0; i < kDataSlots; ++i)
    {
        const auto base = reinterpret_cast<uintptr_t>(g_zeroData[i]);
        if (aAddress >= base && aAddress < base + sizeof(g_zeroData[i]))
        {
            return true;
        }
    }
    return false;
}
