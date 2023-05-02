// This file is part of the Luau programming language and is licensed under MIT License; see LICENSE.txt for details
// This code is based on Lua 5.x implementation licensed under MIT License; see lua_LICENSE.txt for details
#pragma once
#include <windows.h>

// When debugging complex issues, consider enabling one of these:
// This will reallocate the stack very aggressively at every opportunity; use this with asan to catch stale stack pointers
// #define HARDSTACKTESTS 1
// This will call GC validation very aggressively at every incremental GC step; use this with caution as it's SLOW
// #define HARDMEMTESTS 1
// This will call GC validation very aggressively at every GC opportunity; use this with caution as it's VERY SLOW
// #define HARDMEMTESTS 2

// To force MSVC2017+ to generate SSE2 code for some stdlib functions we need to locally enable /fp:fast
// Note that /fp:fast changes the semantics of floating point comparisons so this is only safe to do for functions without ones
#if defined(_MSC_VER) && !defined(__clang__)
#define LUAU_FASTMATH_BEGIN __pragma(float_control(precise, off, push))
#define LUAU_FASTMATH_END __pragma(float_control(pop))
#else
#define LUAU_FASTMATH_BEGIN
#define LUAU_FASTMATH_END
#endif

// Some functions like floor/ceil have SSE4.1 equivalents but we currently support systems without SSE4.1
// Note that we only need to do this when SSE4.1 support is not guaranteed by compiler settings, as otherwise compiler will optimize these for us.
#if (defined(__x86_64__) || defined(_M_X64)) && !defined(__SSE4_1__) && !defined(__AVX__)
#if defined(_MSC_VER) && !defined(__clang__)
#define LUAU_TARGET_SSE41
#elif defined(__GNUC__) && defined(__has_attribute)
#if __has_attribute(target)
#define LUAU_TARGET_SSE41 __attribute__((target("sse4.1")))
#endif
#endif
#endif

// Used on functions that have a printf-like interface to validate them statically
#if defined(__GNUC__)
#define LUA_PRINTF_ATTR(fmt, arg) __attribute__((format(printf, fmt, arg)))
#else
#define LUA_PRINTF_ATTR(fmt, arg)
#endif

#ifdef _MSC_VER
#define LUA_NORETURN __declspec(noreturn)
#else
#define LUA_NORETURN __attribute__((__noreturn__))
#endif

// Can be used to reconfigure visibility/exports for public APIs
#ifndef LUA_API
#define LUA_API extern
#endif

#define LUALIB_API LUA_API

// Can be used to reconfigure visibility for internal APIs
#if defined(__GNUC__)
#define LUAI_FUNC __attribute__((visibility("hidden"))) extern
#define LUAI_DATA LUAI_FUNC
#else
#define LUAI_FUNC extern
#define LUAI_DATA extern
#endif

// Can be used to reconfigure internal error handling to use longjmp instead of C++ EH
#ifndef LUA_USE_LONGJMP
#define LUA_USE_LONGJMP 0
#endif

// LUA_IDSIZE gives the maximum size for the description of the source
#ifndef LUA_IDSIZE
#define LUA_IDSIZE 256
#endif

// LUA_MINSTACK is the guaranteed number of Lua stack slots available to a C function
#ifndef LUA_MINSTACK
#define LUA_MINSTACK 20
#endif

// LUAI_MAXCSTACK limits the number of Lua stack slots that a C function can use
#ifndef LUAI_MAXCSTACK
#define LUAI_MAXCSTACK 8000
#endif

// LUAI_MAXCALLS limits the number of nested calls
#ifndef LUAI_MAXCALLS
#define LUAI_MAXCALLS 20000
#endif

// LUAI_MAXCCALLS is the maximum depth for nested C calls; this limit depends on native stack size
#ifndef LUAI_MAXCCALLS
#define LUAI_MAXCCALLS 200
#endif

// buffer size used for on-stack string operations; this limit depends on native stack size
#ifndef LUA_BUFFERSIZE
#define LUA_BUFFERSIZE 512
#endif

// number of valid Lua userdata tags
#ifndef LUA_UTAG_LIMIT
#define LUA_UTAG_LIMIT 128
#endif

// upper bound for number of size classes used by page allocator
#ifndef LUA_SIZECLASSES
#define LUA_SIZECLASSES 32
#endif

// available number of separate memory categories
#ifndef LUA_MEMORY_CATEGORIES
#define LUA_MEMORY_CATEGORIES 256
#endif

// minimum size for the string table (must be power of 2)
#ifndef LUA_MINSTRTABSIZE
#define LUA_MINSTRTABSIZE 32
#endif

// maximum number of captures supported by pattern matching
#ifndef LUA_MAXCAPTURES
#define LUA_MAXCAPTURES 32
#endif

// enables callbacks to redirect code execution from Luau VM to a custom implementation
#ifndef LUA_CUSTOM_EXECUTION
#define LUA_CUSTOM_EXECUTION 0
#endif

// }==================================================================

/*
@@ LUAI_USER_ALIGNMENT_T is a type that requires maximum alignment.
** CHANGE it if your system requires alignments larger than double. (For
** instance, if your system supports long doubles and they must be
** aligned in 16-byte boundaries, then you should add long double in the
** union.) Probably you do not need to change this.
*/
#define LUAI_USER_ALIGNMENT_T \
    union \
    { \
        double u; \
        void* s; \
        long l; \
    }

#define LUA_VECTOR_SIZE 3 // must be 3 or 4

#define LUA_EXTRA_SIZE (LUA_VECTOR_SIZE - 2)

#define vm_shuffle_comma ,
#define vm_shuffle_semicolon ;

static auto base(uintptr_t address)
{
    return address - 0x400000 + reinterpret_cast<uintptr_t>(GetModuleHandleA(0));
}

const auto luao_nilobject_addy = base(0x29B1040);
const auto luah_dummynode_addy = base(0x29B1020);

#define vm_shuffle3(d, a1, a2, a3) a3 d a2 d a1
#define vm_shuffle4(d, a1, a2, a3, a4) a2 d a4 d a3 d a1
#define vm_shuffle5(d, a1, a2, a3, a4, a5) a3 d a5 d a4 d a2 d a1
#define vm_shuffle6(d, a1, a2, a3, a4, a5, a6) a1 d a3 d a2 d a6 d a5 d a4
#define vm_shuffle7(d, a1, a2, a3, a4, a5, a6, a7) a1 d a2 d a7 d a6 d a4 d a5 d a3
#define vm_shuffle8(d, a1, a2, a3, a4, a5, a6, a7, a8) a6 d a4 d a7 d a2 d a8 d a1 d a5 d a3
#define vm_shuffle9(d, a1, a2, a3, a4, a5, a6, a7, a8, a9) a4 d a5 d a9 d a8 d a7 d a6 d a1 d a3 d a2

template<typename t>
struct vm_value1
{
public:
    operator const t() const
    {
        return (t)((uintptr_t)storage - (uintptr_t)this);
    }

    void operator=(const t& value)
    {
        storage = (t)((uintptr_t)value + (uintptr_t)this);
    }

    const t operator->() const
    {
        return operator const t();
    }

private:
    t storage;
};

template<typename t>
struct vm_value2
{
public:
    operator const t() const
    {
        return (t)((uintptr_t)this - (uintptr_t)storage);
    }

    void operator=(const t& value)
    {
        storage = (t)((uintptr_t)this - (uintptr_t)value);
    }

    const t operator->() const
    {
        return operator const t();
    }

private:
    t storage;
};


template<typename t>
struct vm_value3
{
public:
    operator const t() const
    {
        return (t)((uintptr_t)this ^ (uintptr_t)storage);
    }

    void operator=(const t& value)
    {
        storage = (t)((uintptr_t)value ^ (uintptr_t)this);
    }

    const t operator->() const
    {
        return operator const t();
    }

private:
    t storage;
};

template<typename t>
struct vm_value4
{
public:
    operator const t() const
    {
        return (t)((uintptr_t)this + (uintptr_t)storage);
    }

    void operator=(const t& value)
    {
        storage = (t)((uintptr_t)value - (uintptr_t)this);
    }

    const t operator->() const
    {
        return operator const t();
    }

private:
    t storage;
};