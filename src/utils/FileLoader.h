#pragma once
#include <string>

class FileLoader
{
public:
    static std::string readAll(const std::string& filePath);
};
