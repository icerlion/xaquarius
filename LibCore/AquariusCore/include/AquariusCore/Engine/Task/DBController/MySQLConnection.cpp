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
 * FileName: MySQLConnection.cpp
 * 
 * Purpose:  Implement CMySQLConnection
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/2/22 16:16
===================================================================+*/
#include "AquariusCore/Engine/Task/DBController/MySQLConnection.h"
#include "AquariusCore/Logger/AquariusLogger.h"
#include "AquariusCore/Util/TinyUtil/TimeUtil.h"

boost::mutex CMySQLConnection::ms_mutexInitMysql;

bool CMySQLConnection::InitConnection()
{
    boost::unique_lock<boost::mutex> scoped_lock(ms_mutexInitMysql);
    bool bResult = false;
    do 
    {
        // 1. init mysql
        if(nullptr == mysql_init(&m_hMysql))
        {
            break;
        }
        // 2. set mysql options
        const static unsigned int nTimeOut = 30*24*60*60;//seconds of a month
        if(mysql_options(&m_hMysql, MYSQL_OPT_CONNECT_TIMEOUT, &nTimeOut))
        {
            break;
        }
        const static unsigned int nReConnect = 1;// enable re-connect
        if(mysql_options(&m_hMysql, MYSQL_OPT_RECONNECT, &nReConnect))
        {
            break;
        }
        // 3. real connect to mysql
        if (mysql_real_connect(&m_hMysql, m_strHost.c_str(), m_strUser.c_str(), m_strPassword.c_str(), m_strDBName.c_str(), m_nPort, nullptr,CLIENT_MULTI_STATEMENTS) == nullptr)
        {
            break;
        }
        // 4. set character set
        if(mysql_set_character_set(&m_hMysql,"utf8"))
        {
            break;
        }
        bResult = true;
        CAquariusLogger::Logger(LL_NOTICE_FUN, "Success to connect mysql [%s:%d]", m_strHost.c_str(), m_nPort);
#ifdef WIN32
#pragma warning(push)
#pragma warning(disable: 4127)
#endif // WIN32
    } while (false);
#ifdef WIN32
#pragma warning(pop)
#endif // WIN32
    // 5. Check result
    if (!bResult)
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "Failed to connect mysql [%s]: [%d], msg: [%s]", m_strHost.c_str(), m_nPort, mysql_error(&m_hMysql));
    }
    return bResult;
}

bool CMySQLConnection::RunSQL( const std::string& strSQL, CDBTableResult& refOutRecordList )
{
    // 1. Check connection
    m_nInsertId = 0;
    int nRetCode = mysql_ping(&m_hMysql);
    if (0 != nRetCode)
    {
        // 2. If the connection is error, just re-init the connection
        CAquariusLogger::Logger(LL_WARNING_FUN, "Failed on mysql_ping, res: [%d], msg: [%s]", nRetCode, mysql_error(&m_hMysql));
        if (!InitConnection())
        {
            CAquariusLogger::Logger(LL_ERROR_FUN, "Failed to connect mysql, sql: [%s]", strSQL.c_str());
            return false;
        }
    }
    // 3. query mysql 
    nRetCode = mysql_real_query(&m_hMysql, strSQL.c_str(), (unsigned long)strSQL.length());
    time_t nCurTimePoint = time(nullptr);
    if (nCurTimePoint != m_nSqlStatTime)
    {
        CAquariusLogger::Logger(LL_DEBUG, "RunSQLStatistic: Count: [%u]", m_nRunSqlCount);
        m_nRunSqlCount = 0;
        m_nSqlStatTime = nCurTimePoint;
    }
    else
    {
        ++m_nRunSqlCount;
    }
    CAquariusLogger::Logger(LL_DEBUG, "RunSQL: mysql_query sql: [%s]", strSQL.c_str());
    if (0 != nRetCode)
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "Failed on mysql_query , res: [%d], msg: [%s], sql: [%s]", nRetCode, mysql_error(&m_hMysql), strSQL.c_str());
        return false;
    }
    // 4. get result
    m_nAffectedRows = (int)mysql_affected_rows(&m_hMysql);
    m_nInsertId = mysql_insert_id(&m_hMysql);

    // 5. get query result
    MYSQL_RES* pMySqlResultSet = mysql_store_result(&m_hMysql);
    bool bResult = true;
    if (nullptr != pMySqlResultSet)
    {
        // 6. get number or rows
        int num_rows = (unsigned int)mysql_num_rows(pMySqlResultSet);
        refOutRecordList.Reserve(num_rows);
        // 7. get fields number
        int nNumFields = mysql_num_fields(pMySqlResultSet);
        // 8. Output db result
        for (MYSQL_ROW row = mysql_fetch_row(pMySqlResultSet); row != nullptr; row = mysql_fetch_row(pMySqlResultSet))
        {
            // 8.1 Fetch the array of field length
            unsigned long* arrayLength = mysql_fetch_lengths(pMySqlResultSet);
            // 8.2 Reset the current record
            CDBTableRecord* pDBTableRecord = refOutRecordList.CreateDBTableRecord();
            if (nullptr == pDBTableRecord)
            {
                CAquariusLogger::Logger(LL_ERROR_FUN, "nullptr == pDBTableRecord, sql: [%s]", strSQL.c_str());
                bResult = false;
                break;
            }
            for (int i = 0; i < nNumFields; ++i)
            {
                // 8.3 Init a field to store the data from db
                CDBTableField* pField = pDBTableRecord->GetNextFieldPointer();
                if (nullptr == pField)
                {
                    CAquariusLogger::Logger(LL_ERROR_FUN, "nullptr == pField, GetNextFieldPointer failed, sql: [%s]", strSQL.c_str());
                    bResult = false;
                    break;
                }
                // 8.4 Get the field data and data length
                const char* pValue = row[i];
                unsigned long nDataLen = arrayLength[i];
                // 8.5 Assign the data to field, return false if failed, max buff length was 512 bytes
                if (!pField->AssignBuffData(nDataLen, pValue))
                {
                    CAquariusLogger::Logger(LL_CRITICAL, "Failed on pField->AssignBuffData, sql: [%s]", strSQL.c_str());
                    bResult = false;
                    break;
                }
            }
            if (!bResult)
            {
                break;
            }
        }
        // 9. free result
        mysql_free_result(pMySqlResultSet);
    }
    // 10. Free the other result set
    for ( ; ;)
    {
        // more results? -1 = no, >0 = error, 0 = yes (keep looping)
        nRetCode = mysql_next_result(&m_hMysql);
        if (-1 == nRetCode)
        {
            break;
        }
        if (nRetCode > 0)
        {
            CAquariusLogger::Logger(LL_ERROR_FUN, "Failed on mysql_next_result, res: [%d], msg: [%s], sql: [%s]", nRetCode, mysql_error(&m_hMysql), strSQL.c_str());
            bResult = false;
            break;
        }
        MYSQL_RES* pMySQLRes = mysql_store_result(&m_hMysql);
        if (nullptr != pMySQLRes)
        {
            mysql_free_result(pMySQLRes);
        }
    }
    return bResult;
}

CMySQLConnection::CMySQLConnection( const DBConfig& stDBConfig ) 
    :m_strHost(stDBConfig.strHost),
    m_nPort(stDBConfig.nPort),
    m_strUser(stDBConfig.strUser),
    m_strPassword(stDBConfig.strPassword),
    m_strDBName(stDBConfig.strDBName),
    m_nInsertId(0),
    m_nAffectedRows(0), 
    m_nSqlStatTime(0), 
    m_nRunSqlCount(0)
{
    InitConnection();
}

void CMySQLConnection::CloseConnection()
{
    mysql_close(&m_hMysql);
}
