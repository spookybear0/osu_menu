#pragma once

#include <windows.h>

namespace utilities {
    uintptr_t getBase() {
        return reinterpret_cast<uintptr_t>(GetModuleHandle(0));
    }

    std::string getGDPath() {
        char path_c[MAX_PATH + 1];
        GetModuleFileNameA(NULL, path_c, sizeof(path_c));
        size_t pos = std::string::npos;
        std::string path = std::string(path_c);
        while ((pos = path.find("GeometryDash.exe")) != std::string::npos)
        {
            path.erase(pos, std::string("GeometryDash.exe").length());
        }
        return path;
    }
}
