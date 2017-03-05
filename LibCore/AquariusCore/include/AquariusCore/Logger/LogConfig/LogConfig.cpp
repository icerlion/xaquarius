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
 * FileName: LogConfig.cpp
 * 
 * Purpose:  Implement log config
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/2/22 16:06
===================================================================+*/
#include "AquariusCore/Logger/LogConfig/LogConfig.h"
#include "AquariusCore/Util/TinyUtil/StringUtil.h"
#include "AquariusCore/Util/TypeReDef/TypeReDef.hpp"
#include "boost/property_tree/xml_parser.hpp"

bool CLogConfig::LoadConfigFile( const char* pszFilePath )
{
    // 1. Check pointer
    if (nullptr == pszFilePath)
    {
        BuildDefaultConfig();
        return true;
    }
    // 2. Load xml file
    bool bResult = true;
    try 
    {
        boost::property_tree::ptree ptFile;
        boost::property_tree::read_xml(pszFilePath, ptFile);
        auto pvLogConfig = ptFile.get_child("LogConfig");
        for each (auto ptItem in pvLogConfig)
        {
            boost::property_tree::ptree ptLogVisitor = ptItem.second;
            std::string strClassTypeName = ptLogVisitor.get<std::string>("<xmlattr>.ClassTypeName");
            const LogVisitorConfigBase* pLogVisitorConfig = nullptr;
            if (0 == strClassTypeName.compare(LOG_VISITOR_NAME_SCREEN))
            {
                pLogVisitorConfig = ReadScreenLogConfig(ptLogVisitor);
            }
            else if (0 == strClassTypeName.compare(LOG_VISITOR_NAME_FILE))
            {
                pLogVisitorConfig = ReadFileLogConfig(ptLogVisitor);
            }
            if (nullptr == pLogVisitorConfig)
            {
                JUNIOR_LOG("nullptr == pLogVisitorConfig");
                bResult = false;
                break;
            }
            m_vLogVisitorConfig.push_back(pLogVisitorConfig);
        }
    }
    catch (const boost::property_tree::ptree_error& ptError)
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "Failed to read xml as [%s]", ptError.what());
        bResult = false;
    }
    // Check visitor list
    if (m_vLogVisitorConfig.empty())
    {
        bResult = false;
    }
    // Check result
    return bResult;
}

AUINT32 CLogConfig::ParseLogLevelMask(const std::string& strMask) const
{
    std::vector<std::string> vLogLevel = CStringUtil::SplitStringToToken(strMask, '+');
    if (0 == strMask.length() || 0 == vLogLevel.size())
    {
        JUNIOR_LOG("Warning: No log mask");
        return 0;
    }
    AUINT32 nLogLevelMask = 0;
    for each (const std::string& strLogLevel in vLogLevel)
    {
        ELogLevel nLogLevel = StringToLogLevel(strLogLevel);
        nLogLevelMask |= (AUINT32)nLogLevel;
    }
    return nLogLevelMask;
}

const LogVisitorConfigBase* CLogConfig::ReadScreenLogConfig(const boost::property_tree::ptree& ptTree)
{
    ScreenLogConfig* pLogVisitorConfig = new ScreenLogConfig();
    std::string strLogLevelMask = ptTree.get<std::string>("<xmlattr>.LogLevelMask");
    pLogVisitorConfig->nLogLevelMask = ParseLogLevelMask(strLogLevelMask);
    if (!VerifyScreenLogConfig(pLogVisitorConfig))
    {
        delete pLogVisitorConfig;
        pLogVisitorConfig = nullptr;
    }
    return pLogVisitorConfig;
}

bool CLogConfig::VerifyScreenLogConfig(const ScreenLogConfig* pScreenLogConfig)
{
    if (nullptr == pScreenLogConfig)
    {
        JUNIOR_LOG("nullptr == pScreenLogConfig");
        return false;
    }
    // Verify log visitor config
    bool bResult = true;
    for each (const LogVisitorConfigBase* pConfig in m_vLogVisitorConfig)
    {
        if (nullptr == pConfig)
        {
            bResult = false;
            JUNIOR_LOG("nullptr == pConfig");
            break;
        }
        if (pConfig->strLogVisitorName == pScreenLogConfig->strLogVisitorName)
        {
            bResult = false;
            JUNIOR_LOG("More than one screen log vistor");
            break;
        }
    }
    return bResult;
}

const LogVisitorConfigBase* CLogConfig::ReadFileLogConfig(const boost::property_tree::ptree& ptTree)
{
    FileLogConfig* pLogVisitorConfig = new FileLogConfig();
    std::string strLogLevel = ptTree.get<std::string>("<xmlattr>.LogLevelMask");
    pLogVisitorConfig->nLogLevelMask = ParseLogLevelMask(strLogLevel);
    pLogVisitorConfig->strFileNamePrefix = ptTree.get<std::string>("<xmlattr>.NamePerfix");
    m_strLogFileDir = ptTree.get<std::string>("<xmlattr>.LogDir");
    pLogVisitorConfig->strLogFileDir = m_strLogFileDir;
    pLogVisitorConfig->nMaxFileSize = (size_t)(ptTree.get<int>("<xmlattr>.MaxFileSize") * 1024 * 1024);
    if (!VerifyFileLogConfig(pLogVisitorConfig))
    {
        delete pLogVisitorConfig;
        pLogVisitorConfig = nullptr;
    }
    return pLogVisitorConfig;
}

bool CLogConfig::VerifyFileLogConfig(const FileLogConfig* pFileLogConfig)
{
    if (nullptr == pFileLogConfig)
    {
        JUNIOR_LOG("nullptr == pFileLogConfig");
        return false;
    }
    // Verify log visitor config by existed log config
    bool bResult = true;
    for each (const LogVisitorConfigBase* pConfig in m_vLogVisitorConfig)
    {
        if (nullptr == pConfig)
        {
            bResult = false;
            JUNIOR_LOG("nullptr == pConfig");
            break;
        }
        // Different name, just continue
        if (pConfig->strLogVisitorName != pFileLogConfig->strLogVisitorName)
        {
            continue;
        }
        // Convert pointer
        const FileLogConfig* pExistedFileLogConfig = dynamic_cast<const FileLogConfig*>(pConfig);
        if (nullptr == pExistedFileLogConfig)
        {
            bResult = false;
            JUNIOR_LOG("nullptr == pExistedFileLogConfig");
            break;
        }
        // Can't has the same prefix and dir
        if (0 == pExistedFileLogConfig->strFileNamePrefix.compare(pFileLogConfig->strFileNamePrefix)
            && 0 == pExistedFileLogConfig->strLogFileDir.compare(pFileLogConfig->strLogFileDir))
        {
            bResult = false;
            JUNIOR_LOG("Compare failed");
            break;
        }
    }
    return bResult;
}

CLogConfig::CLogConfig()
{

}

CLogConfig::~CLogConfig()
{
    for each (const LogVisitorConfigBase* pLogVisitorConfigBase in m_vLogVisitorConfig)
    {
        delete pLogVisitorConfigBase;
        pLogVisitorConfigBase = nullptr;
    }
    m_vLogVisitorConfig.clear();
}

void CLogConfig::BuildDefaultConfig()
{
    // 1. Add screen log
    ScreenLogConfig* pScreenLog = new ScreenLogConfig();
    pScreenLog->nLogLevelMask = (AUINT32)ELogLevel::Log_Level_Critical | (AUINT32)ELogLevel::Log_Level_Error | (AUINT32)ELogLevel::Log_Level_Warning| (AUINT32)ELogLevel::Log_Level_Notice;
    m_vLogVisitorConfig.push_back(pScreenLog);
    // 2. Add file log
    FileLogConfig* pFileLog = new FileLogConfig();
    pFileLog->nLogLevelMask = 0xFFFFFFFF;
    pFileLog->strFileNamePrefix = "Common";
    pFileLog->strLogFileDir = "./log/";
    pFileLog->nMaxFileSize = 10 * 1024 * 1024;
    m_vLogVisitorConfig.push_back(pFileLog);
}

void CLogConfig::SetAppName( const std::string& strAppName )
{
    for each (const LogVisitorConfigBase* pBaseConfig in m_vLogVisitorConfig)
    {
        LogVisitorConfigBase* pLogConfig = const_cast<LogVisitorConfigBase*>(pBaseConfig);
        if (nullptr == pLogConfig)
        {
            JUNIOR_LOG("nullptr == pLogConfig");
            continue;
        }
        if (LOG_VISITOR_NAME_FILE != pLogConfig->strLogVisitorName)
        {
            continue;
        }
        FileLogConfig* pFileLogConfig = dynamic_cast<FileLogConfig*>(pLogConfig);
        if (nullptr == pFileLogConfig)
        {
            JUNIOR_LOG("nullptr == pFileLogConfig");
            continue;
        }
        pFileLogConfig->strFileNamePrefix = strAppName + "-" + pFileLogConfig->strFileNamePrefix;
    }
}

