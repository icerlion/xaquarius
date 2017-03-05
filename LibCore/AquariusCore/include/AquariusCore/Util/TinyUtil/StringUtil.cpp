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
 * FileName: StringUtil.cpp
 * 
 * Purpose:  Implement String util function
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/2/22 15:48
===================================================================+*/
#include "AquariusCore/Util/TinyUtil/StringUtil.h"
#include "AquariusCore/Util/TinyUtil/RandUtil.h"
#include "boost/locale.hpp"
#include "boost/algorithm/string.hpp"

std::vector<std::string> CStringUtil::SplitStringToToken(const std::string& strInput, char chDelim)
{
    std::vector<std::string> vResult;
    std::string strToken;
    for each (char chValue in strInput)
    {
        if (chValue != chDelim)
        {
            strToken.append(1, chValue);
        }
        else
        {
            vResult.push_back(strToken);
            strToken.clear();
        }
    }
    if (!strToken.empty())
    {
        vResult.push_back(strToken);
    }
    return vResult;
}

std::vector<int> CStringUtil::SplitStringToIntVector(const std::string& strInput, char chDelim)
{
    std::vector<int> vResult;
    std::vector<std::string> vToken = SplitStringToToken(strInput, chDelim);
    vResult.reserve(vToken.size());
    for each (const std::string& strValue in vToken)
    {
        vResult.push_back(atoi(strValue.c_str()));
    }
    return vResult;
}

std::string CStringUtil::UInt32ToDecString(unsigned int nValue)
{
    char buff[32] = { 0 };
    a_sprintf(buff, "%u", nValue);
    return std::string(buff);
}

std::string CStringUtil::Int32ToDecString(int nValue)
{
    char buff[32] = { 0 };
    a_sprintf(buff, "%d", nValue);
    return std::string(buff);
}

std::string CStringUtil::UInt16ToHexString(unsigned short nValue, bool bHas0X /*= true*/)
{
    char buff[32] = { 0 };
    if (bHas0X)
    {
        a_sprintf(buff, "0X%04X", nValue);
    }
    else
    {
        a_sprintf(buff, "%04X", nValue);
    }
    return std::string(buff);
}

unsigned char CStringUtil::CharToNum(unsigned char ch)
{
    if (ch >= '0' && ch <= '9')
    {
        return ch - '0';
    }
    else if (ch >= 'A' && ch <= 'F')
    {
        return ch - 'A' + 10;
    }
    else if (ch >= 'a' && ch <= 'f')
    {
        return ch - 'a' + 10;
    }
    CAquariusLogger::Logger(LL_ERROR_FUN, "Invalid input char value");
    return 0;
}

std::string& CStringUtil::TrimCharacter(std::string& strValue, char ch)
{
    if (!strValue.empty())
    {
        strValue.erase(0, strValue.find_first_not_of(ch));
        strValue.erase(strValue.find_last_not_of(ch) + 1);
    }
    return strValue;
}

std::string CStringUtil::EncodeStringToHex(const std::string& strInput)
{
    std::string strResult;
    size_t nInputLen = strInput.length();
    strResult.reserve(nInputLen * 2);
    for each (unsigned char ch in strInput)
    {
        strResult.append(CharToStr(ch));
    }
    if (strResult.length() != nInputLen * 2)
    {
        int nLen = (int)strResult.length();
        CAquariusLogger::Logger(LL_CRITICAL_FUN, "Impossible: strResult.length: [%d], input: [%s]", nLen, strInput.c_str());
        strResult.clear();
        return strResult;
    }
    return strResult;
}

std::string CStringUtil::DecodeHexToString(const std::string& strInput, bool bWarningEmptySrc)
{
    std::string strResult;
    if (strInput.empty())
    {
        if (bWarningEmptySrc)
        {
            CAquariusLogger::Logger(LL_WARNING_FUN, "Input string is empty");
        }
        return strResult;
    }
    size_t nInputLen = strInput.length();
    if (nInputLen % 2 != 0)
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "Input string length is invalid: [%s]", strInput.c_str());
        return strResult;
    }
    size_t nResultLen = nInputLen / 2;
    const char* pszInputCursor = strInput.c_str();
    for (unsigned int nIndex = 0; nIndex < nResultLen; ++nIndex)
    {
        unsigned char chLeft = CharToNum(pszInputCursor[0]);
        chLeft <<= 4;
        unsigned char chRight = CharToNum(pszInputCursor[1]);
        unsigned char chValue = (chLeft | chRight);
        strResult.append(1, chValue);
        pszInputCursor += 2;
    }
    return strResult;
}

unsigned char CStringUtil::UrlDecodeFromHex( unsigned char x )
{
    unsigned char y = '0';
    if (x >= 'A' && x <= 'Z')
    {
        y = x - 'A' + 10;
    }
    else if (x >= 'a' && x <= 'z')
    {
        y = x - 'a' + 10;
    }
    else if (x >= '0' && x <= '9')
    {
        y = x - '0';
    }
    else
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "Invalid Input char");
    }
    return y;
}

std::string CStringUtil::UrlEncode( const std::string& strInput )
{
    std::string strResult = "";
    for each (unsigned char ch in strInput)
    {
        if (isalnum(ch) 
            || (ch == '-')
            || (ch == '_')
            || (ch == '.')
            || (ch == '~'))
        {
            strResult += ch;
        }
        else if (ch == ' ')
        {
            strResult += "+";
        }
        else
        {
            strResult += '%';
            strResult += UrlEncodeToHex((unsigned char)ch >> 4);
            strResult += UrlEncodeToHex((unsigned char)ch % 16);
        }
    }
    return strResult;
}

std::string CStringUtil::UrlDecode( const std::string& strInput )
{
    std::string strResult = "";
    size_t length = strInput.length();
    for (size_t i = 0; i < length; ++i)
    {
        if (strInput[i] == '+')
        {
            strResult += ' ';
        }
        else if (strInput[i] == '%')
        {
            if (i + 2 < length)
            {
                unsigned char high = UrlDecodeFromHex((unsigned char)strInput[++i]);
                unsigned char low = UrlDecodeFromHex((unsigned char)strInput[++i]);
                strResult += high * 16 + low;
            }
            else
            {
                CAquariusLogger::Logger(LL_ERROR_FUN, "Invalid input: [%s]", strInput.c_str());
            }
        }
        else
        {
            strResult += strInput[i];
        }
    }
    return strResult;
}

std::string CStringUtil::GenerateRandString(AUINT32 nLength)
{
    if (nLength > 256)
    {
        nLength = 256;
        CAquariusLogger::Logger(LL_WARNING_FUN, "Too big Length: [%u]", nLength);
    }
    char buffResult[256] = { 0 };
    for (size_t nIndex = 0; nIndex < nLength; ++nIndex)
    {
        int nFlag = CRandUtil::RandNumber() % 3;
        switch (nFlag)
        {
        case 0:
            buffResult[nIndex] = 'A' + CRandUtil::RandNumber() % 26;
            break;
        case 1:
            buffResult[nIndex] = 'a' + CRandUtil::RandNumber() % 26;
            break;
        default:
            buffResult[nIndex] = '0' + CRandUtil::RandNumber() % 10;
            break;
        }
    }
    return std::string(buffResult);
}
