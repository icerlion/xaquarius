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
 * FileName: TaskSwap.h
 * 
 * Purpose:  Thread safe, task swap implement Producer-Consumer mode
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/2/22 16:04
===================================================================+*/
#ifndef _TASK_SWAP_H_
#define _TASK_SWAP_H_

#include "boost/thread/sync_queue.hpp"
#include "AquariusCore/Engine/Task/Base/ITask.hpp"

class CTaskSwap
{
public:
    // Constructor
    CTaskSwap(void)
    {
    }

    // Destructor
    ~CTaskSwap(void)
    {
    }

    // Is Request Queue Empty
    inline bool IsRequestQueueEmpty()
    {
        return m_sqRequest.empty();
    }
    
    // Close Request Queue
    inline void CloseRequestQueue()
    {
        m_sqRequest.close();
    }
    
    // Is Response Queue empty
    inline bool IsResponseQueueEmpty()
    {
        return m_sqResponse.empty();
    }

    // Close Response Queue
    inline void CloseResponseQueue()
    {
         m_sqResponse.close();
    }
    
    // Push a request
    bool PushRequest(PTask pTask);

    // Pull a request
    bool PullRequest(PTask& pTask);

    // Push a response
    bool PushResponse(PTask pTask);

    // Pull response
    bool PullResponse(PTask& pTask);

    // Get request task count
    inline AUINT32 GetReqTaskCount()
    {
        return (AUINT32)m_sqRequest.size();
    }

    // Get response task count
    inline AUINT32 GetResTaskCount()
    {
        return (AUINT32)m_sqResponse.size();
    }

private:
    /**
     * Description: Try push task to queue
     * Parameter boost::sync_queue<PTask> & refSQ [in/out]: sync queue task
     * Parameter PTask pTask                      [in]: task
     * Returns bool: true if success
     */
    static bool TryPushQueue(boost::sync_queue<PTask>& refSQ, PTask pTask);

    /**
     * Description: Try pull task from queue
     * Parameter boost::sync_queue<PTask> & refSQ [in]: task sync queue
     * Parameter PTask & pTask                    [out]: task pointer 
     * Parameter unsigned int nThreadCount        [in]: work thread count
     * Returns bool: true if success
     */
    static bool TryPullQueue(boost::sync_queue<PTask>& refSQ, PTask& pTask);

private:
    // Task request queue
    boost::sync_queue<PTask> m_sqRequest;
    // Task response queue
    boost::sync_queue<PTask> m_sqResponse;
};

#endif // _TASK_SWAP_H_
