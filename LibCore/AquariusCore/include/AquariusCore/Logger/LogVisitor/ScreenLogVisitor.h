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
 * FileName: ScreenLogVisitor.h
 * 
 * Purpose:  Visitor by screen, this class shoule be only one instance
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/2/22 16:07
===================================================================+*/
#ifndef _SCREENLOG_VISITOR_H_
#define _SCREENLOG_VISITOR_H_
#include "AquariusCore/Logger/LogVisitor/ILogVisitor.hpp"
#include "AquariusCore/Logger/LogConfig/LogConfig.h"

class CScreenLogVisitor : public ILogVisitor
{
public:
    // Init visitor by config
    virtual bool InitializeLogVisitor(const LogVisitorConfigBase* pLogVisitorConfig) override final;

    // Visit a log msg
    virtual void VisitLogMsg(const LogMsg& stLogMsg) override final;

    // Constructor
    CScreenLogVisitor()
        :ILogVisitor(LOG_VISITOR_NAME_SCREEN)
    {
    }

    // Destructor
    virtual ~CScreenLogVisitor() override final
    {
    }
};

#endif // _SCREENLOG_VISITOR_H_
