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
 * FileName: HTTPTaskController.hpp
 * 
 * Purpose:  Implement HTTPTaskController
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/2/22 16:16
===================================================================+*/
#ifndef _HTTP_TASK_CONTROLLER_HPP_
#define _HTTP_TASK_CONTROLLER_HPP_

#include "curl/curl.h"
#include "AquariusCore/Engine/Task/Base/TaskController.h"
#include "AquariusCore/Util/Singleton/ISingleton.hpp"
#include "AquariusCore/Engine/Task/HTTPController/HTTPTask.hpp"

class CHTTPTaskController : public CTaskController, public ISingleton<CHTTPTaskController>
{
public:
    // Constructor
    CHTTPTaskController(void)
        :CTaskController("HTTPTaskController")
    {
    }

    // Destructor
    virtual ~CHTTPTaskController(void) override final
    {
    }

    // Create task parameter
    virtual ITaskParam* CreateTaskParam() const override final
    {
        return nullptr;
    }

    // On init controller
    virtual bool OnInit(void* /*pInitParam*/) override final
    {
        // You should call curl_global_init(CURL_GLOBAL_ALL); before this function
        return true;
    }

    // On destroy controller
    virtual void OnDestroy() override final
    {
        // You should call curl_global_cleanup(); before this function
        return;
    }

    // Input HTTP task
    static inline bool InputHTTPTask(CHTTPTask* pHTTPTask)
    {
        if (nullptr == pHTTPTask)
        {
            CAquariusLogger::Logger(LL_ERROR_FUN, "nullptr == pHTTPTask");
            return false;
        }
        return GetSingleton().InputTaskRequest(pHTTPTask);
    }
};

#endif // _HTTP_TASK_CONTROLLER_HPP_
