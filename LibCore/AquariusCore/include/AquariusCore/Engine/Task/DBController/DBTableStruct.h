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
 * FileName: DBTableStruct.h
 * 
 * Purpose:  Define DBTable struct
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/2/22 16:15
===================================================================+*/
#ifndef _DB_TABLE_STRUCT_H_
#define _DB_TABLE_STRUCT_H_
#include <string>
#include <vector>
#include "AquariusCore/Util/ObjPool/ObjectPool.hpp"
#include "AquariusCore/Util/TinyUtil/StringUtil.h"

// DBTable field data
class CDBTableField
{
public:
    // Constructor
    CDBTableField();

    // Destructor
    ~CDBTableField();

    // Reset 
    void Reset();

    // Assign buff data, can't call this function after bind.
    bool AssignBuffData(unsigned int nDataLen, const char* pData);

    // Get field as cstr
    const char* GetFieldAsCStr() const;

    // Get field as string
    const std::string& GetFieldAsString() const;

    // Get field as unsigned int
    unsigned int GetFieldAsUInt32() const;

    // Get field as int
    int GetFieldAsInt32() const;

    // Get field as UTC time
    unsigned int GetFieldAsUTCTime() const;

private:
    std::string m_strFieldValue;
};

//////////////////////////////////////////////////////////////////////////
// DB table record, hold some fields
class CDBTableRecord : public IObjInPool
{
public:
    // Constructor
    CDBTableRecord();

    // Destructor
    virtual ~CDBTableRecord() override final;

    // Implement ResetPoolObject from IObjInPool
    virtual void ResetPoolObject() override final;

    // Get next available field pointer to fill the field value
    CDBTableField* GetNextFieldPointer();

    // Get field as cstr
    const char* GetFieldAsCStr(unsigned int nIndex) const;

    // Get field as string
    const std::string& GetFieldAsString(unsigned int nIndex) const;

    // Get field as UTCTime
    unsigned int GetFieldAsUTCTime(unsigned int nIndex) const;

    // Get field as int
    unsigned int GetFieldAsUInt32(unsigned int nIndex) const;

    // Get field as unsigned int
    int GetFieldAsInt32(unsigned int nIndex) const
    {
        return static_cast<int>(GetFieldAsUInt32(nIndex));
    }

    // Get field count
    inline unsigned int GetRealFieldCount() const
    {
        return m_nArrayCursor;
    }

private:
    // Check field index by size
    bool CheckFieldIndex(unsigned int nIndex) const;

private:
    // Field array, max count: 32
    CDBTableField m_arrayField[48];
    // Array cursor
    unsigned int m_nArrayCursor;
};

typedef CObjectPool<CDBTableRecord, 8192, true> CDBTableRecordPool;

//////////////////////////////////////////////////////////////////////////
// Hold db query result, has some record
class CDBTableResult
{
public:
    // Define db table record
    // Constructor
    CDBTableResult()
    {
    }

    // Destructor
    ~CDBTableResult()
    {
        Reset();
    }

    // Reserve result number
    inline void Reserve(size_t nResultNum)
    {
        m_vRecoord.reserve(nResultNum);
    }

    // Create CDBTableRecord
    CDBTableRecord* CreateDBTableRecord();

    // Add a record 
    inline void AddRecord(CDBTableRecord* pRecord)
    {
        m_vRecoord.push_back(pRecord);
    }

    // Get record list
    inline const std::vector<CDBTableRecord*>& GetRecordList() const
    {
        return m_vRecoord;
    }

private:
    // Reset result
    void Reset();

private:
    // Record list
    std::vector<CDBTableRecord*> m_vRecoord;
};

#endif // _DB_TABLE_STRUCT_H_
