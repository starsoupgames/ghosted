#pragma once

#ifndef __NETWORK_UTILS_H__
#define __NETWORK_UTILS_H__
#include <cugl/cugl.h>
#include "Utils.h"

/** The precision to multiply floating point numbers by */
constexpr float FLOAT_PRECISION = 100.0f;

using namespace cugl;
using namespace std;

/** Functions to convert data types to byte vectors */
namespace NetworkUtils {
    /** Split byte vector at certain points */
    inline vector<vector<uint8_t>> split(const vector<uint8_t>& bytes, const vector<unsigned>& sizes) {
        bool raiseError = false;
        int index = 0;
        for (auto& size : sizes) {
            index += size;
            if (bytes.size() < index) {
                raiseError = true;
            }
        }
        vector<vector<uint8_t>> result;
        if (raiseError) {
            CULogError("Byte vector is too small. Byte vector size: %d. Expected size: %d.", bytes.size(), index);
            return result;
        }
        index = 0;
        for (auto& size : sizes) {
            result.push_back(vector(bytes.begin() + index, bytes.begin() + index + size));
            index += size;
        }
        if (index != bytes.size()) {
            result.push_back(vector(bytes.begin() + index, bytes.begin() + bytes.size()));
        }
        return result;
    }

    // byte (1 byte)
    inline void encodeByte(uint8_t b, vector<uint8_t>& out) {
        out.push_back(b);
    }

    inline uint8_t decodeByte(const vector<uint8_t>& bytes) {
        CUAssertLog(bytes.size() == 1, "decodeByte: Byte vector size is not equal to data type size.");
        return bytes[0];
    }

    // bool (1 byte)
    inline void encodeBool(bool b, vector<uint8_t>& out) {
        out.push_back(b);
    }

    inline bool decodeBool(const vector<uint8_t>& bytes) {
        CUAssertLog(bytes.size() == 1, "decodeBool: Byte vector size is not equal to data type size.");
        return (bool)bytes[0];
    }

    // int (4 bytes)
    // adapted from https://os.mbed.com/forum/helloworld/topic/2053/?page=1#comment-54126
    inline void encodeInt(int i, vector<uint8_t>& out) {
        int temp = marshall(i);
        unsigned char bytes[4];
        bytes[0] = (temp >> 24) & 0xFF;
        bytes[1] = (temp >> 16) & 0xFF;
        bytes[2] = (temp >> 8) & 0xFF;
        bytes[3] = temp & 0xFF;
        out.insert(out.end(), begin(bytes), end(bytes));
    }

    inline int decodeInt(const vector<uint8_t>& bytes) {
        CUAssertLog(bytes.size() == sizeof(int), "decodeInt: Byte vector size is not equal to data type size.");
        int32_t i = (bytes[0] << 24) + (bytes[1] << 16) + (bytes[2] << 8) + bytes[3];
        return marshall(i);
    }

    // float (4 bytes)
    inline void encodeFloat(float f, vector<uint8_t>& out) {
        encodeInt(static_cast<int>(f * FLOAT_PRECISION), out);
    }

    inline float decodeFloat(const vector<uint8_t>& bytes) {
        return static_cast<float>(decodeInt(bytes)) / FLOAT_PRECISION;
    }

    // Vec2 (8 bytes)
    inline void encodeVector(const Vec2& v, vector<uint8_t>& out) {
        encodeFloat(v.x, out);
        encodeFloat(v.y, out);
    }

    inline Vec2 decodeVector(const vector<uint8_t>& bytes) {
        CUAssertLog(bytes.size() == 2 * sizeof(float), "decodeVector: Byte vector size is not equal to data type size.");
        vector<vector<uint8_t>> splitBytes = split(bytes, { 4, 4 });
        return Vec2(decodeFloat(splitBytes[0]), decodeFloat(splitBytes[1]));
    }

    // int vector (variable bytes)
    inline void encodeIntList(const vector<int>& list, vector<uint8_t>& out) {
        encodeInt((int)list.size(), out);
        for (auto element : list) {
            encodeInt(element, out);
        }
    }

    inline vector<int> decodeIntList(int length, const vector<uint8_t>& bytes) { // needs to be defined before decodeList(bytes)
        CUAssertLog(bytes.size() == length * sizeof(int), "decodeIntList2: Byte vector size is not equal to data type size.");
        vector<vector<uint8_t>> subdividedList;
        for (unsigned i = 0; i < bytes.size(); i+=4) {
            subdividedList.push_back(vector(bytes.begin() + i, bytes.begin() + i + 4));
        }
        vector<int> result;
        for (auto& encodedInt : subdividedList) {
            result.push_back(decodeInt(encodedInt));
        }
        return result;
    }

    inline vector<int> decodeIntList(const vector<uint8_t>& bytes) {
        CUAssertLog(bytes.size() > sizeof(int), "decodeIntList1: Byte vector size is not equal to data type size.");
        vector<uint8_t> encodedLength(bytes.begin(), bytes.begin() + 4);
        int length = decodeInt(encodedLength);
        return decodeIntList(length, vector(bytes.begin() + 4, bytes.begin() + bytes.size()));
    }

    // bool vector (variable bytes)
    inline void encodeBoolList(const vector<bool>& list, vector<uint8_t>& out) {
        encodeInt((int)list.size(), out);
        for (auto element : list) {
            encodeBool(element, out);
        }
    }

    inline vector<bool> decodeBoolList(int length, const vector<uint8_t>& bytes) { // needs to be defined before decodeList(bytes)
        CUAssertLog(bytes.size() == length, "decodeBoolList2: Byte vector size is not equal to data type size.");
        vector<vector<uint8_t>> subdividedList;
        for (unsigned i = 0; i < bytes.size(); i += 1) {
            subdividedList.push_back(vector(bytes.begin() + i, bytes.begin() + i + 1));
        }
        vector<bool> result;
        for (auto& encodedBool : subdividedList) {
            result.push_back(decodeBool(encodedBool));
        }
        return result;
    }

    inline vector<bool> decodeBoolList(const vector<uint8_t>& bytes) {
        CUAssertLog(bytes.size() > sizeof(int), "decodeBoolList1: Byte vector size is not equal to data type size.");
        vector<uint8_t> encodedLength(bytes.begin(), bytes.begin() + 4);
        int length = decodeInt(encodedLength);
        return decodeBoolList(length, vector(bytes.begin() + 4, bytes.begin() + bytes.size()));
    }

    // Vec2 vector (variable bytes)
    inline void encodeVec2List(const vector<Vec2>& list, vector<uint8_t>& out) {
        encodeInt((int)list.size(), out);
        for (auto& element : list) {
            encodeVector(element, out);
        }
    }

    inline vector<Vec2> decodeVec2List(int length, const vector<uint8_t>& bytes) { // needs to be defined before decodeList(bytes)
        CUAssertLog(bytes.size() == length * 2 * sizeof(float), "decodeVec2List2: Byte vector size is not equal to data type size.");
        vector<vector<uint8_t>> subdividedList;
        for (unsigned i = 0; i < bytes.size(); i += 8) {
            subdividedList.push_back(vector(bytes.begin() + i, bytes.begin() + i + 8));
        }
        vector<Vec2> result;
        for (auto& encodedVec2 : subdividedList) {
            result.push_back(decodeVector(encodedVec2));
        }
        return result;
    }

    inline vector<Vec2> decodeVec2List(const vector<uint8_t>& bytes) {
        CUAssertLog(bytes.size() > sizeof(int), "decodeVec2List1: Byte vector size is not equal to data type size.");
        auto splitBytes = split(bytes, { 4 });
        int length = decodeInt(splitBytes[0]);
        vector<Vec2> result;
        if (length == 0) {
            return result;
        }
        return decodeVec2List(length, splitBytes[1]);
    }
};
#endif /** __NETWORK_UTILS_H__ */