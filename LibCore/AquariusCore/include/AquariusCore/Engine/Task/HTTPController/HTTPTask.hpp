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
 * FileName: HTTPTask.hpp
 * 
 * Purpose:  Define HTTPTask, support HTTP request/response
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/2/22 16:17
===================================================================+*/
#ifndef _HTTP_TASK_HPP_
#define _HTTP_TASK_HPP_

#include "AquariusCore/Engine/Task/Base/ITask.hpp"
#include "AquariusCore/Util/HTTPHelper/HTTPHelper.h"

// Base class of HTTP task.
class CHTTPTask : public ITask
{
public:
    // Constructor
    CHTTPTask(void)
    {
    }

    // Destructor
    virtual ~CHTTPTask(void) override
    {
    }

    // Process http Task Request
    virtual void ProcTaskRequest(ITaskParam* pParam) override final
    {
        if (nullptr != pParam)
        {
            CAquariusLogger::Logger(LL_ERROR_FUN, "nullptr != pParam, pParam should be nullptr");
            return;
        }
        ProcHTTPTaskRequest();
    }

    // Process http Task response
    virtual void ProcTaskResponse() override
    {
        ProcHTTPTaskResponse();
    }

    // The real http task should implement ProcHTTPTaskRequest
    virtual void ProcHTTPTaskRequest() = 0;

    // The real http task should implement ProcHTTPTaskResponse
    virtual void ProcHTTPTaskResponse() = 0;

protected:
    // URL string
    std::string m_strURL;
    // HTTP connection
    CHTTPHelper m_hHTTPDownloader;
};

#endif // _HTTP_TASK_HPP_
