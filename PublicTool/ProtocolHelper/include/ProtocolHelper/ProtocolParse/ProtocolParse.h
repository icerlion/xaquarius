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
 * FileName: ProtocolParse.h
 * 
 * Purpose:  Parse protocol from xml file
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/3/5 14:18
===================================================================+*/
#ifndef _PROTOCOL_PARSE_H_
#define _PROTOCOL_PARSE_H_
#include "MsgTypeDef.h"
#include "boost/property_tree/ptree.hpp"
#include <set>

class CProtocolParse
{
public:
    static void Initialize();

    static void Destroy(void);

    // Parse protocol group
    static void ParseProtocolGroup();

    // Get std::string ms_strVersionName
    inline static std::string GetVersionName()
    {
        return ms_strVersionName;
    }

    // Write file signature at the first line.
    static void WriteFileSignature(std::ofstream& ofs, const char* pszCommentStr);

    // Get protocol version
    static const std::string& GetProtocolVersion();

private:
    /**
     * Description: Parse xml file to hpp file
     * Parameter const std::string& strXMLFile [in]: xml file path
     * Parameter const std::string& strHPPFile [in]: hpp file path
     * Parameter bool bIsClientUsing     [in]: client using flag
     * Returns: bool: true if success
     */
    static bool ParseProtocolFile(const char* pszXMLFile, const char* pszHPPFile, bool bIsClientUsing, bool bWriteProtocolVersion);

    // Write CS file
    static void WriteCSharpFile();
    
    // Read xml file, parse msg type
    static bool ReadFromXML(const char* pszFilePath, std::vector<IMsgType*>& vFileMsgList, std::vector<std::string>& vIncludeFile);

    // Parse Enum define
    static bool ParseEnum(const boost::property_tree::ptree& ptNode, std::vector<IMsgType*>& vFileMsgList);

    // Parse Struct define
    static bool ParseStruct(const boost::property_tree::ptree& ptNode, std::vector<IMsgType*>& vFileMsgList);

    // add field for net msg package
    static void AddFieldForNetMsg(CMsgTypeStruct* pStruct);

    // Retrieve a msg id from pool
    static AUINT16 AllocateMsgId();

    // Write Protocol.properties file
    static bool WriteProtocolProperties();

private:
    // Msg list.
    static std::vector<IMsgType*> ms_vGlobalFileMsgList;
    // Client msg list
    static std::vector<IMsgType*> ms_vClientMsgList;
    // rand msg id
    static std::set<AUINT16> ms_setMsgIdPool;
    // reserved msg id
    static std::set<AUINT16> ms_setMsgIdReserved;
    // Version Name
    static std::string ms_strVersionName;
    // Version value
    static std::string ms_strVersionValue;
};

#endif // _PROTOCOL_PARSE_H_
