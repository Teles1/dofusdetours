#pragma once

#include "hook/hook.h"
#include "hook_manager/hook_manager.h"

namespace utils
{
	inline void *get_process_address_safe(const char *module, const char *func)
	{
		const auto mod{ GetModuleHandleA(module) };

		if (!mod)
		{
			return nullptr;
		}

		return GetProcAddress(mod, func);
	}
}