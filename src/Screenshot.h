#pragma once

#include <Windows.h>

class Screenshot
{
public:
    Screenshot(HWND hwnd);
    ~Screenshot();

    HBITMAP GetBitmap() const { return bitmap_; }
    BYTE* GetPixels() const { return pixels_; }
    int GetWidth() const { return width_; }
    int GetHeight() const { return height_; }

private:
    HBITMAP bitmap_ = nullptr;
    BYTE* pixels_ = nullptr;
    int width_ = 0;
    int height_ = 0;
};