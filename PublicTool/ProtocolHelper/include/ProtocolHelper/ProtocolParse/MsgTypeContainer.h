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
 * FileName: MsgTypeContainer.h
 * 
 * Purpose:  Container of msg type
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/3/5 14:19
===================================================================+*/
#ifndef _MSG_TYPE_CONTAINER_H_
#define _MSG_TYPE_CONTAINER_H_
#include <map>
#include <string>
#include <vector>


// basic type info
enum class EMsgTypeCateGory : int
{
    Invalid = 0,
    TypeBasic = 1,
    TypeEnum = 2,
    TypeStruct = 3,
};

// Basic type info
typedef struct MsgTypeInfo
{
    // Constructor
    MsgTypeInfo(const std::string& _name = "", const std::string& _init = "", size_t _size = 0, EMsgTypeCateGory _type = EMsgTypeCateGory::Invalid, const std::string& _net_order_fun = "", const std::string& _host_order_fun = "")
        :name(_name),
        init(_init),
        size(_size),
        type(_type),
        net_order_fun(_net_order_fun),
        host_order_fun(_host_order_fun)
    {
    }
    // type name
    std::string name;
    // init string
    std::string init;
    // size of the type
    size_t size;
    // class type
    EMsgTypeCateGory type;
    // function of ToNetOrder
    std::string net_order_fun;
    // function of ToHostOrder
    std::string host_order_fun;
} MsgTypeInfo;

class CMsgTypeContainer
{
public:
    // Get type init value by name
    static const std::string& GetMsgTypeInitValue(const std::string& strType);

    // Get type size by name
    static size_t GetMsgTypeSize(const std::string& strType);

    // Get protocol version
    static const std::string& GetProtocolVersion()
    {
        return ms_strProtocolVersion;
    }

    // Add enum to type map, Enum name can be the reduplicate.
    static bool AddEnum(const std::string& strName);

    // Add enum to type map, struct name can not be reduplicate.
    static bool AddStruct(const std::string& strName, size_t nSize);

    // Check struct name
    static bool IsStructTypeName(const std::string& strName);

    // Check enum name
    static bool IsEnumTypeName( const std::string& strName );

    // Check basic type name
    static bool IsBasicTypeName( const std::string& strName );

    // Find type info by name
    static const MsgTypeInfo& GetMsgTypeInfo(const std::string& strName);

    // Init map type info
    static void Initialize();

    // Destroy
    static void Destroy();

private:
    // Key: type name
    // Value: type info
    static std::map<std::string, MsgTypeInfo> ms_mapMsgTypeInfo;
    // version of protocol
    static std::string ms_strProtocolVersion;
};

#endif // _MSG_TYPE_CONTAINER_H_
