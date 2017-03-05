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
 * FileName: MsgTypeContainer.cpp
 * 
 * Purpose:  MsgTypeContainer
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/3/5 14:19
===================================================================+*/
#include "MsgTypeContainer.h"
#include "AquariusCore/Logger/AquariusLogger.h"
#include "AquariusCore/Util/TypeReDef/TypeReDef.hpp"
#include "AquariusCore/Util/TinyUtil/TimeUtil.h"

std::map<std::string, MsgTypeInfo> CMsgTypeContainer::ms_mapMsgTypeInfo;


void CMsgTypeContainer::Initialize()
{
    if (!ms_mapMsgTypeInfo.empty())
    {
        return;
    }
    ms_mapMsgTypeInfo.insert(std::make_pair("char", MsgTypeInfo("char", "0", sizeof(char), EMsgTypeCateGory::TypeBasic)));
    ms_mapMsgTypeInfo.insert(std::make_pair("AINT8", MsgTypeInfo("AINT8", "0", sizeof(AINT8), EMsgTypeCateGory::TypeBasic)));
    ms_mapMsgTypeInfo.insert(std::make_pair("AUINT8", MsgTypeInfo("AUINT8", "0", sizeof(AUINT8), EMsgTypeCateGory::TypeBasic)));
    ms_mapMsgTypeInfo.insert(std::make_pair("AINT16", MsgTypeInfo("AINT16", "0", sizeof(AINT16), EMsgTypeCateGory::TypeBasic, "CCommonUtil::HostToNet16", "CCommonUtil::NetToHost16")));
    ms_mapMsgTypeInfo.insert(std::make_pair("AUINT16", MsgTypeInfo("AUINT16", "0", sizeof(AUINT16), EMsgTypeCateGory::TypeBasic, "CCommonUtil::HostToNet16", "CCommonUtil::NetToHost16")));
    ms_mapMsgTypeInfo.insert(std::make_pair("AINT32", MsgTypeInfo("AINT32", "0", sizeof(AINT32), EMsgTypeCateGory::TypeBasic, "CCommonUtil::HostToNet32", "CCommonUtil::NetToHost32")));
    ms_mapMsgTypeInfo.insert(std::make_pair("AUINT32", MsgTypeInfo("AUINT32", "0", sizeof(AUINT32), EMsgTypeCateGory::TypeBasic, "CCommonUtil::HostToNet32", "CCommonUtil::NetToHost32")));
}

void CMsgTypeContainer::Destroy()
{
    return;
}

const MsgTypeInfo& CMsgTypeContainer::GetMsgTypeInfo(const std::string& strName)
{
    static MsgTypeInfo stInvalidType;
    std::map<std::string, MsgTypeInfo>::const_iterator itFind = ms_mapMsgTypeInfo.find(strName);
    if (itFind != ms_mapMsgTypeInfo.end())
    {
        return itFind->second;
    }
    return stInvalidType;
}

const std::string& CMsgTypeContainer::GetMsgTypeInitValue(const std::string& strType)
{
    const MsgTypeInfo& refTypeInfo = GetMsgTypeInfo(strType);
    if (EMsgTypeCateGory::Invalid == refTypeInfo.type)
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "Fail to find type init value, type name:%s", strType.c_str());
    }
    return refTypeInfo.init;
}

size_t CMsgTypeContainer::GetMsgTypeSize(const std::string& strType)
{
    const MsgTypeInfo& refTypeInfo = GetMsgTypeInfo(strType);
    if (EMsgTypeCateGory::Invalid == refTypeInfo.type)
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "Fail to find type size, type name:%s", strType.c_str());
    }
    return refTypeInfo.size;
}

bool CMsgTypeContainer::AddStruct( const std::string& strName, size_t nSize)
{
    // 1. Check size of struct
    if (nSize > CONST_MAX_MSG_LENGTH - 10)
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "Msg size is bigger than %d bytes: %s, size:%d",
            CONST_MAX_MSG_LENGTH,
            strName.c_str(),
            nSize);
        return false;
    }
    // 2. Check msg name
    MsgTypeInfo stTypeInfo(strName, "", nSize, EMsgTypeCateGory::TypeStruct);
    if (!(ms_mapMsgTypeInfo.insert(std::make_pair(stTypeInfo.name, stTypeInfo)).second))
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "Reduplicate struct name:%s", strName.c_str());
        return false;
    }
    return true;
}

bool CMsgTypeContainer::AddEnum( const std::string& strName )
{
    // 1. Check duplicate name type
    const MsgTypeInfo& refTypeInfo = GetMsgTypeInfo(strName);
    if (EMsgTypeCateGory::Invalid != refTypeInfo.type)
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "Invalid enum name:%s", strName.c_str());
        return false;
    }
    // 2. Add enum to type list
    MsgTypeInfo stTypeInfo(strName, strName + "::Invalid", sizeof(AINT16), EMsgTypeCateGory::TypeEnum);
    return ms_mapMsgTypeInfo.insert(std::make_pair(stTypeInfo.name, stTypeInfo)).second;
}

bool CMsgTypeContainer::IsStructTypeName( const std::string& strName )
{
    const MsgTypeInfo& type = GetMsgTypeInfo(strName);
    return EMsgTypeCateGory::TypeStruct == type.type;
}

bool CMsgTypeContainer::IsEnumTypeName( const std::string& strName )
{
    const MsgTypeInfo& type = GetMsgTypeInfo(strName);
    return EMsgTypeCateGory::TypeEnum == type.type;
}

bool CMsgTypeContainer::IsBasicTypeName( const std::string& strName )
{
    const MsgTypeInfo& type = GetMsgTypeInfo(strName);
    return EMsgTypeCateGory::TypeBasic == type.type;
}
