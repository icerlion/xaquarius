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
 * FileName: FileLogVisitor.h
 * 
 * Purpose:  Write log into file stream
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/2/22 16:08
===================================================================+*/
#ifndef _FILELOG_VISITOR_H_
#define _FILELOG_VISITOR_H_
#include "AquariusCore/Logger/LogVisitor/ILogVisitor.hpp"
#include "AquariusCore/Logger/LogConfig/LogConfig.h"
#include <fstream>

class CFileLogVisitor : public ILogVisitor
{
public:
    // Init log visitor
    virtual bool InitializeLogVisitor(const LogVisitorConfigBase* pLogVisitorConfig) override final;

    // Visit a log msg
    virtual void VisitLogMsg(const LogMsg& stLogMsg) override final;

    // Constructor
    CFileLogVisitor();

    // Destructor
    virtual ~CFileLogVisitor() override final
    {
    }

private:
    // Check log file stream by date point
    bool CheckLogFileStream();

    // Open log file stream by date point
    bool OpenLogFileStream();

    // Return true if file size is small enough
    bool IsLogFileSizeGood();

private:
    // File name prefix
    std::string m_strFileNamePrefix;
    // Log file directory
    std::string m_strLogFileDir;
    // Log file date string
    struct tm m_stPreviousLogFileCreateTM;
    // stream of log file
    std::ofstream m_ofsLog;
    // Time point of flush file 
    time_t m_nFlushTime;
    // File size of the current log file
    size_t m_nCurrentFileSize;
    //  Config of max log file size
    size_t m_nMaxFileSize;
    // "s": create by start server
    // "r": create by rotation
    std::string m_strLogStartRotation;
};

#endif // _FILELOG_VISITOR_H_
