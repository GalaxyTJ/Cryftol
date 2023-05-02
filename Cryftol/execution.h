#pragma once
#include "scheduler.h"
#include <Luau/Compiler.h>


	inline auto execute(std::string_view script)
	{
		auto states = lua_newthread(state);
		lua_setsafeenv(states, LUA_ENVIRONINDEX, false);

		auto bytecode = Luau::compile(script.data());

		if (bytecode.at(0) == 0)
		{
			lua_getglobal(states, "warn");
			lua_pushstring(states, bytecode.data() + 1);
			lua_pcall(states, 1, 0, 0);
		}
		else
		{
			luau_load(state, "@Cryftol", bytecode.data(), bytecode.length(), 0);
			task_defer(states);
		}
	}