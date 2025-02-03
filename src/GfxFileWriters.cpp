#include "GfxFileWriters.h"

#include <Windows.h>
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

#include <fstream>
#include <iostream>

void SaveBMP(const void* buffer, int w, int h, const std::string& filename)
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