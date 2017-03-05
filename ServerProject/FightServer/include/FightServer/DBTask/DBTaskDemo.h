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
 * FileName: DBTaskDemo.h
 * 
 * Purpose:  DBTask demo, show how to use dbtask
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/2/22 15:28
===================================================================+*/
 
#ifndef _DBTASKDEMO_H_
#define _DBTASKDEMO_H_
#include "AquariusCore/Engine/Task/DBController/DBTask.hpp"
#include "Protocol/SSProtocol.hpp"

class CDBTaskDemo : public CDBTask
{
public:
    // Constructor
    CDBTaskDemo();

    // Destructor
    virtual ~CDBTaskDemo(void) override final;

    // Rewrite function from CDBTask::ProcDBTaskRequest
    virtual void ProcDBTaskRequest(CMySQLConnection& hMySQLConnection) override final;

    // Rewrite function from CDBTask::ProcDBTaskResponse
    virtual void ProcDBTaskResponse() override final;

    // Rewrite function from IDBTask::ReleaseSelf
    virtual void ReleaseSelf() override final;

    // Init dbtask
    void InitDBTask(const ChatMsg& stChatMsg);

    // Init pool object
    virtual void ResetPoolObject() override final;

private:
    ChatMsg m_stChatMsg;
};

typedef CObjectPool<CDBTaskDemo, 2048> CDBTaskDemoPool;

#endif // _DBTASKDEMO_H_
