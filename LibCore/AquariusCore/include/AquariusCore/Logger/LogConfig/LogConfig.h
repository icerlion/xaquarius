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
 * FileName: LogConfig.h
 * 
 * Purpose:  Define log config type
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/2/22 16:06
===================================================================+*/
#ifndef _LOG_CONFIG_H_
#define _LOG_CONFIG_H_
#include "AquariusCore/Logger/LogType/LogType.hpp"
#include "AquariusCore/Util/Singleton/ISingleton.hpp"
#include "AquariusCore/Util/TypeReDef/TypeReDef.hpp"
#include "boost/property_tree/ptree.hpp"
#include <vector>
#include <string>

//////////////////////////////////////////////////////////////////////////
// Basic log visitor config
typedef struct LogVisitorConfigBase 
{
    // Constructor
    LogVisitorConfigBase(const std::string& strName)
        :nLogLevelMask(0),
        strLogVisitorName(strName)
    {
    }
    // Destructor
    virtual ~LogVisitorConfigBase()
    {
    }
    // Filter level
    AUINT32 nLogLevelMask;
    // Visitor name
    std::string strLogVisitorName;
} LogVisitorConfigBase;

//////////////////////////////////////////////////////////////////////////
// Screen Log config
typedef struct ScreenLogConfig : public LogVisitorConfigBase
{
    // Constructor
    ScreenLogConfig()
        :LogVisitorConfigBase(LOG_VISITOR_NAME_SCREEN)
    {
    }
    // Destructor
    virtual ~ScreenLogConfig() override final
    {
    }
} ScreenLogConfig;

//////////////////////////////////////////////////////////////////////////
// File log config
typedef struct FileLogConfig : public LogVisitorConfigBase
{
    // Constructor
    FileLogConfig()
        :LogVisitorConfigBase(LOG_VISITOR_NAME_FILE),
        nMaxFileSize(0)
    {
    }
    // Destructor
    virtual ~FileLogConfig() override final
    {
    }
    // Prefix string of file name
    std::string strFileNamePrefix;
    // Log file dir
    std::string strLogFileDir;
    // Max file size
    size_t nMaxFileSize;
} FileLogConfig;

//////////////////////////////////////////////////////////////////////////
class CLogConfig : public ISingleton<CLogConfig>
{
public:
    // Constructor
    CLogConfig();

    // Destructor
    virtual ~CLogConfig() override final;

    // Load config file
    bool LoadConfigFile(const char* pszFilePath);

    // Set app name
    void SetAppName(const std::string& strAppName);

    // Get visitor config list
    inline const std::vector<const LogVisitorConfigBase*>& GetLogVisitorConfigList() const
    {
        return m_vLogVisitorConfig;
    }

protected:
    // Build default log config
    void BuildDefaultConfig();

    // Read ScreenLogConfig
    const LogVisitorConfigBase* ReadScreenLogConfig(const boost::property_tree::ptree& ptTree);

    // Verify screen log config
    bool VerifyScreenLogConfig(const ScreenLogConfig* pScreenLogConfig);

    // Read file log config
    const LogVisitorConfigBase* ReadFileLogConfig(const boost::property_tree::ptree& ptTree);

    // Verify file log config
    bool VerifyFileLogConfig(const FileLogConfig* pFileLogConfig);

    // Convert string to log level mask
    AUINT32 ParseLogLevelMask(const std::string& strMask) const;

private:
    // Visitor config list
    std::vector<const LogVisitorConfigBase*> m_vLogVisitorConfig;
    // log file dir
    std::string m_strLogFileDir;
};

#endif // _LOG_CONFIG_H_
