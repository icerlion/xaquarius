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
 * FileName: MySQLConnection.h
 * 
 * Purpose:  Implement mysql connection, support run sql
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/2/22 16:16
===================================================================+*/
#ifndef _MYSQL_CONNECTION_H_
#define _MYSQL_CONNECTION_H_
#include <string>
#include "AquariusCore/Engine/Config/ServerItemDef.hpp"
#include "boost/thread.hpp"
#include "AquariusCore/Engine/Task/Base/ITask.hpp"
#include "AquariusCore/Engine/Task/DBController/DBTableStruct.h"
#ifdef WIN32
#include <Windows.h>
#endif // WIN32
#include "mysql/mysql.h"


// MySQL connection
class CMySQLConnection : public ITaskParam
{
public:
    // Constructor
    CMySQLConnection(const DBConfig& stDBConfig);

    // Destructor
    virtual ~CMySQLConnection() override final
    {
        CloseConnection();
    }

    // Run SQL which has no result set
    bool RunSQL( const std::string& strSQL)
    {
        CDBTableResult hDBTableResult;
        return RunSQL(strSQL, hDBTableResult);
    }

    // Run SQL
    bool RunSQL( const std::string& strSQL, CDBTableResult& refOutRecordList);

    // Get insert record id
    inline AUINT64 GetInsertId() const
    {
        return m_nInsertId;
    }
    
    // Get MYSQL handle
    inline MYSQL& GetMYSQLHandle()
    {
        return m_hMysql;
    }

    // Get Affected Rows
    inline unsigned int GetAffectedRows() const
    {
        return m_nAffectedRows;
    }

private:
    // Init connection
    bool InitConnection();

    // Close connection
    void CloseConnection();

private:
    // MYSQL handle
    MYSQL m_hMysql;
    // DB host ip
    std::string m_strHost;
    // DB host port
    int m_nPort;
    // user name
    std::string m_strUser;
    // password
    std::string m_strPassword;
    // db name
    std::string m_strDBName;
    // available only for insert sql
    AUINT64 m_nInsertId;
    // affected rows
    int m_nAffectedRows;
    // mutex for init
    static boost::mutex ms_mutexInitMysql;
    // Pre Stat SQL time point
    time_t m_nSqlStatTime;
    // Run SQL count in one second
    unsigned int m_nRunSqlCount;
};
#endif // _MYSQL_CONNECTION_H_
