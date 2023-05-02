#pragma once
#include <lgc.h>
#include <lapi.h>
#include <lualib.h>
#include "execution.h"
#include <curl/curl.h>

namespace functions
{
	__forceinline int getreg(lua_State* states)
	{
		lua_pushvalue(states, LUA_REGISTRYINDEX);
		return 1;
	}

	__forceinline int getgenv(lua_State* states)
	{
		lua_pushvalue(states, LUA_GLOBALSINDEX);

		return 1;
	}

	__forceinline int getrenv(lua_State* states)
	{
		lua_pushvalue(state, LUA_GLOBALSINDEX);
		lua_xmove(state, states, 1);

		return 1;
	}

	__forceinline int isreadonly(lua_State* states)
	{
		luaL_checktypes(states, 1, LUA_TTABLE, LUA_TUSERDATA);
		lua_pushboolean(states, lua_getreadonly(states, 1));

		return 1;
	}

	inline int setreadonly(lua_State* states)
	{
		luaL_checktypes(states, 1, LUA_TTABLE, LUA_TUSERDATA);
		luaL_checktype(states, 2, LUA_TBOOLEAN);
		lua_setreadonly(state, 1, lua_toboolean(state, 2));

		return 0;
	}

	inline int loadstring(lua_State* states)
	{
		luaL_checktype(states, 1, LUA_TSTRING);

		size_t len{};
		const char* script = lua_tolstring(states, 1, &len);

		lua_setsafeenv(states, LUA_ENVIRONINDEX, false);

		auto bytecode = Luau::compile(std::string(script, len));

		if (luau_load(state, luaL_optstring(states, 2, "@"), bytecode.data(), bytecode.length(), 0))
		{
			lua_pushnil(states);
			lua_insert(states, -2);
			return 2;
		}

		return 1;
	}

	__forceinline int getrawmetatable(lua_State* states)
	{
		luaL_checktypes(states, 1, LUA_TTABLE, LUA_TUSERDATA);
		lua_getmetatable(states, 1);

		return 1;
	}

	inline int setrawmetatable(lua_State* states)
	{
		luaL_checktypes(states, 1, LUA_TTABLE, LUA_TUSERDATA);
		luaL_checktype(states, 2, LUA_TTABLE);
		lua_settop(states, 2);
		lua_setmetatable(states, 1);

		return 0;
	}

	__forceinline int identifyexecutor(lua_State* states)
	{
		lua_pushstring(states, "Cryftol");
		lua_pushstring(states, "1.0.0");

		return 2;
	}

	inline int getnamecallmethod(lua_State* states)
	{
		lua_pushstring(states, states->namecall ? getstr(states->namecall) : NULL);

		return 1;
	}

	__forceinline int setnamecallmethod(lua_State* states)
	{
		luaL_checktype(states, 1, LUA_TSTRING);
		state->namecall = reinterpret_cast<TString*>(index2addr(states, 1)->value.p);

		return 0;
	}

	__forceinline std::size_t write_callback(void* contents, std::size_t size, std::size_t nmemb, void* userp)
	{
		((std::string*)userp)->append((char*)contents, size * nmemb);
		return (size * nmemb);
	}

	inline int httpget(lua_State* states)
	{
		luaL_checktype(states, 1, LUA_TSTRING);

		size_t len{};
		const char* url = lua_tolstring(states, 1, &len);

		curl_global_init(CURL_GLOBAL_DEFAULT);
		CURL* curl = curl_easy_init();

		if (curl)
		{
			std::string buffer;

			curl_easy_setopt(curl, CURLOPT_URL, std::string(url));
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);

			curl_easy_perform(curl);
			curl_easy_cleanup(curl);

			lua_pushlstring(state, buffer.data(), buffer.length());
			return 1;
		}

		curl_global_cleanup();
		return 0;
	}
	
	auto initialization(lua_State* state)
	{
		lua_newtable(state);
		lua_setglobal(state, "_G");

		lua_pushcclosure(state, getreg, "getreg", 0);
		lua_setglobal(state, "getreg");

		lua_pushcclosure(state, getgenv, "getgenv", 0);
		lua_setglobal(state, "getgenv");

		lua_pushcclosure(state, getrenv, "getrenv", 0);
		lua_setglobal(state, "getrenv");

		lua_pushcclosure(state, httpget, "HttpGet", 0);
		lua_setglobal(state, "HttpGet");

		lua_pushcclosure(state, isreadonly, "isreadonly", 0);
		lua_setglobal(state, "isreadonly");

		lua_pushcclosure(state, setreadonly, "setreadonly", 0);
		lua_setglobal(state, "setreadonly");

		lua_pushcclosure(state, loadstring, "loadstring", 0);
		lua_setglobal(state, "loadstring");

		lua_pushcclosure(state, getrawmetatable, "getrawmetatable", 0);
		lua_setglobal(state, "getrawmetatable");

		lua_pushcclosure(state, setrawmetatable, "setrawmetatable", 0);
		lua_setglobal(state, "setrawmetatable");

		lua_pushcclosure(state, identifyexecutor, "identifyexecutor", 0);
		lua_setglobal(state, "identifyexecutor");

		lua_pushcclosure(state, getnamecallmethod, "getnamecallmethod", 0);
		lua_setglobal(state, "getnamecallmethod");

		lua_pushcclosure(state, setnamecallmethod, "setnamecallmethod", 0);
		lua_setglobal(state, "setnamecallmethod");
	}
}