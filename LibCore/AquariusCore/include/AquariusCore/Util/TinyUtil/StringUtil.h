/*+==================================================================
 * Copyright (C) 2017 XAQUARIUS. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * FileName: StringUtil.h
 * 
 * Purpose:  Define String util function
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/2/22 15:44
===================================================================+*/
#ifndef _STRING_UTIL_H_
#define _STRING_UTIL_H_ 
#include <string>
#include <vector>
#include "AquariusCore/Util/TypeReDef/TypeReDef.hpp"
#include "AquariusCore/Logger/AquariusLogger.h"
#include "AquariusCore/Util/TinyUtil/CommonUtil.h"

class CStringUtil
{
public:
    // Trim character from string
    static std::string& TrimCharacter(std::string& strValue, char ch);


    // Encode original string to hex string
    static std::string EncodeStringToHex(const std::string& strInput);

    // Decode hex string
    static std::string DecodeHexToString(const std::string& strInput, bool bWarningEmptySrc);

    // Split string to token
    static std::vector<std::string> SplitStringToToken(const std::string& strInput, char chDelim);

    // Split string to int vector
    static std::vector<int> SplitStringToIntVector(const std::string& strInput, char chDelim);

    // format uint32 to string
    static std::string UInt32ToDecString(unsigned int nValue);

    // format int32 to string
    static std::string Int32ToDecString(int nValue);

    // Format uint16 to hex string
    static std::string UInt16ToHexString(unsigned short nValue, bool bHas0X = true);

    // Make safe string
    template<unsigned int CHAR_COUNT>
    inline static std::string BuildSafeString(const char(&data)[CHAR_COUNT])
    {
        static_assert((CHAR_COUNT >= 1), "Data size too small");
        char safe_copy[CHAR_COUNT] = { 0 };
#ifdef WIN32
        memcpy_s(safe_copy, CHAR_COUNT, data, CHAR_COUNT);
#else
        memcpy(safe_copy, data, CHAR_COUNT);
#endif // Linux
        safe_copy[CHAR_COUNT - 1] = 0;
        return std::string(safe_copy);
    }

    // Convert char to string
    static inline const char* CharToStr(unsigned char ch)
    {
        static const char* s_chArray[0xFF + 1] = 
        {"00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "0A", "0B", "0C", "0D", "0E", "0F", 
        "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "1A", "1B", "1C", "1D", "1E", "1F",
        "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "2A", "2B", "2C", "2D", "2E", "2F",
        "30", "31", "32", "33", "34", "35", "36", "37", "38", "39", "3A", "3B", "3C", "3D", "3E", "3F",
        "40", "41", "42", "43", "44", "45", "46", "47", "48", "49", "4A", "4B", "4C", "4D", "4E", "4F",
        "50", "51", "52", "53", "54", "55", "56", "57", "58", "59", "5A", "5B", "5C", "5D", "5E", "5F",
        "60", "61", "62", "63", "64", "65", "66", "67", "68", "69", "6A", "6B", "6C", "6D", "6E", "6F",
        "70", "71", "72", "73", "74", "75", "76", "77", "78", "79", "7A", "7B", "7C", "7D", "7E", "7F",
        "80", "81", "82", "83", "84", "85", "86", "87", "88", "89", "8A", "8B", "8C", "8D", "8E", "8F",
        "90", "91", "92", "93", "94", "95", "96", "97", "98", "99", "9A", "9B", "9C", "9D", "9E", "9F",
        "A0", "A1", "A2", "A3", "A4", "A5", "A6", "A7", "A8", "A9", "AA", "AB", "AC", "AD", "AE", "AF",
        "B0", "B1", "B2", "B3", "B4", "B5", "B6", "B7", "B8", "B9", "BA", "BB", "BC", "BD", "BE", "BF",
        "C0", "C1", "C2", "C3", "C4", "C5", "C6", "C7", "C8", "C9", "CA", "CB", "CC", "CD", "CE", "CF",
        "D0", "D1", "D2", "D3", "D4", "D5", "D6", "D7", "D8", "D9", "DA", "DB", "DC", "DD", "DE", "DF",
        "E0", "E1", "E2", "E3", "E4", "E5", "E6", "E7", "E8", "E9", "EA", "EB", "EC", "ED", "EE", "EF",
        "F0", "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "FA", "FB", "FC", "FD", "FE", "FF"};
        return s_chArray[ch];
    }

    // Convert char to number value
    static unsigned char CharToNum(unsigned char ch);

    // Encode url
    static std::string UrlEncode(const std::string& strInput);

    // Decode url
    static std::string UrlDecode(const std::string& strInput);

    // Generate rand string
    static std::string GenerateRandString(AUINT32 nLength);

private:
    // Url Encode
    inline static unsigned char UrlEncodeToHex(unsigned char x) 
    { 
        return  x > 9 ? x + 55 : x + 48; 
    }

    // Url UrlDecode
    static unsigned char UrlDecodeFromHex(unsigned char x);
};

#endif // _STRING_UTIL_H_
