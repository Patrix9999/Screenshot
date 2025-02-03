#include <Windows.h>

HMODULE current_dll_handle;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		current_dll_handle = hModule;
		break;

	case DLL_PROCESS_DETACH:
		break;
	}

	return TRUE;
}