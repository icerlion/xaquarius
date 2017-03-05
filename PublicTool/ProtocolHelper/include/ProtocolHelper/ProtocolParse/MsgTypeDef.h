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
 * FileName: MsgTypeDef.h
 * 
 * Purpose:  Define msg type
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/3/5 14:19
===================================================================+*/
#ifndef _MSG_TYPE_DEF_H_
#define _MSG_TYPE_DEF_H_
#include <string>
#include <vector>
#include <set>
#include "AquariusCore/Logger/AquariusLogger.h"
#include "AquariusCore/Util/TinyUtil/StringUtil.h"

// Basic msg type
class IMsgType
{
public:
    // Constructor
    IMsgType()
    {
    }

    // Destructor
    virtual ~IMsgType()
    {
    }

    // Set the msg name
    inline void SetMsgName(const std::string& name)
    {
        m_strMsgName = name;
    }

    // Set the desc
    void SetDesc(const std::string& desc);

    // Get msg name
    inline const std::string& GetMsgName() const
    {
        return m_strMsgName;
    }

    // Get desc
    inline const std::string& GetDesc() const
    {
        return m_strDesc;
    }

protected:
    // msg name
    std::string m_strMsgName;
    // msg desc
    std::string m_strDesc;
    // Field name set
    std::set<std::string> m_setFieldName;

};

//////////////////////////////////////////////////////////////////////////
// Enum field type define
typedef struct EnumField 
{
    // constructor
    EnumField()
        :value(0)
    {
    }
    std::string name;
    std::string desc;
    int value;
} EnumField;

// Define enum type
class CMsgTypeEnum : public IMsgType
{
public:
    // Constructor
    CMsgTypeEnum()
    {
    }

    // Destructor
    virtual ~CMsgTypeEnum() override final
    {
    }

    // Add field for this enum
    bool AddField(const EnumField& stField);

    // Get field list
    inline const std::vector<EnumField>& GetFieldList() const
    {
        return m_vFieldList;
    }

private:
    // Enum field list
    std::vector<EnumField> m_vFieldList;
    // value of enum field
    std::set<int> m_setFieldValue;
};

//////////////////////////////////////////////////////////////////////////
// Define struct field
typedef struct StructField 
{
    // Constructor
    StructField()
        :element_count(1),
        size(0)
    {
    }

    std::string ShortName() const;

    // Field type
    std::string type;
    // Field name
    std::string name;
    // Init value
    std::string init_value;
    // Desc of this struct
    std::string desc;
    // Element count, greater than 1 if it was array
    int element_count; 
    // size of this field
    size_t size;
} StructField;

// Define struct type
class CMsgTypeStruct : public IMsgType
{
public:
    // Constructor
    CMsgTypeStruct()
        :m_nMsgId(0),
        m_nStructSize(0),
        m_strMonitorFlag("false")
    {
    }

    // Destructor
    virtual ~CMsgTypeStruct() override final
    {
    }


    // Add file into this field.
    inline void AddField(const StructField& refField)
    {
        m_nStructSize += refField.size;
        m_vFieldList.push_back(refField);
    }

    // Set msg id of this struct
    inline void SetMsgId(unsigned short nMsgId)
    {
        m_nMsgId = nMsgId;
    }

    // Get field list
    inline const std::vector<StructField>& GetFieldList() const
    {
        return m_vFieldList;
    }

    // Get msg id if it was net pkg.
    inline unsigned short GetMsgId() const
    {
        return m_nMsgId;
    }

    // Get struct size
    inline size_t GetStructSize() const
    {
        return m_nStructSize;
    }

    // Get the flag of net pkg.
    inline bool IsNetPkg() const
    {
        return 0 != m_nMsgId;
    }

    // Get the flag of monitor this msg, only for net msg
    inline const std::string& GetMonitorFlag() const
    {
        return m_strMonitorFlag;
    }

    // Set watch flag
    inline void SetMonitorFlag(const std::string& bFlag)
    {
        m_strMonitorFlag = bFlag;
    }

private:
    // Field list
    std::vector<StructField> m_vFieldList;
    // Msg id, only for net pkg
    unsigned short m_nMsgId;
    // Struct size
    size_t m_nStructSize;
    // Write log when watch this msg
    std::string m_strMonitorFlag;
};

#endif // _MSG_TYPE_DEF_HPP_
