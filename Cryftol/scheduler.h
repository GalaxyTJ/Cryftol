#pragma once

#include <string_view>

#include "addresses.h"

namespace scheduler
{
    inline int get_job_by_name(std::string job_name)
    {
        int job_return{};

        std::uint32_t job_start = *reinterpret_cast<uintptr_t*>(taskscheduler() + 308);

        while (job_start != *reinterpret_cast<uintptr_t*>(taskscheduler() + 312))
        {
            if (*reinterpret_cast<std::string*>(*reinterpret_cast<uintptr_t*>(job_start) + 16) == job_name)
            {
                job_return = *reinterpret_cast<uintptr_t*>(job_start);
            }
            job_start += 8;
        }
        return job_return;
    }

    __forceinline int get_script_context()
    {
        return *reinterpret_cast<uintptr_t*>(get_job_by_name("WaitingHybridScriptsJob") + 304);
    }
}