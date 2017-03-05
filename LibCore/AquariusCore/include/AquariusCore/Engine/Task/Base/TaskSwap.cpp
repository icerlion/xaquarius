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
 * FileName: TaskSwap.cpp
 * 
 * Purpose:  Implement thread-safe TaskSwap
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/2/22 16:04
===================================================================+*/
#include "AquariusCore/Engine/Task/Base/TaskSwap.h"
#include "AquariusCore/Logger/AquariusLogger.h"

bool CTaskSwap::PushRequest( PTask pTask )
{
    bool bResult = TryPushQueue(m_sqRequest, pTask);
    if (!bResult)
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "Failed to PushRequest!");
    }
    return bResult;
}

bool CTaskSwap::PushResponse( PTask pTask )
{
    bool bResult = TryPushQueue(m_sqResponse, pTask);
    if (!bResult)
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "Failed to PushResponse!");
    }
    return bResult;
}

bool CTaskSwap::PullRequest( PTask& pTask )
{
    bool bResult = TryPullQueue(m_sqRequest, pTask);
    if (!bResult)
    {
        CAquariusLogger::Logger(LL_WARNING_FUN, "Failed to PullRequest!");
    }
    return bResult;
}

bool CTaskSwap::PullResponse( PTask& pTask )
{
    // Only main thread will call PullResponse
    bool bResult = TryPullQueue(m_sqResponse, pTask);
    if (!bResult)
    {
        CAquariusLogger::Logger(LL_WARNING_FUN, "Failed to PullResponse!");
    }
    return bResult;
}

bool CTaskSwap::TryPushQueue( boost::sync_queue<PTask>& refSQ, PTask pTask )
{
    CAquariusLogger::Logger(LL_DEBUG_FUN, "Queue Size: [%u]", refSQ.size());
    const static int MAX_TRY_PUSH_COUNT = 3;
    bool bResult = false;
    int nMaxCount = 0;
    do 
    {
        if (boost::concurrent::queue_op_status::success == refSQ.try_push(pTask))
        {
            bResult = true;
            break;
        }
    } while (++nMaxCount < MAX_TRY_PUSH_COUNT);
    return bResult;
}

bool CTaskSwap::TryPullQueue( boost::sync_queue<PTask>& refSQ, PTask& pTask)
{
    // Pull always return true.
    // 1. refSQ was empty
    // 2. WorkThread call refSQ.try_pull, pTask is nullptr
    // 3. MainThread call refSQ.try_push
    // 4. refSQ was not empty
    refSQ.try_pull(pTask);
    return true;
}
