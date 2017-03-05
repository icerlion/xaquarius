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
 * FileName: DBTaskController.hpp
 * 
 * Purpose:  DBTask Controller
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/2/22 16:16
===================================================================+*/
#ifndef _DBTASK_CONTROLLER_HPP_
#define _DBTASK_CONTROLLER_HPP_

#include "AquariusCore/Engine/Task/Base/TaskController.h"
#include "AquariusCore/Engine/Task/DBController/MySQLConnection.h"
#include "AquariusCore/Engine/Config/ServerGroupConfig.h"
#include "AquariusCore/Engine/Task/DBController/DBTask.hpp"
#include "AquariusCore/Engine/Task/DBController/DBTableStruct.h"
#include "AquariusCore/Util/Singleton/ISingleton.hpp"

//////////////////////////////////////////////////////////////////////////
class CDBTaskController : public CTaskController, public ISingleton<CDBTaskController>
{
public:
    // Constructor
    CDBTaskController()
        :CTaskController("DBTaskController")
    {
    }

    // Destructor
    virtual ~CDBTaskController() override final
    {
    }

    // Input DB task
    static inline bool InputDBTask(CDBTask* pDBTask)
    {
        if (nullptr == pDBTask)
        {
            CAquariusLogger::Logger(LL_ERROR_FUN, "nullptr == pDBTask");
            return false;
        }
        return GetSingleton().InputTaskRequest(pDBTask);
    }

private:
    // Create task parameter
    virtual ITaskParam* CreateTaskParam() const override final
    {
        // This function would be called only once. The connection object can be reused!
        return new CMySQLConnection(m_stDBConfig);
    }

    // On init controller
    virtual bool OnInit(void* pInitParam) override final
    {
        // Update DB config
        DBConfig* pDBConfig = (DBConfig*)pInitParam;
        if (nullptr == pDBConfig)
        {
            return false;
        }
        m_stDBConfig = *pDBConfig;
        return true;
    }

    // On destroy controller
    virtual void OnDestroy() override final
    {
        return;
    }

private:
    // Hold DB config
    DBConfig m_stDBConfig;
};
#endif // _DBTASK_CONTROLLER_HPP_
