#include "GfxFileWriters.h"

#include <Windows.h>
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

#include <fstream>

// global GDI Plus RAII
class GDIPlusManager
{
public:
    GDIPlusManager()
    {
        Gdiplus::GdiplusStartupInput gdiplusStartupInput;
        GdiplusStartup(&token, &gdiplusStartupInput, nullptr);
    }

    ~GDIPlusManager()
    {
        Gdiplus::GdiplusShutdown(token);
    }

    ULONG_PTR token;
};

static GDIPlusManager gdi_plus;

// Helper function to get encoder CLSID
static int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
    UINT num = 0, size = 0;
    Gdiplus::GetImageEncodersSize(&num, &size);
    if (size == 0)
        return -1;

    Gdiplus::ImageCodecInfo * pImageCodecInfo = (Gdiplus::ImageCodecInfo*)malloc(size);
    if (!pImageCodecInfo)
        return -1;

    GetImageEncoders(num, size, pImageCodecInfo);
    for (UINT i = 0; i < num; ++i)
    {
        if (wcscmp(pImageCodecInfo[i].MimeType, format) == 0)
        {
            *pClsid = pImageCodecInfo[i].Clsid;
            free(pImageCodecInfo);
            return 0;
        }
    }
    free(pImageCodecInfo);
    return -1;
}

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

bool SavePNG(const void* buffer, int w, int h, const std::string& filename)
{
    // Create a GDI+ Bitmap
    Gdiplus::Bitmap bitmap(w, h, PixelFormat32bppARGB);
    Gdiplus::BitmapData bmpData;

    // Lock the bitmap for direct pixel access
    Gdiplus::Rect rect(0, 0, w, h);
    if (bitmap.LockBits(&rect, Gdiplus::ImageLockModeWrite, PixelFormat32bppARGB, &bmpData) != Gdiplus::Ok)
        return false;

    // Convert BGRA buffer to BGR
    const uint8_t* src = reinterpret_cast<const uint8_t*>(buffer);
    uint8_t* dest = reinterpret_cast<uint8_t*>(bmpData.Scan0);
    for (size_t i = 0; i < w * h; ++i)
    {
        int idx = i * 4;

        uint8_t blue = src[idx + 0];
        uint8_t green = src[idx + 1];
        uint8_t red = src[idx + 2];

        dest[idx + 0] = blue;
        dest[idx + 1] = green;
        dest[idx + 2] = red;
        dest[idx + 3] = 255;
    }

    bitmap.UnlockBits(&bmpData);

    // Get the PNG encoder CLSID
    CLSID pngClsid;
    if (GetEncoderClsid(L"image/png", &pngClsid) == -1)
        return false;

    // Save the image
    std::wstring wFilename(filename.begin(), filename.end());
    return bitmap.Save(wFilename.c_str(), &pngClsid, nullptr) == Gdiplus::Ok;
}

bool SaveJPG(const void* buffer, int w, int h, const std::string& filename, unsigned long quality)
{
    Gdiplus::Bitmap bitmap(w, h, w * 4, PixelFormat32bppARGB, (BYTE*)buffer);

    CLSID jpgClsid;
    if (GetEncoderClsid(L"image/jpeg", &jpgClsid) != 0)
        return false;

    //if (Gdiplus::GetImageEncodersSize(nullptr, nullptr) != Gdiplus::Ok
    //||  Gdiplus::GetImageEncoders(1, sizeof(Gdiplus::ImageCodecInfo), reinterpret_cast<Gdiplus::ImageCodecInfo*>(&jpgClsid)) != Gdiplus::Ok)
    //    return false;

    Gdiplus::EncoderParameters encoderParams;
    encoderParams.Count = 1;
    encoderParams.Parameter[0].Guid = Gdiplus::EncoderQuality;
    encoderParams.Parameter[0].Type = Gdiplus::EncoderParameterValueTypeLong;
    encoderParams.Parameter[0].NumberOfValues = 1;
    encoderParams.Parameter[0].Value = &quality;

    return bitmap.Save(std::wstring(filename.begin(), filename.end()).c_str(), &jpgClsid, &encoderParams) == Gdiplus::Ok;
}