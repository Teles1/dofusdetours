#pragma once

#include <mutex>
#include <vector>

#include "../hook/hook.h"

struct hook_manager
{
	bool initAllHooks() const;
	static bool freeAllHooks();
	void add_hook(hook* init_fn);
private:
	std::vector<hook> hooks_{};
	std::mutex lock_;
};
hook_manager& get_hook_manager();