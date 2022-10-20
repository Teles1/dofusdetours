#include "hook_manager.h"

#include <vector>


void hook::create(void* src, void* dst)
{
	MH_CreateHook(src, dst, &m_original);
}

void* hook::getInitFunc() const
{
	return m_init_fn;
}

hook::hook(void *init_fn)
{
	m_init_fn = init_fn;

	get_hook_manager().add_hook(this);
}

bool hook_manager::initAllHooks() const
{
	if (MH_Initialize() != MH_OK)
	{
		return false;
	}

	for (const auto &hook : hooks_)
	{
		const auto init{ hook.getInitFunc() };

		if (init == nullptr)
		{
			throw std::exception("init function cannot be null");
		}
		reinterpret_cast<void(*)()>(init)();
	}

	return MH_EnableHook(MH_ALL_HOOKS) == MH_OK;
}

bool hook_manager::freeAllHooks()
{
	if (MH_Uninitialize() != MH_OK)
	{
		return false;
	}

	return false;
}

void hook_manager::add_hook(hook* hook)
{
	std::scoped_lock lock(lock_);
	hooks_.emplace_back(*hook);
}

hook_manager& get_hook_manager()
{
	static hook_manager instance;
	return instance;
}
