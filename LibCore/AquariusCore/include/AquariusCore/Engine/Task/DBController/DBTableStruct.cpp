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
 * FileName: DBTableStruct.cpp
 * 
 * Purpose:  Implement db query result
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/2/22 16:15
===================================================================+*/
#include "AquariusCore/Engine/Task/DBController/DBTableStruct.h"
#include "AquariusCore/Util/TypeReDef/TypeReDef.hpp"
#include "AquariusCore/Util/TinyUtil/TimeUtil.h"
#include "AquariusCore/Logger/AquariusLogger.h"

bool CDBTableField::AssignBuffData( unsigned int nDataLen, const char* pData )
{
    // Check existed value
    if (!m_strFieldValue.empty())
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "This value has been assigned!");
        return false;
    }
    // Check pointer
    if (nullptr == pData)
    {
        // Enable nullptr field
        return true;
    }
    // Check string length
    m_strFieldValue.assign(pData);
    if (m_strFieldValue.length() != nDataLen)
    {
        CAquariusLogger::Logger(LL_CRITICAL_FUN, "Input Data StrLen: [%d], Input DataLen: [%d]", m_strFieldValue.length(), nDataLen);
        return false;
    }
    return true;
}

const char* CDBTableField::GetFieldAsCStr( ) const
{
    return m_strFieldValue.c_str();
}

const std::string& CDBTableField::GetFieldAsString( ) const
{
    return m_strFieldValue;
}

unsigned int CDBTableField::GetFieldAsUInt32() const
{
    unsigned int nResult = strtoul(m_strFieldValue.c_str(), nullptr, 10);
    return nResult;
}

int CDBTableField::GetFieldAsInt32() const
{
    return atoi(m_strFieldValue.c_str());
}

CDBTableField::CDBTableField() 
{
}

CDBTableField::~CDBTableField()
{
}


void CDBTableField::Reset()
{
    m_strFieldValue.clear();
}

unsigned int CDBTableField::GetFieldAsUTCTime() const
{
    if (m_strFieldValue.empty())
    {
        return 0;
    }
    unsigned int nUTCTime = (unsigned int)CTimeUtil::StringToUTCTime(m_strFieldValue.c_str());
    if (1 == nUTCTime)
    {
        nUTCTime = 0;
    }
    return nUTCTime;
}

//////////////////////////////////////////////////////////////////////////
const char* CDBTableRecord::GetFieldAsCStr( unsigned int nIndex ) const
{
    if (!CheckFieldIndex(nIndex))
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "CheckFieldIndex failed, target index: [%d], array size: [%d], array cursor: [%d]", nIndex, A_ARRAY_SIZE(m_arrayField), m_nArrayCursor);
        return nullptr;
    }
    const CDBTableField& stField = m_arrayField[nIndex];
    return stField.GetFieldAsCStr();
}

const std::string& CDBTableRecord::GetFieldAsString( unsigned int nIndex ) const
{
    if (!CheckFieldIndex(nIndex))
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "CheckFieldIndex failed, target index: [%d], array size: [%d], array cursor: [%d]", nIndex, A_ARRAY_SIZE(m_arrayField), m_nArrayCursor);
        const static std::string s_Invalid;
        return s_Invalid;
    }
    const CDBTableField& stField = m_arrayField[nIndex];
    return stField.GetFieldAsString();
}

unsigned int CDBTableRecord::GetFieldAsUTCTime(unsigned int nIndex) const
{
    if (!CheckFieldIndex(nIndex))
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "CheckFieldIndex failed, target index: [%d], array size: [%d], array cursor: [%d]", nIndex, A_ARRAY_SIZE(m_arrayField), m_nArrayCursor);
        return 0;
    }
    const CDBTableField& stField = m_arrayField[nIndex];
    return stField.GetFieldAsUTCTime();
}

bool CDBTableRecord::CheckFieldIndex( unsigned int nIndex ) const
{
    if (nIndex >= A_ARRAY_SIZE(m_arrayField))
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "Input index: [%d], array size: [%d]", nIndex, A_ARRAY_SIZE(m_arrayField));
        return false;
    }
    if (nIndex >= m_nArrayCursor)
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "Input index: [%d], array cursor: [%d]", nIndex, m_nArrayCursor);
        return false;
    }
    return true;
}

unsigned int CDBTableRecord::GetFieldAsUInt32( unsigned int nIndex ) const
{
    if (!CheckFieldIndex(nIndex))
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "CheckFieldIndex failed, target index: [%d], array size: [%d], array cursor: [%d]", nIndex, A_ARRAY_SIZE(m_arrayField), m_nArrayCursor);
        return 0;
    }
    const CDBTableField& stField = m_arrayField[nIndex];
    return stField.GetFieldAsUInt32();
}

CDBTableField* CDBTableRecord::GetNextFieldPointer()
{
    CDBTableField* pDBTableField = nullptr;
    if (m_nArrayCursor < A_ARRAY_SIZE(m_arrayField))
    {
        pDBTableField = &m_arrayField[m_nArrayCursor];
        ++m_nArrayCursor;
    }
    else
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "ArrayCursor: [%u], ArraySize: [%u]", m_nArrayCursor, A_ARRAY_SIZE(m_arrayField));
    }
    return pDBTableField;
}

void CDBTableRecord::ResetPoolObject()
{
    // Reset field
    for each (CDBTableField& refDBTableField in m_arrayField)
    {
        refDBTableField.Reset();
    }
    m_nArrayCursor = 0;
}

CDBTableRecord::~CDBTableRecord()
{

}

CDBTableRecord::CDBTableRecord() 
    :m_nArrayCursor(0)
{

}

void CDBTableResult::Reset()
{
    for each (CDBTableRecord* pDBTableRecord in m_vRecoord)
    {
        CDBTableRecordPool::RecycleObject(pDBTableRecord);
    }
    m_vRecoord.clear();
}

CDBTableRecord* CDBTableResult::CreateDBTableRecord()
{
    CDBTableRecord* pDBTableRecord = CDBTableRecordPool::AcquireObject();
    if (nullptr == pDBTableRecord)
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "nullptr == pDBTableRecord, CDBTableRecordPool is exhausted");
        return nullptr;
    }
    m_vRecoord.push_back(pDBTableRecord);
    return pDBTableRecord;
}
