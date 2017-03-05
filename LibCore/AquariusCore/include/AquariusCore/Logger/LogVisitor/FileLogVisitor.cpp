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
 * FileName: FileLogVisitor.cpp
 * 
 * Purpose:  Implement file log visitor
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/2/22 16:11
===================================================================+*/
#include "AquariusCore/Logger/LogVisitor/FileLogVisitor.h"
#include "AquariusCore/Util/TinyUtil/FileSystemUtil.hpp"
#include "AquariusCore/Util/TinyUtil/StringUtil.h"
#include "AquariusCore/Util/TinyUtil/TimeUtil.h"
#include "AquariusCore/Util/TypeReDef/TypeReDef.hpp"

static const std::string CONST_LOG_FILE_EXT_NAME = ".log";

bool CFileLogVisitor::InitializeLogVisitor(const LogVisitorConfigBase* pLogVisitorConfig)
{
    const FileLogConfig* pFileLogConfig = dynamic_cast<const FileLogConfig*>(pLogVisitorConfig);
    if (nullptr == pFileLogConfig)
    {
        JUNIOR_LOG("nullptr == pFileLogConfig");
        return false;
    }
    m_nLogLevelMask = pFileLogConfig->nLogLevelMask;
    m_strFileNamePrefix = pFileLogConfig->strFileNamePrefix;
    m_strLogFileDir = pFileLogConfig->strLogFileDir;
    m_nMaxFileSize = pFileLogConfig->nMaxFileSize;
    if (m_nMaxFileSize < 1024 * 1024)
    {
        JUNIOR_LOG("Max log file size is invalid");
        return false;
    }
    if (!CFileSystemUtil::TryCreatePath(m_strLogFileDir))
    {
        JUNIOR_LOG("TryCreatePath failed");
        return false;
    }
    if (!CheckLogFileStream())
    {
        JUNIOR_LOG("CheckLogFileStream failed!");
        return false;
    }
    if (!m_ofsLog.is_open())
    {
        JUNIOR_LOG("File stream is not open");
        return  false;
    }
    return true;
}

void CFileLogVisitor::VisitLogMsg( const LogMsg& stLogMsg )
{
    // 1. Check log file stream by the current time point
    if (!CheckLogFileStream())
    {
        JUNIOR_LOG("CheckLogFileStream failed!");
        return;
    }
    // 2. Check file stream status
    if (!m_ofsLog.is_open())
    {
        JUNIOR_LOG("FileStream is not open");
        return;
    }
    // 3. Write log to stream
    size_t nLogHeaderLen = strlen(stLogMsg.buffLogHeader);
    size_t nLogContentLen = strlen(stLogMsg.buffLogContent);
    m_ofsLog.write(stLogMsg.buffLogHeader, nLogHeaderLen);
    m_ofsLog.write(stLogMsg.buffLogContent, nLogContentLen);
    m_ofsLog.write("\n", 1);
    m_nCurrentFileSize += nLogHeaderLen + nLogContentLen + 1;
    // 4. Flush log after 5 seconds
    time_t nCurTime = time(nullptr);
    if (nCurTime > 5 + m_nFlushTime )
    {
        m_ofsLog.flush();
        m_nFlushTime = nCurTime;
    }
    return;
}

bool CFileLogVisitor::OpenLogFileStream()
{
    // 1. Close old file stream
    m_ofsLog.flush();
    m_ofsLog.close();
    m_nCurrentFileSize = 0;
    // 2. Build base file name
    m_stPreviousLogFileCreateTM = CTimeUtil::UTCTimeToLocalTime(time(nullptr));
    const char* TIME_FORMAT = "-%04d%02d%02d-%02d%02d%02d";
    char buff[32] = { 0 };
    a_sprintf(buff, TIME_FORMAT,
        m_stPreviousLogFileCreateTM.tm_year + 1900,
        m_stPreviousLogFileCreateTM.tm_mon + 1,
        m_stPreviousLogFileCreateTM.tm_mday,
        m_stPreviousLogFileCreateTM.tm_hour,
        m_stPreviousLogFileCreateTM.tm_min,
        m_stPreviousLogFileCreateTM.tm_sec);
    // 3. Check existed file name
    std::string strLogFileFullPath = m_strLogFileDir;
    strLogFileFullPath.append("/").append(m_strFileNamePrefix).append(buff).append(m_strLogStartRotation).append(CONST_LOG_FILE_EXT_NAME);
    m_strLogStartRotation = "-r";
    m_ofsLog.open(strLogFileFullPath, std::fstream::trunc);
    return m_ofsLog.is_open();
}

bool CFileLogVisitor::IsLogFileSizeGood()
{
    return m_nCurrentFileSize < m_nMaxFileSize;
}

bool CFileLogVisitor::CheckLogFileStream( )
{
    // `1. Get the log file date string by the current date point
    struct tm stCurrentLogFileCreateTM = CTimeUtil::UTCTimeToLocalTime(time(nullptr));
    if (( stCurrentLogFileCreateTM.tm_yday == m_stPreviousLogFileCreateTM.tm_yday && stCurrentLogFileCreateTM.tm_year == m_stPreviousLogFileCreateTM.tm_year)
        && IsLogFileSizeGood())
    {
        return true;
    }
    // 2. Open log file stream
    if (!OpenLogFileStream())
    {
        JUNIOR_LOG("OpenLogFileStream failed");
        return false;
    }
    m_nCurrentFileSize = 0;
    return true;
}

CFileLogVisitor::CFileLogVisitor() 
    :ILogVisitor(LOG_VISITOR_NAME_FILE),
    m_nFlushTime(0),
    m_nCurrentFileSize(0),
    m_nMaxFileSize(1024 * 1024 * 1024),
    m_strLogStartRotation("-s")
{
    memset(&m_stPreviousLogFileCreateTM, 0, sizeof(m_stPreviousLogFileCreateTM));
}
