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
 * FileName: TaskController.cpp
 * 
 * Purpose:  Implement Task controller
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/2/22 16:04
===================================================================+*/
#include "AquariusCore/Engine/Task/Base/TaskController.h"
#include "AquariusCore/Engine/Task/Base/TaskSwap.h"
#include "AquariusCore/Engine/Timer/TimerManager.h"
#include "AquariusCore/Logger/AquariusLogger.h"
#include "AquariusCore/Util/TinyUtil/CommonUtil.h"

void CTaskController::TimerOutputTaskResponse()
{
    // Output Task response
    ITask* pTaskRes = nullptr;
    // Try to pull a task response
    m_hTaskSwap.PullResponse(pTaskRes);
    if (nullptr != pTaskRes)
    {
        // Process task response
        pTaskRes->ProcTaskResponse();
        // The task was useless now, just release it
        pTaskRes->ReleaseSelf();
    }
}

bool CTaskController::InputTaskRequest( ITask* pTaskReq )
{
    // Check parameter
    if (nullptr == pTaskReq)
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "nullptr == pTaskReq: [%s]", m_strControllerName.c_str());
        return false;
    }
    // Push a task task
    if (!m_hTaskSwap.PushRequest(pTaskReq))
    {
        pTaskReq->ReleaseSelf();
        CAquariusLogger::Logger(LL_ERROR_FUN, "PushRequest failed, release it: [%s]", m_strControllerName.c_str());
        return false;
    }
    return true;
}

void CTaskController::TaskLoopThread()
{
    ITaskParam* pTaskParam = CreateTaskParam();
    // Get task swap
    AUINT32 nProcessReqCount = 0;
    time_t nStatTime = time(nullptr);
    while (m_bStartTaskThread)
    {
        ITask* pTaskReq = nullptr;
        // Try to pull a task request
        m_hTaskSwap.PullRequest(pTaskReq);
        if (nullptr != pTaskReq)
        {
            // process task request
            pTaskReq->ProcTaskRequest(pTaskParam);
            // Add task response
            if (!m_hTaskSwap.PushResponse(pTaskReq))
            {
                // Don't release the Task, main thread and work thread maybe access the memory in the same time.
                // Leak is better then conflict
                CAquariusLogger::Logger(LL_ERROR_FUN, "Push task response failed, leak a pTaskReq: [%s]", m_strControllerName.c_str());
            }
            ++nProcessReqCount;
        }
        else
        {
            CCommonUtil::SleepThisThread(1);
        }
        time_t nCurTime = time(nullptr);
        if (nCurTime - nStatTime >= 3)
        {
            nStatTime = nCurTime;
            if (0 != nProcessReqCount)
            {
                CAquariusLogger::Logger(LL_DEBUG_FUN, "ProcessReqTaskCount: [%u] in [%s]", nProcessReqCount, m_strControllerName.c_str());
                nProcessReqCount = 0;
            }
        }
    }
    CAquariusLogger::Logger(LL_NOTICE_FUN, "Try make task request queue empty: [%s]", m_strControllerName.c_str());
    while (!m_hTaskSwap.IsRequestQueueEmpty())
    {
        ITask* pTaskReq = nullptr;
        // Try to pull a request
        m_hTaskSwap.PullRequest(pTaskReq);
        if (nullptr != pTaskReq)
        {
            // process task request
            pTaskReq->ProcTaskRequest(pTaskParam);
            // Just release the task!
            pTaskReq->ReleaseSelf();
        }
    }
    m_hTaskSwap.CloseRequestQueue();
    CAquariusLogger::Logger(LL_NOTICE_FUN, "Try make task response queue empty: [%s]", m_strControllerName.c_str());
    while (!m_hTaskSwap.IsResponseQueueEmpty())
    {
        // Output task response
        ITask* pTaskRes = nullptr;
        // Try to pull a response
        m_hTaskSwap.PullResponse(pTaskRes);
        if (nullptr != pTaskRes)
        {
            // Response just send msg to player, we can ignore it!
            // The task was useless now, just release it
            pTaskRes->ReleaseSelf();
        }
    }
    m_hTaskSwap.CloseResponseQueue();
    if (nullptr != pTaskParam)
    {
        delete pTaskParam;
        pTaskParam = nullptr;
    }
    CAquariusLogger::Logger(LL_NOTICE_FUN, "Exit this TaskLoopThread: [%s]", m_strControllerName.c_str());
}

bool CTaskController::Initialize(unsigned int nTaskThreadCount, void* pInitParam)
{
    // Subclass can init itself.
    if (!OnInit(pInitParam))
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "OnInit failed");
        return false;
    }
    m_bStartTaskThread = true;
    for (unsigned int i = 0; i < nTaskThreadCount; ++i)
    {
        m_hTaskThreadGroup.create_thread(boost::bind(&CTaskController::TaskLoopThread, this));
    }
    // Register timer to output task response
    CTimerManager::RegisterTimer(boost::bind(&CTaskController::TimerOutputTaskResponse, this), 5);
    return true;
}

void CTaskController::Destroy()
{
    m_bStartTaskThread = false;
    m_hTaskThreadGroup.join_all();
    OnDestroy();
}

CTaskController::CTaskController( const char* pszControllerName )
    :m_bStartTaskThread(false),
    m_strControllerName(pszControllerName)
{

}

CTaskController::~CTaskController()
{

}
