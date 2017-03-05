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
 * FileName: AquariusLogger.h
 * 
 * Purpose:  Entry of log util,
 *           The user should include this header file only!
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/2/22 16:12
===================================================================+*/
#ifndef _AQUARIUS_LOGGER_H_
#define _AQUARIUS_LOGGER_H_

#include "AquariusCore/Util/TypeReDef/TypeReDef.hpp"
#include "AquariusCore/Logger/LogType/LogType.hpp"
#include <vector>

struct LogVisitorConfigBase;
class ILogVisitor;
// Interface of Logger
class CAquariusLogger
{
public:
    /**
     * Description: Init log util by config. 
     *              This function should be called at the enter of main function. not-thread-safe
     * Parameter const std::string & strAppName     [in]: app name
     * Returns bool: true if success
     */
    static bool Initialize(const std::string& strAppName);

    /**
     * Description: Init log util by config. 
     *              This function should be called at the enter of main function. not-thread-safe
     * Parameter const char * pszConfigFilePath     [in]: config file path
     * Parameter const std::string & strAppName     [in]: app name
     * Returns bool: true if success
     */
    static bool Initialize(const char* pszConfigFilePath, const std::string& strAppName);

    /**
     * Description: Write a log, thread-safe
     * Parameter LogLevel nLevel                    [in]: log level
     * Parameter const char * pszFunctionName       [in]: function name
     * Parameter const char * pszMsgFormat          [in]: log msg format
     * Returns void:
     */
    static void Logger(ELogLevel nLevel, const char* pszFunctionName, const char* pszMsgFormat, ...);

private:
    // Get singleton instance
    inline static CAquariusLogger& GetSingleton()
    {
        static CAquariusLogger s_instance;
        return s_instance;
    }

    // Constructor
    CAquariusLogger(void);

    // Destructor
    ~CAquariusLogger(void);

    // Add a log visitor by config
    bool AddLogVisitor(const LogVisitorConfigBase* pLogVisitorConfig);
    
    // Create log visitor by name
    ILogVisitor* CreateLogVisitor(const std::string& strLogVisitorName) const;

    // Get the time point when log happened
    static void GetLogTime(std::string& strTimePoint);

private:
    // Visitor list
    std::vector<ILogVisitor*> m_vLogVisitor;
    // Min log level to process
    AUINT32 m_nFullLogLevelMask;
};

#endif // _AQUARIUS_LOGGER_H_
