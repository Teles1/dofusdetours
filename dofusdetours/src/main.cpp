#include <iostream>
#include <stdio.h>

#include "app/app.h"

#define UNLOAD_VIRTUAL_KEY VK_F12

DWORD __stdcall mainThread(LPVOID param)
{
	if (!app::load())
	{
		FreeLibraryAndExitThread(static_cast<HMODULE>(param), EXIT_SUCCESS);
	}

	while (!GetAsyncKeyState(UNLOAD_VIRTUAL_KEY))
	{
		Sleep(50);
	}

	if (!app::unload())
	{
		FreeLibraryAndExitThread(static_cast<HMODULE>(param), EXIT_SUCCESS);
	}

	Sleep(500);

	FreeLibraryAndExitThread(static_cast<HMODULE>(param), EXIT_SUCCESS);
}

int __stdcall DllMain(HINSTANCE inst, DWORD reason, LPVOID reserved)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		if (AllocConsole()) {
			(void)freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
			(void)freopen_s((FILE**)stdout, "CONOUT$", "w", stderr);
			SetConsoleTitle("Console");
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
			std::cerr << "DLL Loaded" << std::endl;
		}
		if (const auto main_thread{ CreateThread(nullptr, 0, mainThread, inst, 0, 0) })
		{
			CloseHandle(main_thread);
		}
	}else if (reason == DLL_PROCESS_DETACH)
	{
		std::cerr << "DLL Unloaded" << std::endl;

		FreeConsole();
	}

	return 1;
}