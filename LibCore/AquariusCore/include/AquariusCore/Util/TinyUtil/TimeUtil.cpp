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
 * FileName: TimeUtil.cpp
 * 
 * Purpose:  Implement TimeUtil function
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/2/22 15:43
===================================================================+*/
#include "AquariusCore/Util/TinyUtil/TimeUtil.h"
#include "AquariusCore/Logger/AquariusLogger.h"
#include "AquariusCore/Util/TinyUtil/StringUtil.h"

time_t CTimeUtil::StringToUTCTime( std::string strUTCTime )
{
    // Input format: yyyy-mm-dd hh:mm:ss or yyyy-mm-ddThh:mm:ss or yyyymmddhhmmss
    if (14 == strUTCTime.length())
    {
        // Format: yyyymmddhhmmss
        std::string strYear = strUTCTime.substr(0, 4);
        std::string strMonth = strUTCTime.substr(4, 2);
        std::string strDay = strUTCTime.substr(6, 2);
        std::string strHour = strUTCTime.substr(8, 2);
        std::string strMinutes = strUTCTime.substr(10, 2);
        std::string strSeconds = strUTCTime.substr(12, 2);
        strUTCTime.clear();
        strUTCTime.append(strYear).append("-").append(strMonth).append("-").append(strDay).append("T");
        strUTCTime.append(strHour).append(":").append(strMinutes).append(":").append(strSeconds);
    }
    size_t nKeyPos = strUTCTime.find(' ');
    if (std::string::npos == nKeyPos)
    {
        nKeyPos = strUTCTime.find('T');
    }
    if (std::string::npos == nKeyPos)
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "Invalid time format: [%s]", strUTCTime.c_str());
        return 0;
    }
    // Split date and time
    const std::string& strDate = strUTCTime.substr(0, nKeyPos);
    // Parse date token
    std::vector<int> vDateField = CStringUtil::SplitStringToIntVector(strDate, '-');
    if (vDateField.size() != 3)
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "Invalid time format: [%s]", strUTCTime.c_str());
        return 0;
    }
    const std::string& strTime = strUTCTime.substr(nKeyPos + 1, strUTCTime.length());
    std::vector<int> vTimeField = CStringUtil::SplitStringToIntVector(strTime, ':');
    if (vTimeField.size() != 3)
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "Invalid time format: [%s]", strUTCTime.c_str());
        return 0;
    }
    struct tm stTM;
    memset(&stTM, 0, sizeof(stTM));
    stTM.tm_year = vDateField[0] - 1900;
    stTM.tm_mon = vDateField[1] - 1;
    stTM.tm_mday = vDateField[2];
    stTM.tm_hour = vTimeField[0];
    stTM.tm_min = vTimeField[1];
    stTM.tm_sec = vTimeField[2];
    return mktime(&stTM);
}

std::string CTimeUtil::UTCTimeToReadableString( time_t nTimePoint)
{
    struct tm stTM = UTCTimeToLocalTime(nTimePoint);
    const char* TIME_FORMAT = "%04d-%02d-%02d %02d:%02d:%02d";
    char buff[32] = { 0 };
    a_sprintf(buff, TIME_FORMAT,
        stTM.tm_year + 1900,
        stTM.tm_mon + 1,
        stTM.tm_mday,
        stTM.tm_hour,
        stTM.tm_min,
        stTM.tm_sec);
    return std::string(buff);
}

struct tm CTimeUtil::UTCTimeToLocalTime( time_t nTime )
{
    struct tm stResult;
#ifdef LINUX
    struct tm* pTM = localtime(&nTime);
    if (nullptr == pTM)
    {
        CAquariusLogger::Logger(LL_CRITICAL_FUN, "nullptr == pTM");
    }
    else
    {
        stResult = *pTM;
    }
#else
    localtime_s(&stResult, &nTime);
#endif
    return stResult;
}
