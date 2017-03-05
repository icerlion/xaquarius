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
 * FileName: ITask.hpp
 * 
 * Purpose:  Interface of backend task
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/2/22 16:03
===================================================================+*/
#ifndef _ITASK_HPP_
#define _ITASK_HPP_
#include "AquariusCore/Util/ObjPool/ObjectPool.hpp"

// Define ITaskParam, support task parameter
class ITaskParam
{
public:
    // Constructor
    ITaskParam()
    {
    }

    // Destructor
    virtual ~ITaskParam()
    {
    }
};

//////////////////////////////////////////////////////////////////////////
// Task interface
class ITask : public IObjInPool
{
public:
    // Constructor
    ITask(void)
    {
    }

    // Destructor
    virtual ~ITask(void) override
    {
    }

    // Process task request, called by TaskThread
    virtual void ProcTaskRequest(ITaskParam* pParam) = 0;

    // Process task response, called by MainThread
    virtual void ProcTaskResponse() = 0;

    // Release self, called after ProcTaskResponse in MainThread
    virtual void ReleaseSelf() = 0;
};

// Define PTask
typedef ITask* PTask;

#endif // _ITASK_HPP_
