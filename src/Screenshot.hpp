namespace GOTHIC_NAMESPACE
{
	zCSoundFX* screenshot_sfx = nullptr;

	int screen_width = 0;
	int screen_height = 0;
	int screen_bpp = 0;

	zSTRING screenshot_sound = "carve02.wav";
	zSTRING screenshot_file_type = "jpg";
	unsigned long screenshot_jpg_quality = 95;
	bool screenshot_border_fix = true;

	bool IsUsingDirectX11()
	{
		DDDEVICEIDENTIFIER2 identifier;

		reinterpret_cast<zCRnd_D3D*>(zrenderer)->xd3d_pdd7->GetDeviceIdentifier(&identifier, NULL);
		return strcmp(identifier.szDriver, "DirectX11") == 0;
	}

	void UpdateScreenInfo()
	{
		// Update screen information
		screen_width = zoptions->ReadInt("VIDEO", "zVidResFullscreenX", 800);
		screen_height = zoptions->ReadInt("VIDEO", "zVidResFullscreenY", 600);
		screen_bpp = zoptions->ReadInt("VIDEO", "zVidResFullscreenBPP", 32);
	}

	std::string GetScreenshotFilePath(const zSTRING& extension)
	{
		auto now = std::chrono::system_clock::now();
		auto now_time_t = std::chrono::system_clock::to_time_t(now);

		std::tm now_tm;
		localtime_s(&now_tm, &now_time_t);

		std::ostringstream ts_stream;
		ts_stream << std::put_time(&now_tm, "%Y-%m-%d-%H-%M-%S");

		std::string screenshot_path = "Screens/";
		screenshot_path += ts_stream.str();
		screenshot_path += ".";
		screenshot_path += extension.ToChar();

		return screenshot_path;
	}

	void ReadConfigValues()
	{
		screenshot_sound = zoptions->ReadString("Screenshot", "sfx", screenshot_sound);
		screenshot_file_type = zoptions->ReadString("Screenshot", "file_type", screenshot_file_type);
		screenshot_jpg_quality = zoptions->ReadDWord("Screenshot", "jpg_quality", screenshot_jpg_quality);
		screenshot_border_fix = zoptions->ReadBool("Screenshot", "border_fix", screenshot_border_fix);

		// Border fix for fullscreen mode (compatibility mode for older games)
		if (screenshot_border_fix)
		{
			auto SetAppCompatData = reinterpret_cast<void(WINAPI*)(DWORD, DWORD)>(GetProcAddress(GetModuleHandleA("ddraw.dll"), "SetAppCompatData"));
			if (SetAppCompatData) SetAppCompatData(12, 0);
		}
	}

	bool CreateScreensSubfolder(const std::string& filename)
	{
		std::filesystem::path file_path = filename;
		std::filesystem::path dir_path = file_path.parent_path();

		if (std::filesystem::exists(dir_path) && std::filesystem::is_directory(dir_path))
			return true;

		try
		{
			if (!std::filesystem::create_directories(dir_path))
				return false;
		}
		catch (const std::filesystem::filesystem_error& e)
		{
			return false;
		}

		return true;
	}

	void SaveScreenshotFile(void* buffer)
	{
		std::string screenshot_file_path = GetScreenshotFilePath(screenshot_file_type);
		if (!CreateScreensSubfolder(screenshot_file_path))
			return;

		if (screenshot_file_type == "jpg")
			SaveJPG(buffer, screen_width, screen_height, screenshot_file_path, screenshot_jpg_quality);
		else if (screenshot_file_type == "png")
			SavePNG(buffer, screen_width, screen_height, screenshot_file_path);
		else if (screenshot_file_type == "bmp")
			SaveBMP(buffer, screen_width, screen_height, screenshot_file_path);
	}

	void CaptureScreenshot()
	{
		// Hacky fix for crash inside the menu while using dx11 backend
		static const bool dx11_used = IsUsingDirectX11();
		if (dx11_used && !zCCamera::activeCam)
			return;

		zCTextureConvert* tex_cvt = zrenderer->CreateTextureConvert();
		zrenderer->Vid_GetFrontBufferCopy(*tex_cvt);

		void* buffer;
		int pitch_x_bytes = screen_width * screen_bpp / 8;

		tex_cvt->GetTextureBuffer(0, buffer, pitch_x_bytes);
		SaveScreenshotFile(buffer);

		if (screenshot_sfx)
			zsound->PlaySound(screenshot_sfx, 0);

		zDELETE(tex_cvt);
	}
}