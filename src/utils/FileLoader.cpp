#include "FileLoader.h"
#include <fstream>
#include <sstream>
#include <stdexcept>

std::string FileLoader::readAll(const std::string& filePath) {
    std::ifstream in(filePath);
    if (!in.is_open()) {
        throw std::runtime_error("Cannot open file: " + filePath);
    }

    std::ostringstream ss;
    ss << in.rdbuf();
    return ss.str();
}
