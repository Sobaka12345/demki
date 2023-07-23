#pragma once

#include "assert.hpp"

#include <filesystem>
#include <fstream>
#include <vector>

#ifdef _WIN32
#	include <windows.h>    //GetModuleFileNameW
#else
#	include <limits.h>
#	include <unistd.h>    //readlink
#endif

inline std::filesystem::path executablePath()
{
    static std::filesystem::path s_executablePath =
        []() -> std::filesystem::path {
#ifdef _WIN32
        wchar_t path[MAX_PATH] = { 0 };
        GetModuleFileNameW(NULL, path, MAX_PATH);
        return path;
#else
        char result[PATH_MAX];
        ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
        return std::string(result, (count > 0) ? count : 0);
#endif
    }()
                    .parent_path();

    return s_executablePath;
}

inline std::vector<char> readFile(std::filesystem::path filePath)
{
    if (filePath.is_relative())
    {
        filePath = executablePath() / filePath;
    }

    std::ifstream file(filePath, std::ios::ate | std::ios::binary);

    ASSERT(file.is_open(), ("failed to open file: " + filePath.string()).c_str());

    size_t fileSize = static_cast<size_t>(file.tellg());
    std::vector<char> buffer(fileSize);
    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();
    return buffer;
}
