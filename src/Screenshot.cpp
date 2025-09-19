#include "Screenshot.h"

Screenshot::Screenshot(HWND hwnd)
{
    RECT rect;
    GetClientRect(hwnd, &rect);

    POINT point = { rect.left, rect.top };
    ClientToScreen(hwnd, &point);

    width_ = rect.right - rect.left;
    height_ = rect.bottom - rect.top;

    HDC hDC = GetDC(nullptr);
    HDC memDC = CreateCompatibleDC(hDC);

    BITMAPINFOHEADER biHeader = {};
    biHeader.biSize = sizeof(BITMAPINFOHEADER);
    biHeader.biBitCount = 32;
    biHeader.biCompression = BI_RGB;
    biHeader.biPlanes = 1;
    biHeader.biWidth = width_;
    biHeader.biHeight = -height_; // top-down

    BITMAPINFO bInfo = {};
    bInfo.bmiHeader = biHeader;

    bitmap_ = CreateDIBSection(hDC, &bInfo, DIB_RGB_COLORS, reinterpret_cast<void**>(&pixels_), nullptr, 0);

    SelectObject(memDC, bitmap_);
    BitBlt(memDC, 0, 0, width_, height_, hDC, point.x, point.y, SRCCOPY);

    DeleteDC(memDC);
    ReleaseDC(nullptr, hDC);
}

Screenshot::~Screenshot()
{
    if (bitmap_)
        DeleteObject(bitmap_);
}