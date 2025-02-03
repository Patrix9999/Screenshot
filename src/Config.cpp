#include "Config.h"

#include <fstream>
#include <filesystem>

#include <Windows.h>

extern HMODULE current_dll_handle;

static std::string GetIniFilePath()
{
    CHAR ini_file_name[] = "Screenshot.ini";
    static CHAR ini_file_path[MAX_PATH] = {};

    DWORD result = GetModuleFileNameA(current_dll_handle, ini_file_path, sizeof(ini_file_path));
    if (result == 0 || result >= MAX_PATH)
        return "";

    ini_file_path[strrchr(ini_file_path, '\\') - ini_file_path + 1] = NULL;
    strcat_s(ini_file_path, ini_file_name);

    return ini_file_path;
}

bool ConfigFileExists()
{
    std::filesystem::path ini_file_path = GetIniFilePath();
    return std::filesystem::exists(ini_file_path);
}

void CreateDefaultConfigFile()
{
    std::ofstream file(GetIniFilePath());

    file << "[Settings]" << std::endl;
    file << "sfx=carve02.wav" << std::endl;
    file << "file_type=jpg" << std::endl;
    file << "jpg_quality=95" << std::endl;
    file << "border_fix=1" << std::endl;
}

std::string GetConfigOptionString(const std::string& section, const std::string& option)
{
    std::string file_path = GetIniFilePath();
    if (file_path.empty())
        return "";

    static CHAR value[64] = {};
    GetPrivateProfileStringA(section.c_str(), option.c_str(), NULL, value, sizeof(value), file_path.c_str());

    return value;
}

long GetConfigOptionLong(const std::string& section, const std::string& option)
{
    static std::string file_path = GetIniFilePath();
    if (file_path.empty())
        return 0;

    static CHAR value[16] = {};
    GetPrivateProfileStringA(section.c_str(), option.c_str(), NULL, value, sizeof(value), file_path.c_str());

    long result = 0;

    try
    {
        result = std::stol(std::string(value));
    }
    catch (...)
    {

    }

    return result;
}
