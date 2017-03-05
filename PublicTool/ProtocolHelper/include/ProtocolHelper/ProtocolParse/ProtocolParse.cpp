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
 * FileName: ProtocolParse.cpp
 * 
 * Purpose:  Implement ProtocolParse
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/3/5 14:19
===================================================================+*/
#include "ProtocolParse.h"
#include <fstream>
#include "boost/date_time.hpp"
#include "AquariusCore/Logger/AquariusLogger.h"
#include "AquariusCore/Util/TinyUtil/StringUtil.h"
#include "AquariusCore/Util/TinyUtil/RandUtil.h"
#include "AquariusCore/Util/TinyUtil/TimeUtil.h"
#include "MsgTypeContainer.h"
#include "ProtocolHelper/CPlusPlus/CPlusPlusWriter.h"
#include "ProtocolHelper/CPlusPlus/MsgUtilWriter.h"
#include "ProtocolHelper/CSharp/CSharpMsgHandlerWriter.h"
#include "ProtocolHelper/CSharp/CSharpProtocolWriter.h"
#include "ProtocolHelper/CSharp/CSharpVerifyWriter.h"
#include "boost/property_tree/xml_parser.hpp"

std::set<AUINT16> CProtocolParse::ms_setMsgIdPool;
std::set<AUINT16> CProtocolParse::ms_setMsgIdReserved;
std::string CProtocolParse::ms_strVersionName;
std::vector<IMsgType*> CProtocolParse::ms_vGlobalFileMsgList;
std::vector<IMsgType*> CProtocolParse::ms_vClientMsgList;
std::string CProtocolParse::ms_strVersionValue;

void CProtocolParse::Initialize()
{
    time_t nCurTime = time(nullptr);
    struct tm stTM;
    localtime_s(&stTM, &nCurTime);
    char buff[32] = { 0 };
    sprintf_s(buff, "%04d%02d%02dT%02d%02d%02d", stTM.tm_year + 1900, stTM.tm_mon + 1, stTM.tm_mday, stTM.tm_hour, stTM.tm_min, stTM.tm_sec);
    ms_strVersionValue = buff;
    CMsgTypeContainer::Initialize();
}

void CProtocolParse::Destroy(void)
{
    for each (auto& pMsg in ms_vGlobalFileMsgList)
    {
        delete pMsg;
    }
    ms_vGlobalFileMsgList.clear();
    CMsgTypeContainer::Destroy();
}

bool CProtocolParse::ReadFromXML(const char* pszFilePath, std::vector<IMsgType*>& vFileMsgList, std::vector<std::string>& vIncludeFile)
{
    // 1. Load xml file
    boost::property_tree::ptree ptFile;
    boost::property_tree::read_xml(pszFilePath, ptFile);
    // 2. Find root node 
    boost::property_tree::ptree pRoot = ptFile.get_child("MsgDef");
    bool bResult = true;
    // 3. Loop Item to parse msg type
    for each (auto ptItem in pRoot)
    {
        std::string strTagName = ptItem.first;
        auto ptNode = ptItem.second;
        if ("<xmlcomment>" == strTagName)
        {
            continue;
        }
        else if ("Include" == strTagName)
        {
            vIncludeFile.push_back(ptNode.get<std::string>("<xmlattr>.file"));
        }
        else if ("VersionName" == strTagName)
        {
            ms_strVersionName = ptNode.get<std::string>("<xmlattr>.value");
        }
        else if ("Struct" == strTagName)
        {
            bResult = ParseStruct(ptNode, vFileMsgList);
        }
        else if ("Enum" == strTagName)
        {
            bResult = ParseEnum(ptNode, vFileMsgList);
        }
        else
        {
            CAquariusLogger::Logger(LL_ERROR_FUN, "Unknown tag name: %s", strTagName.c_str());
            bResult = false;
        }
        // 3.3 Check result
        if (!bResult)
        {
            break;
        }
    }
    return bResult;
}

bool CProtocolParse::ParseEnum(const boost::property_tree::ptree& ptNode, std::vector<IMsgType*>& vFileMsgList)
{
    // 1. allocate CMsgTypeEnum
    CMsgTypeEnum* pEnum = new CMsgTypeEnum();
    // 2. Load common attribute
    pEnum->SetMsgName(ptNode.get<std::string>("<xmlattr>.name"));
    pEnum->SetDesc(ptNode.get<std::string>("<xmlattr>.desc"));
    // 3. Loop Field
    bool bResult = true;
    for each (auto ptItem in ptNode)
    {
        std::string strTagName = ptItem.first;
        if (strTagName != "Field")
        {
            continue;
        }
        auto ptField = ptItem.second;
        EnumField stField;
        stField.name = ptField.get<std::string>("<xmlattr>.name");
        stField.value = ptField.get<int>("<xmlattr>.value");;
        stField.desc = ptField.get<std::string>("<xmlattr>.desc");
        if (!pEnum->AddField(stField))
        {
            bResult = false;
            break;
        }
    }
    // 4. Update enum name set
    if (bResult)
    {
        vFileMsgList.push_back(pEnum);
        CMsgTypeContainer::AddEnum(pEnum->GetMsgName());
    }
    return bResult;
}

void CProtocolParse::AddFieldForNetMsg(CMsgTypeStruct* pStruct)
{
    if (nullptr == pStruct)
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "nullptr == pStruct");
        return;
    }
    pStruct->SetMsgId(AllocateMsgId());
    // Add msg length as the second field
    StructField stFieldMsgLen;
    stFieldMsgLen.name = "msg_len";
    stFieldMsgLen.desc = "msg len, you should not change the value";
    stFieldMsgLen.type = "AUINT16";
    stFieldMsgLen.element_count = 1;
    size_t nTypeSize = CMsgTypeContainer::GetMsgTypeSize(stFieldMsgLen.type);
    stFieldMsgLen.size = nTypeSize * stFieldMsgLen.element_count;
    stFieldMsgLen.init_value = "sizeof(" + pStruct->GetMsgName() + ")";
    pStruct->AddField(stFieldMsgLen);
    // Add msg id as the first field
    StructField stFieldMsgId;
    stFieldMsgId.name = "msg_id";
    stFieldMsgId.desc = "msg id, you should not change the value";
    stFieldMsgId.type = "AUINT16";
    stFieldMsgId.element_count = 1;
    nTypeSize = CMsgTypeContainer::GetMsgTypeSize(stFieldMsgId.type);
    stFieldMsgId.size = nTypeSize * stFieldMsgId.element_count;
    stFieldMsgId.init_value = pStruct->GetMsgName() + "_Id";
    pStruct->AddField(stFieldMsgId);
}

bool CProtocolParse::ParseStruct(const boost::property_tree::ptree& ptNode, std::vector<IMsgType*>& vFileMsgList)
{
    // 1. Allocate CMsgTypeStruct
    CMsgTypeStruct* pStruct = new CMsgTypeStruct();
    // 2. Read common attribute
    pStruct->SetMsgName(ptNode.get<std::string>("<xmlattr>.name"));
    pStruct->SetDesc(ptNode.get<std::string>("<xmlattr>.desc"));
    pStruct->SetMonitorFlag(ptNode.get<std::string>("<xmlattr>.monitor", "false"));
    // 3. Check net pkg flag
    bool bIsNetPkg = ptNode.get<bool>("<xmlattr>.netpkg", false);
    if (bIsNetPkg)
    {
        AddFieldForNetMsg(pStruct);
        // Check const msg id 
        int nMsgId = ptNode.get<int>("<xmlattr>.msgid", 0);
        if (0 != nMsgId)
        {
            if (nMsgId < 0 || nMsgId >= 0x2FFF)
            {
                CAquariusLogger::Logger(LL_ERROR_FUN, "Invalid pre-define msg id: [0x%04X]", nMsgId);
                return false;
            }
            if (!ms_setMsgIdReserved.insert((AUINT16)nMsgId).second)
            {
                CAquariusLogger::Logger(LL_ERROR_FUN, "Reduplicate reserved msg id: [0x%04X]", nMsgId);
                return false;
            }
            pStruct->SetMsgId((AUINT16)nMsgId);
        }
    }
    // 5. Loop to load field of the struct
    bool bResult = true;
    for each (auto ptItem in ptNode)
    {
        std::string strTagName = ptItem.first;
        if (strTagName != "Field")
        {
            continue;
        }
        auto ptField = ptItem.second;
        StructField stField;
        stField.name = ptField.get<std::string>("<xmlattr>.name");
        stField.desc = ptField.get<std::string>("<xmlattr>.desc");
        stField.type = ptField.get<std::string>("<xmlattr>.type");
        stField.element_count = ptField.get<int>("<xmlattr>.element_count", 1);
        // Calc the field size 
        size_t nTypeSize = CMsgTypeContainer::GetMsgTypeSize(stField.type);
        if (0 == nTypeSize)
        {
            CAquariusLogger::Logger(LL_ERROR_FUN, "Fail to find type size: %s", stField.name.c_str());
            bResult = false;
            break;
        }
        else
        {
            stField.size = nTypeSize * stField.element_count;
        }
        // Set init value for basic type and enum type
        if (( CMsgTypeContainer::IsBasicTypeName(stField.type)  || CMsgTypeContainer::IsEnumTypeName(stField.type)) 
            && 1 == stField.element_count)
        {
            stField.init_value = CMsgTypeContainer::GetMsgTypeInitValue(stField.type);
        }
        // Add element count before array
        pStruct->AddField(stField);
    }
    // 6. Check result
    if (!bResult)
    {
        return false;
    }
    // 7. Set msg id
    if (pStruct->IsNetPkg() && 0 == pStruct->GetMsgId())
    {
        AUINT16 nAllocateMsgId = AllocateMsgId();
        if (0 == nAllocateMsgId)
        {
            CAquariusLogger::Logger(LL_ERROR_FUN, "0 == nAllocateMsgId");
            return false;
        }
        pStruct->SetMsgId(nAllocateMsgId);
    }
    vFileMsgList.push_back(pStruct);
    // 8. Add struct size
    return CMsgTypeContainer::AddStruct(pStruct->GetMsgName(), pStruct->GetStructSize());
}

bool CProtocolParse::ParseProtocolFile(const char* pszXMLFile, const char* pszHPPFile, bool bIsClientUsing, bool bWriteProtocolVersion)
{
    // 1. Write log
    CAquariusLogger::Logger(LL_NOTICE, "Begin to parse protocol file: %s", pszXMLFile);
    // 2. Load and parse XML file
    std::vector<IMsgType*> vFileMsgList;
    std::vector<std::string> vIncludeFile;
    try
    {
        if (!ReadFromXML(pszXMLFile, vFileMsgList, vIncludeFile))
        {
            CAquariusLogger::Logger(LL_ERROR_FUN, "Fail to parse protocol file: %s", pszXMLFile);
            return false;
        }
    }
    catch (const boost::property_tree::ptree_error& boostPTError)
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "ParseProtocolFile failed as [%s]", boostPTError.what());
        return false;
    }
    // 3. Write to hpp file
    if (!CCPlusPlusWriter::WriteToHpp(pszHPPFile, vFileMsgList, vIncludeFile, bWriteProtocolVersion))
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "Fail to generate write hpp file: %s", pszHPPFile);
        return false;
    }
    CAquariusLogger::Logger(LL_NOTICE, "Success to parse protocol file [%s] to hpp file [%s]", pszXMLFile, pszHPPFile);
    // 4. Client using msg list
    if (bIsClientUsing)
    {
        std::copy(vFileMsgList.begin(), vFileMsgList.end(), std::back_inserter(ms_vClientMsgList));
    }
    return true;
}

void CProtocolParse::WriteCSharpFile()
{
    std::set<std::string> setPrefix;
    setPrefix.insert("GC_");
    CCSharpProtocolWriter::WriteCSProtocolFile(ms_vClientMsgList, "./CSharp/Protocol/CSProtocol.cs");
    CCSharpVerifyWriter::WriteCSVerifyFile(ms_vClientMsgList, "./CSharp/Protocol/VerifyProtocol.cs");
    CCSharpMsgHandlerWriter::WriteCSMsgHandlerFile(ms_vClientMsgList, "./CSharp/Protocol/ServerMsgHandler.cs", setPrefix);
}

void CProtocolParse::ParseProtocolGroup( )
{
    CMsgUtilWriter::BeginMsgUtilHpp("./CPlusPlus/Protocol/MsgUtil.hpp");
    ParseProtocolFile("./xml/ProtocolType.xml", "./CPlusPlus/Protocol/ProtocolType.hpp", true, true);
    ParseProtocolFile("./xml/CSProtocol.xml", "./CPlusPlus/Protocol/CSProtocol.hpp", true, false);
    ParseProtocolFile("./xml/SSProtocol.xml", "./CPlusPlus/Protocol/SSProtocol.hpp", false, false);
    CMsgUtilWriter::EndMsgUtilHpp();
    WriteCSharpFile();
    WriteProtocolProperties();
}

void CProtocolParse::WriteFileSignature(std::ofstream& ofs, const char* pszCommentStr)
{
    ofs << pszCommentStr << " Warning: This file is auto-generated by ParseProtocol.exe, you should not change it!" << std::endl;
    ofs << pszCommentStr << " Time: " << CTimeUtil::UTCTimeToReadableString(time(0)) << std::endl;
}

const std::string& CProtocolParse::GetProtocolVersion()
{
    return ms_strVersionValue;
}

AUINT16 CProtocolParse::AllocateMsgId()
{
    AUINT16 nMsgId = 0;
    size_t nStoreMsgIdCount = ms_setMsgIdPool.size();
    do 
    {
        nMsgId = (AUINT16)CRandUtil::RandNumber(0x2000, 0x9000);
        if (ms_setMsgIdPool.insert(nMsgId).second)
        {
            break;
        }
    } while (ms_setMsgIdPool.size() < 8192);
    if (ms_setMsgIdPool.size() == nStoreMsgIdCount)
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "There is no more msg id in pool");
        assert(false);
        return 0;
    }
    return nMsgId;
}

bool CProtocolParse::WriteProtocolProperties( )
{
    std::ofstream ofs("./protocol.properties");
    WriteFileSignature(ofs, "#");
    ofs << "ProtocolVersion=" << GetProtocolVersion() << std::endl;
    ofs.close();
    return true;
}