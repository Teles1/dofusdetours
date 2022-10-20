#pragma once

#include "MinHook.h"

class hook
{
private:
	void *m_original{ nullptr };
	void *m_init_fn{ nullptr };

public:
	void create(void *src, void *dst);

public:
	template<typename fn> fn original()
	{
		return reinterpret_cast<fn>(m_original);
	}

	void* getInitFunc() const;

public:
	hook(void *init_fn);
};

#define MAKE_HOOK(name, address, type, callconvo, ...) namespace hooks \
{\
	namespace name\
	{\
		void init(); \
		static ::hook hook(init); \
		using fn = type(callconvo *)(__VA_ARGS__); \
		type callconvo func(__VA_ARGS__); \
	}\
} \
void hooks::name::init() { hook.create(reinterpret_cast<void *>(address), reinterpret_cast<void *>(func)); } \
type callconvo hooks::name::func(__VA_ARGS__)

#define CALL_ORIGINAL hook.original<fn>()