#include <thread>
#include "functions.h"

std::string httphook = R"(
local httpget_func = function(self, ...)
    return HttpGet(...)
end

local gaem = game
local gmt = getrawmetatable(game)
local oldnamecall = gmt.__namecall
local oldindex = gmt.__index

setreadonly(gmt, false)

gmt.__index = function(self, i)
    if self == gaem and (i == 'HttpGet' or i == 'HttpGetAsync') then
        return httpget_func
    end
    return oldindex(self, i)
end

gmt.__namecall = function(self, ...)
    if self == gaem and (getnamecallmethod() == 'HttpGet' or getnamecallmethod() == 'HttpGetAsync') then
        return httpget_func(self, ...)
    end
    return oldnamecall(self, ...)
end

setreadonly(gmt, true)
)";

int main()
{
	state = get_global_state(scheduler::get_script_context());
	*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(state->userdata) + 24) = 8;

	luaL_sandboxthread(state);
	functions::initialization(state);

	execute(httphook); // Do not delete if you wanna support httpget
	execute("print(identifyexecutor())");
}

int __stdcall DllMain(void*, int reason, void*)
{
	if (reason == 1)
	{
		std::thread(main).detach();
	}
	return 1;
}