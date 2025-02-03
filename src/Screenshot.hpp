#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

#include <fstream>
#include <iostream>

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

	void SaveBMPFile(const void* buffer, int w, int h, const std::wstring& filename)
	{
		// Set up BITMAPINFO structure to describe the format of the bitmap
		BITMAPINFOHEADER bmiHeader = {};
		bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmiHeader.biWidth = w;
		bmiHeader.biHeight = -h;
		bmiHeader.biPlanes = 1;
		bmiHeader.biBitCount = 32;  // Assuming raw data is in BGRA or RGBA format
		bmiHeader.biCompression = BI_RGB;

		// Create a BITMAPINFO structure
		BITMAPINFO bitmapInfo = {};
		bitmapInfo.bmiHeader = bmiHeader;

		// Create an HBITMAP from the raw pixel data
		HBITMAP hBitmap = CreateDIBitmap(GetDC(NULL), &bmiHeader, CBM_INIT, buffer, &bitmapInfo, DIB_RGB_COLORS);

		if (hBitmap == NULL)
		{
			std::cerr << "Failed to create HBITMAP." << std::endl;
			return;
		}

		BITMAP bm;
		BITMAPINFOHEADER bi;

		// Get the bitmap information
		GetObject(hBitmap, sizeof(BITMAP), &bm);

		// Fill the BITMAPINFOHEADER
		ZeroMemory(&bi, sizeof(bi));
		bi.biSize = sizeof(bi);
		bi.biWidth = bm.bmWidth;
		bi.biHeight = bm.bmHeight;
		bi.biPlanes = 1;
		bi.biBitCount = 24;  // 24-bit image
		bi.biCompression = BI_RGB;
		bi.biSizeImage = bm.bmWidthBytes * bm.bmHeight;

		// Create a compatible DC and select the bitmap into it
		HDC hdc = CreateCompatibleDC(NULL);
		SelectObject(hdc, hBitmap);

		// Create a buffer to hold the image data
		DWORD dwBmpSize = bm.bmWidthBytes * bm.bmHeight;
		char* lpBitmapData = new char[dwBmpSize];

		// Get the bitmap bits
		GetDIBits(hdc, hBitmap, 0, bm.bmHeight, lpBitmapData, (BITMAPINFO*)&bi, DIB_RGB_COLORS);

		// Create the BMP file
		std::ofstream file(filename, std::ios::binary);
		if (!file) {
			std::cerr << "Failed to open file!" << std::endl;
			delete[] lpBitmapData;
			DeleteDC(hdc);
			DeleteObject(hBitmap);
			return;
		}

		// Write BMP Header
		BITMAPFILEHEADER bfh;
		ZeroMemory(&bfh, sizeof(bfh));
		bfh.bfType = 0x4D42;  // 'BM'
		bfh.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwBmpSize;
		bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

		file.write((const char*)&bfh, sizeof(bfh));
		file.write((const char*)&bi, sizeof(bi));

		// Write the pixel data
		file.write(lpBitmapData, dwBmpSize);

		file.close();
		delete[] lpBitmapData;
		DeleteDC(hdc);
		DeleteObject(hBitmap);
	}

	void SaveScreenshotFile(void* buffer)
	{
		int bytes_per_pixel = screen_bpp / 8;
		int pitch_x_bytes = screen_width * screen_bpp / 8;

		SaveBMPFile(buffer, screen_width, screen_height, L"Screenshots/screenshot.bmp");
	}

	void CaptureScreenshot()
	{
		zCTextureConvert* tex_cvt = zrenderer->CreateTextureConvert();
		zrenderer->Vid_GetFrontBufferCopy(*tex_cvt);

		void* buffer;
		int pitch_x_bytes = screen_width * screen_bpp / 8;

		tex_cvt->GetTextureBuffer(0, buffer, pitch_x_bytes);
		CopyToClipboard(buffer, screen_width, screen_height, screen_bpp / 8);
		SaveScreenshotFile(buffer);

		zsound->PlaySound(screenshot_sfx, 0);

		zDELETE(tex_cvt);
	}
}