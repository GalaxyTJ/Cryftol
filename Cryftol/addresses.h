#pragma once
#include <lua.h>


	const auto task_defer = reinterpret_cast<int(__cdecl*)(lua_State*)>(base(0x7DFF20));
	const auto taskscheduler = reinterpret_cast<int(__cdecl*)()>(base(0xB38CC0));


	lua_State* state{};

	lua_State* get_global_state(int script_context)
	{
		return reinterpret_cast<lua_State*>(*reinterpret_cast<uintptr_t*>(script_context + 244) - (script_context + 244));
	}
