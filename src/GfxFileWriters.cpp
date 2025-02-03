#include "GfxFileWriters.h"

#include <Windows.h>
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

#include <fstream>

bool SaveBMP(const void* buffer, int w, int h, const std::string& filename)
{
    BITMAPFILEHEADER bfh = {};
    bfh.bfType = 0x4D42;
    bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    bfh.bfSize = bfh.bfOffBits + (w * h * 4);

    BITMAPINFOHEADER bih = {};
    bih.biSize = sizeof(BITMAPINFOHEADER);
    bih.biWidth = w;
    bih.biHeight = -h;  // Negative to keep the image top-down
    bih.biPlanes = 1;
    bih.biBitCount = 32; // 32-bit (BGRA format)
    bih.biCompression = BI_RGB;

    std::ofstream file(filename, std::ios::binary);
    if (!file)
        return false;

    file.write(reinterpret_cast<const char*>(&bfh), sizeof(bfh));
    file.write(reinterpret_cast<const char*>(&bih), sizeof(bih));
    file.write(reinterpret_cast<const char*>(buffer), w * h * 4);

    file.close();

    return true;
}