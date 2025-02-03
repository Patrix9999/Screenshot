namespace GOTHIC_NAMESPACE
{
	auto AppWndProc_Original = (LRESULT(CALLBACK*)(HWND, zUWORD, WPARAM, LPARAM))zSwitch(0x004F4EE0, 0x005078E0, 0x00500A80, 0x00503770);

	LRESULT CALLBACK AppWndProc(HWND hwnd, zUWORD msg, WPARAM wParam, LPARAM lParam);
	auto Hook_AppWndProc = Union::CreateHook(reinterpret_cast<void*>(zSwitch(0x004F4EE0, 0x005078E0, 0x00500A80, 0x00503770)), &AppWndProc, Union::HookType::Hook_Detours);
	LRESULT CALLBACK AppWndProc(HWND hwnd, zUWORD msg, WPARAM wParam, LPARAM lParam)
	{
		Hook_AppWndProc.Disable();

		switch (msg)
		{
			case WM_KEYUP:
			{
				// wParam: The virtual-key code of the nonsystem key.
				// lParam: The repeat count, scan code, extended-key flag, context code, previous key-state flag, and transition-state flag

				if (wParam == VK_SNAPSHOT)
					CaptureScreenshot();

				break;
			}
		}

		LRESULT result = AppWndProc_Original(hwnd, msg, wParam, lParam);

		Hook_AppWndProc.Enable();

		return result;
	}

	auto Hook_oCGame_UpdateScreenResolution = Union::CreateHook(reinterpret_cast<void*>(zSwitch(0x00638C50, 0x0065F1A0, 0x00666300, 0x006C2E00)), &oCGame::Hook_UpdateScreenResolution, Union::HookType::Hook_Detours);
	void oCGame::Hook_UpdateScreenResolution()
	{
		Hook_oCGame_UpdateScreenResolution.Disable();

		this->UpdateScreenResolution();
		UpdateScreenInfo();

		Hook_oCGame_UpdateScreenResolution.Enable();
	}
}