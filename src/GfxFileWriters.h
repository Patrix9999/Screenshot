#pragma once

#include <string>

bool SaveBMP(const void* buffer, int w, int h, const std::string& filename);
bool SavePNG(const void* buffer, int w, int h, const std::string& filename);
bool SaveJPG(const void* buffer, int w, int h, const std::string& filename, unsigned long quality = 95);