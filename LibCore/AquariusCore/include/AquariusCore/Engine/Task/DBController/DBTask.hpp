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
 * FileName: DBTask.hpp
 * 
 * Purpose:  Base Class of DBTask
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/2/22 16:16
===================================================================+*/
#ifndef _DBTASK_HPP_
#define _DBTASK_HPP_
#include "AquariusCore/Engine/Task/Base/ITask.hpp"
#include "AquariusCore/Engine/Task/DBController/MySQLConnection.h"

class CDBTask : public ITask
{
protected:
    // Constructor
    CDBTask()
    {
    }

    // Destructor
    virtual ~CDBTask() override
    {
    }

    // Implement function from ITask::ProcTaskRequest
    virtual void ProcTaskRequest(ITaskParam* pParam) override final
    {
        CMySQLConnection* pMySQLConnection = dynamic_cast<CMySQLConnection*>(pParam);
        if (nullptr == pMySQLConnection)
        {
            CAquariusLogger::Logger(LL_ERROR_FUN, "nullptr == pMySQLConnection");
            return;
        }
        ProcDBTaskRequest(*pMySQLConnection);
    }

    // Implement function from ITask::ProcTaskResponse
    virtual void ProcTaskResponse() override final
    {
        ProcDBTaskResponse();
    }

    // DB task should implement ProcDBTaskRequest
    virtual void ProcDBTaskRequest(CMySQLConnection& hMySQLConnection) = 0;

    // DB task should implement ProcDBTaskResponse
    virtual void ProcDBTaskResponse() = 0;

};
#endif // _IDBTASK_HPP_
