namespace GOTHIC_NAMESPACE
{
	zCSoundFX* screenshot_sfx = nullptr;

	int screen_width = 0;
	int screen_height = 0;
	int screen_bpp = 0;

	void UpdateScreenInfo()
	{
		// Update screen information
		screen_width = zoptions->ReadInt("VIDEO", "zVidResFullscreenX", 800);
		screen_height = zoptions->ReadInt("VIDEO", "zVidResFullscreenY", 600);
		screen_bpp = zoptions->ReadInt("VIDEO", "zVidResFullscreenBPP", 32);
	}

	// Function to copy raw image data to clipboard (useful for debugging)
	bool CopyToClipboard(const void* pPixels, int width, int height, int bytesPerPixel)
	{
		if (!pPixels) return false;

		// Define bitmap info
		BITMAPINFO bmi = {};
		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth = width;
		bmi.bmiHeader.biHeight = -height;  // Negative height to store it top-down
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = bytesPerPixel * 8;
		bmi.bmiHeader.biCompression = BI_RGB;
		bmi.bmiHeader.biSizeImage = width * height * bytesPerPixel;

		// Allocate memory for DIB
		HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, sizeof(BITMAPINFOHEADER) + bmi.bmiHeader.biSizeImage);
		if (!hMem) return false;

		void* pMem = GlobalLock(hMem);
		if (!pMem) {
			GlobalFree(hMem);
			return false;
		}

		// Copy the BITMAPINFOHEADER
		memcpy(pMem, &bmi.bmiHeader, sizeof(BITMAPINFOHEADER));
		// Copy pixel data after the header
		memcpy((BYTE*)pMem + sizeof(BITMAPINFOHEADER), pPixels, bmi.bmiHeader.biSizeImage);

		GlobalUnlock(hMem);

		// Open clipboard and set the data
		if (OpenClipboard(nullptr)) {
			EmptyClipboard();
			SetClipboardData(CF_DIB, hMem);
			CloseClipboard();
			return true;
		}

		GlobalFree(hMem);
		return false;
	}

	std::string GetScreenshotFilePath(const std::string& extension)
	{
		auto now = std::chrono::system_clock::now();
		auto now_time_t = std::chrono::system_clock::to_time_t(now);

		std::tm now_tm;
		localtime_s(&now_tm, &now_time_t);

		std::ostringstream ts_stream;
		ts_stream << std::put_time(&now_tm, "%Y-%m-%d-%H-%M-%S");

		std::string screenshot_path = "Screens/";
		screenshot_path += ts_stream.str();
		screenshot_path += extension;

		return screenshot_path;
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
		std::string screenshot_file_path = GetScreenshotFilePath(".jpg");
		if (!CreateScreensSubfolder(screenshot_file_path))
			return;

		SaveJPG(buffer, screen_width, screen_height, screenshot_file_path);
	}

	void CaptureScreenshot()
	{
		zCTextureConvert* tex_cvt = zrenderer->CreateTextureConvert();
		zrenderer->Vid_GetFrontBufferCopy(*tex_cvt);

		void* buffer;
		int pitch_x_bytes = screen_width * screen_bpp / 8;

		tex_cvt->GetTextureBuffer(0, buffer, pitch_x_bytes);
		SaveScreenshotFile(buffer);

		zsound->PlaySound(screenshot_sfx, 0);

		zDELETE(tex_cvt);
	}
}