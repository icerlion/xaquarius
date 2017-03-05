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
 * FileName: LogType.hpp
 * 
 * Purpose:  Define common log type
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/2/22 16:07
===================================================================+*/
#ifndef _LOG_TYPE_HPP_
#define _LOG_TYPE_HPP_
#include "AquariusCore/Util/TypeReDef/TypeReDef.hpp"
#include <string>
#include <map>

// Define log severity levels
enum class ELogLevel : int
{
    Log_Level_Debug = 0x00000001,
    Log_Level_Notice = 0x00000002,
    Log_Level_Warning = 0x00000004,
    Log_Level_Error = 0x0000008,
    Log_Level_Critical = 0x00000010,
};

// Define log level for outside using
#define LL_DEBUG (ELogLevel::Log_Level_Debug), ("")
#define LL_DEBUG_FUN (ELogLevel::Log_Level_Debug), (__FUNCTION__)
#define LL_NOTICE (ELogLevel::Log_Level_Notice), ("")
#define LL_NOTICE_FUN (ELogLevel::Log_Level_Notice), (__FUNCTION__)
#define LL_WARNING (ELogLevel::Log_Level_Warning), ("")
#define LL_WARNING_FUN (ELogLevel::Log_Level_Warning), (__FUNCTION__)
#define LL_ERROR (ELogLevel::Log_Level_Error), ("")
#define LL_ERROR_FUN (ELogLevel::Log_Level_Error), (__FUNCTION__)
#define LL_CRITICAL (ELogLevel::Log_Level_Critical), ("")
#define LL_CRITICAL_FUN (ELogLevel::Log_Level_Critical), (__FUNCTION__)
// Simple log
#define JUNIOR_LOG(msg) printf("<<<Error>>>[%s]%s\n", __FUNCTION__, msg);

// Log level name
const static std::string LOG_LEVEL_NAME_DEBUG = "Debug";
const static std::string LOG_LEVEL_NAME_NOTICE = "Notice";
const static std::string LOG_LEVEL_NAME_WARNING = "Warning";
const static std::string LOG_LEVEL_NAME_ERROR = "Error";
const static std::string LOG_LEVEL_NAME_CRITICAL = "Critical";

// Get log level name
inline const char* LogLevelToCStr(ELogLevel nLevel)
{
    const char* pResult = "NA";
    switch (nLevel)
    {
    case ELogLevel::Log_Level_Debug:
        pResult = LOG_LEVEL_NAME_DEBUG.c_str();
        break;
    case ELogLevel::Log_Level_Notice:
        pResult = LOG_LEVEL_NAME_NOTICE.c_str();
        break;
    case ELogLevel::Log_Level_Warning:
        pResult = LOG_LEVEL_NAME_WARNING.c_str();
        break;
    case ELogLevel::Log_Level_Error:
        pResult = LOG_LEVEL_NAME_ERROR.c_str();
        break;
    case ELogLevel::Log_Level_Critical:
        pResult = LOG_LEVEL_NAME_CRITICAL.c_str();
        break;
    default:
        JUNIOR_LOG("Invalid log level enum");
    }
    return pResult;
}

// Get log level value by name
inline ELogLevel StringToLogLevel(const std::string& strLogLevelName)
{
    if (strLogLevelName == LOG_LEVEL_NAME_DEBUG) { return ELogLevel::Log_Level_Debug; }
    if (strLogLevelName == LOG_LEVEL_NAME_NOTICE) { return ELogLevel::Log_Level_Notice; }
    if (strLogLevelName == LOG_LEVEL_NAME_WARNING) { return ELogLevel::Log_Level_Warning; }
    if (strLogLevelName == LOG_LEVEL_NAME_ERROR) { return ELogLevel::Log_Level_Error; }
    if (strLogLevelName == LOG_LEVEL_NAME_CRITICAL) { return ELogLevel::Log_Level_Critical; }
    JUNIOR_LOG("Invalid log level name");
    return ELogLevel::Log_Level_Error;
}

// Define log msg
typedef struct LogMsg
{
    // Constructor
    LogMsg()
        :nLogLevel(ELogLevel::Log_Level_Debug),
        nThreadId(0)
    {
        memset(buffLogHeader, 0, sizeof(buffLogHeader));
        memset(buffLogContent, 0, sizeof(buffLogContent));
    }
    ELogLevel nLogLevel;
    unsigned int nThreadId;
    std::string strTimePoint;
    char buffLogHeader[256];
    char buffLogContent[5120];
    // Build formated log content
    inline void BuildLogHeader(const char* pszFunctionName)
    {
        // Format: [DateTime][ThreadId][LogLevel] - FunctionName: LogMsg
        if ((nullptr != pszFunctionName) && ('\0' != pszFunctionName[0]))
        {
            a_sprintf(buffLogHeader, "[%s][0x%08X][%s] - %s:", strTimePoint.c_str(), nThreadId, LogLevelToCStr(nLogLevel), pszFunctionName);
        }
        else
        {
            a_sprintf(buffLogHeader, "[%s][0x%08X][%s] - ", strTimePoint.c_str(), nThreadId, LogLevelToCStr(nLogLevel));
        }
    }
} LogMsg;

//////////////////////////////////////////////////////////////////////////
// Log visitor name list
const static std::string LOG_VISITOR_NAME_SCREEN = "CScreenLogVisitor";
const static std::string LOG_VISITOR_NAME_FILE = "CFileLogVisitor";

#endif // _LOG_TYPE_HPP_
