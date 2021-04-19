#pragma once

#ifndef __UTILS_H__
#define __UTILS_H__

#include <cugl/cugl.h>

using namespace std;
using namespace cugl;

/**
 * Namespace for utility functions that should be globally accessible
 */
namespace utils {

    // logging functions
    inline void Log(const string& s) {
        CULog(s.c_str());
    }
    inline void Log(const float f) {
        CULog("%f", f);
    }
    inline void Log(const Vec2& v) {
        CULog("%f, %f", v.x, v.y);
    }
    inline void Log(const string& label, const Vec2& v) {
        CULog((label + ": %f, %f").c_str(), v.x, v.y);
    }
    inline void LogWithNoise(const string& s) {
        int n = rand() % 26;
        string c(1, (char)(n + 65));
        CULog((s + " " + c).c_str());
    }

    // https://stackoverflow.com/questions/4654636/how-to-determine-if-a-string-is-a-number-with-c
    inline bool isNumeric(const string& s) {
        return !s.empty() && s.find_first_not_of("0123456789") == string::npos;
    }

}

#endif // __UTILS_H__
