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
 * FileName: MsgUtilWriter.cpp
 * 
 * Purpose:  Implement CMsgUtilWriter
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/3/5 14:21
===================================================================+*/
#include "MsgUtilWriter.h"
#include "AquariusCore/Logger/AquariusLogger.h"
#include "AquariusCore/Util/TinyUtil/StringUtil.h"
#include "AquariusCore/Util/TinyUtil/TimeUtil.h"
#include "ProtocolHelper/ProtocolParse/MsgTypeContainer.h"
#include "ProtocolHelper/ProtocolParse/ProtocolParse.h"

std::ofstream CMsgUtilWriter::ms_ofsMsgUtilHpp;

void CMsgUtilWriter::UpdateMsgUtilHpp(const CMsgTypeStruct*  pMsgStruct)
{
    if (nullptr == pMsgStruct)
    {
        return;
    }
    if (!pMsgStruct->IsNetPkg())
    {
        return;
    }
    std::string strMsgName = pMsgStruct->GetMsgName();
    ms_ofsMsgUtilHpp << "        mapMsgInfo.insert(std::make_pair( " << strMsgName << "_Id, MsgInfo(\"" << strMsgName << "\", " << strMsgName << "_Id, sizeof(" << strMsgName << "))));" << std::endl;
}

void CMsgUtilWriter::BeginMsgUtilHpp( const std::string& strFilePath )
{
    ms_ofsMsgUtilHpp.open(strFilePath.c_str(), std::ofstream::trunc | std::ofstream::out);
    if (!ms_ofsMsgUtilHpp.is_open())
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "Failed to Open file: [%s]", strFilePath.c_str());
        return;
    }
    CProtocolParse::WriteFileSignature(ms_ofsMsgUtilHpp, "//");
    ms_ofsMsgUtilHpp << "#ifndef _MSG_UTIL_HPP_" << std::endl;
    ms_ofsMsgUtilHpp << "#define _MSG_UTIL_HPP_" << std::endl << std::endl;
    ms_ofsMsgUtilHpp << "#include \"Protocol/ProtocolType.hpp\"" << std::endl;
    ms_ofsMsgUtilHpp << "#include \"Protocol/CSProtocol.hpp\"" << std::endl;
    ms_ofsMsgUtilHpp << "#include \"Protocol/SSProtocol.hpp\"" << std::endl;
    ms_ofsMsgUtilHpp << "#include \"AquariusCore/Util/TypeReDef/TypeReDef.hpp\"" << std::endl;
    ms_ofsMsgUtilHpp << "#include \"AquariusCore/Util/TinyUtil/StringUtil.h\"" << std::endl << std::endl;
    ms_ofsMsgUtilHpp << "class CMsgUtil" << std::endl;
    ms_ofsMsgUtilHpp << "{" << std::endl;
    ms_ofsMsgUtilHpp << "public:" << std::endl;
    ms_ofsMsgUtilHpp << "    // Get msg name and msg len by id" << std::endl;
    ms_ofsMsgUtilHpp << "    static void GetMsgInfo(AUINT16 nMsgId, std::string& strMsgName, AUINT16& nMsgLen )" << std::endl;
    ms_ofsMsgUtilHpp << "    {" << std::endl;
    ms_ofsMsgUtilHpp << "        const MsgInfo& stMsgInfo = FindMsgInfo(nMsgId);" << std::endl;
    ms_ofsMsgUtilHpp << "        strMsgName = stMsgInfo.strMsgName;" << std::endl;
    ms_ofsMsgUtilHpp << "        nMsgLen = stMsgInfo.nMsgLen;" << std::endl;
    ms_ofsMsgUtilHpp << "    }" << std::endl << std::endl;
    ms_ofsMsgUtilHpp << "private:" << std::endl;
    ms_ofsMsgUtilHpp << "    // Define Msg Info struct" << std::endl;
    ms_ofsMsgUtilHpp << "    typedef struct MsgInfo" << std::endl;
    ms_ofsMsgUtilHpp << "    {" << std::endl;
    ms_ofsMsgUtilHpp << "        // Constructor" << std::endl;
    ms_ofsMsgUtilHpp << "        MsgInfo(const char* name = \"NA\", AUINT16 id = 0, AUINT16 len = 0)" << std::endl;
    ms_ofsMsgUtilHpp << "          :strMsgName(name)," << std::endl;
    ms_ofsMsgUtilHpp << "          nMsgId(id)," << std::endl;
    ms_ofsMsgUtilHpp << "          nMsgLen(len)" << std::endl;
    ms_ofsMsgUtilHpp << "        {" << std::endl;
    ms_ofsMsgUtilHpp << "        }" << std::endl;
    ms_ofsMsgUtilHpp << "        // Destructor" << std::endl;
    ms_ofsMsgUtilHpp << "        ~MsgInfo()" << std::endl;
    ms_ofsMsgUtilHpp << "        {" << std::endl;
    ms_ofsMsgUtilHpp << "        }" << std::endl;
    ms_ofsMsgUtilHpp << "        std::string strMsgName; // MsgName" << std::endl;
    ms_ofsMsgUtilHpp << "        AUINT16 nMsgId;         // MsgId" << std::endl;
    ms_ofsMsgUtilHpp << "        AUINT16 nMsgLen;        // MsgLen" << std::endl;
    ms_ofsMsgUtilHpp << "    } MsgInfo;" << std::endl << std::endl;
    ms_ofsMsgUtilHpp << "    // Key: msg id" << std::endl;
    ms_ofsMsgUtilHpp << "    // Value: msg info" << std::endl;
    ms_ofsMsgUtilHpp << "    typedef std::map<AUINT32, MsgInfo> MapMsgInfo;" << std::endl << std::endl;
    ms_ofsMsgUtilHpp << "    // Get MapMsgInfo;" << std::endl;
    ms_ofsMsgUtilHpp << "    inline static MapMsgInfo& GetMapMsgInfo()" << std::endl;
    ms_ofsMsgUtilHpp << "    {" << std::endl;
    ms_ofsMsgUtilHpp << "        // Hold all msg info;" << std::endl;
    ms_ofsMsgUtilHpp << "        static MapMsgInfo s_mapMsgInfo;" << std::endl;
    ms_ofsMsgUtilHpp << "        return s_mapMsgInfo;" << std::endl;
    ms_ofsMsgUtilHpp << "    }" << std::endl << std::endl;
    ms_ofsMsgUtilHpp << "    // Find msg info by id, if the id is not found, the msg name will be the msg id" << std::endl;
    ms_ofsMsgUtilHpp << "    static const MsgInfo& FindMsgInfo(AUINT16 nMsgId)" << std::endl;
    ms_ofsMsgUtilHpp << "    {" << std::endl;
    ms_ofsMsgUtilHpp << "        // Init mapMsgInfo before find it" << std::endl;
    ms_ofsMsgUtilHpp << "        MapMsgInfo& mapMsgInfo = GetMapMsgInfo();" << std::endl;
    ms_ofsMsgUtilHpp << "        if (mapMsgInfo.empty())" << std::endl;
    ms_ofsMsgUtilHpp << "        {" << std::endl;
    ms_ofsMsgUtilHpp << "            InitializeMapMsgInfo();" << std::endl;
    ms_ofsMsgUtilHpp << "        }" << std::endl;
    ms_ofsMsgUtilHpp << "        MapMsgInfo::const_iterator itFind = mapMsgInfo.find(nMsgId);" << std::endl;
    ms_ofsMsgUtilHpp << "        if (itFind == mapMsgInfo.end())" << std::endl;
    ms_ofsMsgUtilHpp << "        {" << std::endl;
    ms_ofsMsgUtilHpp << "            static MsgInfo s_InvalidMsgInfo;" << std::endl;
    ms_ofsMsgUtilHpp << "            s_InvalidMsgInfo.strMsgName = CStringUtil::UInt16ToHexString(nMsgId);" << std::endl;
    ms_ofsMsgUtilHpp << "            s_InvalidMsgInfo.nMsgId = nMsgId;" << std::endl;
    ms_ofsMsgUtilHpp << "            return s_InvalidMsgInfo;" << std::endl;
    ms_ofsMsgUtilHpp << "        }" << std::endl;
    ms_ofsMsgUtilHpp << "        return itFind->second;" << std::endl;
    ms_ofsMsgUtilHpp << "    }" << std::endl << std::endl;
    ms_ofsMsgUtilHpp << "    // Insert all msg info" << std::endl;
    ms_ofsMsgUtilHpp << "    static void InitializeMapMsgInfo()" << std::endl;
    ms_ofsMsgUtilHpp << "    {" << std::endl;
    ms_ofsMsgUtilHpp << "        MapMsgInfo& mapMsgInfo = GetMapMsgInfo();" << std::endl;
}

void CMsgUtilWriter::EndMsgUtilHpp()
{
    if (!ms_ofsMsgUtilHpp.is_open())
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "ms_ofsMsgUtilHpp is not open");
        return;
    }
    ms_ofsMsgUtilHpp << "    } // End of InitializeMapMsgInfo" << std::endl;
    ms_ofsMsgUtilHpp << "};" << std::endl << std::endl;
    ms_ofsMsgUtilHpp << "#endif // _MSG_UTIL_HPP_" << std::endl << std::endl;
    ms_ofsMsgUtilHpp.close();
    CAquariusLogger::Logger(LL_NOTICE, "Success to write MsgUtil.hpp");
}
