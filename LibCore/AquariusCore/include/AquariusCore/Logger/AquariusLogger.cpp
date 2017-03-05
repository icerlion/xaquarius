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
 * FileName: AquariusLogger.cpp
 * 
 * Purpose:  Implement CAquariusLogger
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/2/22 16:12
===================================================================+*/
#include "AquariusCore/Util/TypeReDef/TypeReDef.hpp"
#include "AquariusCore/Util/TinyUtil/CommonUtil.h"
#include "AquariusCore/Util/TinyUtil/TimeUtil.h"
#include <stdarg.h>
#include <sys/timeb.h>
#include <time.h>
#include "boost/date_time.hpp"
#include "AquariusCore/Logger/AquariusLogger.h"
#include "AquariusCore/Logger/LogVisitor/FileLogVisitor.h"
#include "AquariusCore/Logger/LogVisitor/ScreenLogVisitor.h"
#include "AquariusCore/Logger/LogVisitor/ILogVisitor.hpp"

CAquariusLogger::CAquariusLogger(void)
    :m_nFullLogLevelMask(0)
{
}

CAquariusLogger::~CAquariusLogger(void)
{
    // Free every log visitor
    for each (ILogVisitor* pValue in m_vLogVisitor)
    {
        delete pValue;
        pValue = nullptr;
    }
    m_vLogVisitor.clear();
}

void CAquariusLogger::GetLogTime(std::string& strTimePoint)
{
    // Get time buffer
#ifdef LINUX
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    time_t nUTCTime = tv.tv_sec;
    int nMS = tv.tv_usec / 1000;
#else
    struct _timeb stTimeBuffer;
    _ftime_s( &stTimeBuffer );
    time_t nUTCTime = stTimeBuffer.time;
    int nMS = stTimeBuffer.millitm;
#endif
    // Convert to local time
    struct tm stTM = CTimeUtil::UTCTimeToLocalTime(nUTCTime);
    // Format to string
    const char* TIME_FORMAT = "%04d-%02d-%02d %02d:%02d:%02d.%d";
    char buff[64] = { 0 };
    a_sprintf(buff, TIME_FORMAT,
        stTM.tm_year + 1900,
        stTM.tm_mon + 1,
        stTM.tm_mday,
        stTM.tm_hour,
        stTM.tm_min,
        stTM.tm_sec,
        nMS);
    strTimePoint.append(buff);
}

void CAquariusLogger::Logger( ELogLevel nLevel, const char* pszFunctionName, const char* pszMsgFormat, ... )
{
    // 1. Check log level
    std::vector<ILogVisitor*>& vLogVisitor = GetSingleton().m_vLogVisitor;
    if (0 == ((AUINT32)nLevel & GetSingleton().m_nFullLogLevelMask) && 0 != vLogVisitor.size())
    {
        return;
    }
    // 2. Build a log msg
    LogMsg stLogMsg;
    stLogMsg.nLogLevel = nLevel;
    GetLogTime(stLogMsg.strTimePoint);
    stLogMsg.nThreadId = CCommonUtil::GetThisThreadId();
    stLogMsg.BuildLogHeader(pszFunctionName);
    // 3. parse var parameter
    va_list vaList;
    va_start(vaList, pszMsgFormat);
#ifdef WIN32
    vsprintf_s(stLogMsg.buffLogContent, pszMsgFormat, vaList);
#else
    vsprintf(stLogMsg.buffLogContent, pszMsgFormat, vaList);
#endif
    va_end(vaList);
    // 4. Get log visitor list and loop every visitor to record log
    for each (ILogVisitor* pLogVisitor in vLogVisitor)
    {
        if (nullptr == pLogVisitor)
        {
            JUNIOR_LOG("nullptr == pLogVisitor!");
            continue;
        }
        pLogVisitor->RecordLog(stLogMsg);
    }
    if (0 == vLogVisitor.size())
    {
        printf("[UnInitLog]%s%s\n", stLogMsg.buffLogHeader, stLogMsg.buffLogContent);
    }
}

bool CAquariusLogger::Initialize(const char* pszConfigFilePath, const std::string& strAppName)
{
    if (!CLogConfig::GetSingleton().LoadConfigFile(pszConfigFilePath))
    {
        JUNIOR_LOG("CLogConfig::LoadConfigFile failed!");
        return false;
    }
    CLogConfig::GetSingleton().SetAppName(strAppName);
    bool bResult = true;
    // Get config list
    const std::vector<const LogVisitorConfigBase*>& vLogVisitorConfig = CLogConfig::GetSingleton().GetLogVisitorConfigList();
    for each (const LogVisitorConfigBase* pValue in vLogVisitorConfig)
    {
        // Add log visitor by config
        if (!GetSingleton().AddLogVisitor(pValue))
        {
            JUNIOR_LOG("AddLogVisitor Failed");
            bResult = false;
            break;
        }
    }
    if (bResult)
    {
        CAquariusLogger::Logger(LL_DEBUG, "===LoggerInitialized===");
    }
    return bResult;
}

bool CAquariusLogger::Initialize( const std::string& strAppName )
{
    return Initialize(nullptr, strAppName);
}

ILogVisitor* CAquariusLogger::CreateLogVisitor(const std::string& strLogVisitorName) const
{
    // Create log visitor by name
    ILogVisitor* pLogVisitor = nullptr;
    if (0 == strLogVisitorName.compare(LOG_VISITOR_NAME_SCREEN))
    {
        pLogVisitor = new CScreenLogVisitor();
    }
    else if (0 == strLogVisitorName.compare(LOG_VISITOR_NAME_FILE))
    {
        pLogVisitor = new CFileLogVisitor();
    }
    else
    {
        JUNIOR_LOG("Invalid visitor name");
    }
    return pLogVisitor;
}

bool CAquariusLogger::AddLogVisitor(const LogVisitorConfigBase* pLogVisitorConfig)
{
    // 1. Check config content
    if (nullptr == pLogVisitorConfig)
    {
        JUNIOR_LOG("nullptr == pLogVisitorConfig");
        return false;
    }
    // 2. Create visitor by class type
    ILogVisitor* pLogVisitor = CreateLogVisitor(pLogVisitorConfig->strLogVisitorName);
    if (nullptr == pLogVisitor)
    {
        JUNIOR_LOG("nullptr == pLogVisitor");
        return false;
    }
    // 3. Init log visitor
    if (!pLogVisitor->InitializeLogVisitor(pLogVisitorConfig))
    {
        JUNIOR_LOG("LogVisitor->Initialize failed");
        delete pLogVisitor;
        pLogVisitor = nullptr;
        return false;
    }
    m_vLogVisitor.push_back(pLogVisitor);
    m_nFullLogLevelMask |= pLogVisitorConfig->nLogLevelMask;
    return true;
}
