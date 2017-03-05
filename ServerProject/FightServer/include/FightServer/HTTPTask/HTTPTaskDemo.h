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
 * FileName: HTTPTaskDemo.h
 * 
 * Purpose:  HTTPTask demo, show how to use httptask
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/2/22 16:20
===================================================================+*/
#ifndef _HTTPTASKDEMO_H_
#define _HTTPTASKDEMO_H_
#include "AquariusCore/Engine/Task/HTTPController/HTTPTask.hpp"
#include "Protocol/SSProtocol.hpp"

class CHTTPTaskDemo : public CHTTPTask
{
public:
    // Constructor
    CHTTPTaskDemo(void);

    // Destructor
    virtual ~CHTTPTaskDemo(void) override final;

    // Rewrite function from CHTTPTask::ProcHTTPTaskRequest
    virtual void ProcHTTPTaskRequest() override final;

    // Rewrite function from CHTTPTask::ProcHTTPTaskResponse
    virtual void ProcHTTPTaskResponse() override final;

    // Rewrite function from CHTTPTask::ReleaseSelf
    virtual void ReleaseSelf() override final;

    // Init http task
    void InitHTTPTask( );

    // Init pool object
    virtual void ResetPoolObject() override final;

};

typedef CObjectPool<CHTTPTaskDemo, 1024> CHTTPTaskDemoPool;

#endif // _HTTPTASKDEMO_H_
