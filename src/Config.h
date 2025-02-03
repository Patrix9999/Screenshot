#include <string>

bool ConfigFileExists();
void CreateDefaultConfigFile();
std::string GetConfigOptionString(const std::string& section, const std::string& option);
long GetConfigOptionLong(const std::string& section, const std::string& option);