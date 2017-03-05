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
 * FileName: TaskController.h
 * 
 * Purpose:  Define TaskController
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/2/22 16:03
===================================================================+*/
#ifndef _TASK_CONTROLLER_H_
#define _TASK_CONTROLLER_H_
#include "AquariusCore/Engine/Task/Base/ITask.hpp"
#include "AquariusCore/Engine/Task/Base/TaskSwap.h"
#include "boost/thread.hpp"

class CTaskController
{
protected:
    // Constructor
    CTaskController(const char* pszControllerName);

    // Destructor
    virtual ~CTaskController();

public:
    // Init task controller
    bool Initialize(unsigned int nTaskThreadCount, void* pInitParam);

    // Destroy task controller
    void Destroy();

    // Main thread will call this function
    bool InputTaskRequest(ITask* pTaskReq);

    // Get req task count
    inline AUINT32 GetReqTaskCount()
    {
        return m_hTaskSwap.GetReqTaskCount();
    }

    // Get res task count
    inline AUINT32 GetResTaskCount()
    {
        return m_hTaskSwap.GetResTaskCount();
    }

private:
    // Main thread will call this function
    void TimerOutputTaskResponse();

    // Task work thread
    void TaskLoopThread();

    // Create task parameter for different task controller
    virtual ITaskParam* CreateTaskParam() const = 0;

    // On init controller
    virtual bool OnInit(void* pInitParam) = 0;

    // On destroy controller
    virtual void OnDestroy() = 0;

protected:
    // Task thread group
    boost::thread_group m_hTaskThreadGroup;
    // Flag of start task thread
    bool m_bStartTaskThread;
    // Task swap manager of this controller
    CTaskSwap m_hTaskSwap;
    // Task Controller Name
    std::string m_strControllerName;
};

#endif // _TASK_CONTROLLER_H_
