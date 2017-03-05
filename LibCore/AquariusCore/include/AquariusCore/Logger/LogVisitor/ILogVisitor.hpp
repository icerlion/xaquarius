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
 * FileName: ILogVisitor.hpp
 * 
 * Purpose:  Interface of log visitor
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/2/22 16:07
===================================================================+*/
#ifndef _ILOG_VISITOR_HPP_
#define _ILOG_VISITOR_HPP_
#include "AquariusCore/Logger/LogType/LogType.hpp"
#include "boost/thread/mutex.hpp"
#include "AquariusCore/Logger/LogConfig/LogConfig.h"

class ILogVisitor
{
public:
    // Destructor
    virtual ~ILogVisitor()
    {
    }

    // Initialize log visitor by config
    virtual bool InitializeLogVisitor(const LogVisitorConfigBase* pLogVisitorConfig) = 0;

    // Record a log msg
    inline void RecordLog(const LogMsg& stLogMsg)
    {
        if (0 != (((AUINT32)stLogMsg.nLogLevel) & m_nLogLevelMask)) 
        {
            // Multi thread protected
            boost::unique_lock<boost::mutex> scoped_lock(m_mutex_log);
            VisitLogMsg(stLogMsg);
        }
    }

protected:
    // Constructor
    ILogVisitor(const std::string& strLogVisitorName)
        :m_nLogLevelMask(0),
        m_strLogVisitorName(strLogVisitorName)
    {
    }

    // Visit a log msg
    virtual void VisitLogMsg(const LogMsg& stLogMsg) = 0;

protected:
    // Filter log level
    AUINT32 m_nLogLevelMask;
    // Visitor name
    const std::string m_strLogVisitorName;
    // log mutex
    boost::mutex m_mutex_log;
};

#endif // _ILOG_VISITOR_HPP_
