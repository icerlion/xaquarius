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
 * FileName: CSharpMsgHandlerWriter.cpp
 * 
 * Purpose:  Write msg handle function using C# language
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/3/5 14:20
===================================================================+*/
#include "CSharpMsgHandlerWriter.h"
#include "AquariusCore/Logger/AquariusLogger.h"
#include "AquariusCore/Util/TinyUtil/StringUtil.h"
#include "ProtocolHelper/ProtocolParse/ProtocolParse.h"
#include "boost/algorithm/string.hpp"

std::vector<std::string> CCSharpMsgHandlerWriter::ms_vMsgDispatch;

void CCSharpMsgHandlerWriter::WriteCSMsgHandlerFile( const std::vector<IMsgType*>& vClientMsgList, const char* pszFilePath, const std::set<std::string>& setMsgPrefix )
{
    // 1. Open file stream
    std::ofstream ofs;
    ofs.open(pszFilePath, std::fstream::out);
    if (!ofs.is_open())
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "Failed to open file: [%s]", pszFilePath);
        return;
    }
    std::vector<std::string> vToken = CStringUtil::SplitStringToToken(pszFilePath, '/');
    if (vToken.empty())
    {
        return;
    }
    std::string strClassName = vToken.back();
    strClassName.resize(strClassName.length() - 3);
    // 2. Write file signature and file header
    CProtocolParse::WriteFileSignature(ofs, "//");
    ofs << "using System;" << std::endl;
    ofs << "using System.Text;" << std::endl << std::endl;
    ofs << "namespace NetIOLayer" << std::endl;
    ofs << "{" << std::endl;
    ofs << "    public class " << strClassName << std::endl;
    ofs << "    {" << std::endl;
    // 3. Loop every msg to write handle function
    for each (auto& value in vClientMsgList)
    {
        const CMsgTypeStruct* pMsg = dynamic_cast<const CMsgTypeStruct*>(value);
        // The msg maybe an Enum
        if (nullptr == pMsg || !pMsg->IsNetPkg())
        {
            continue;
        }
        WriteMsgHandler(pMsg, ofs, setMsgPrefix);
    }
    // 4. Write msg dispatch function
    WriteFunMsgDispatch(ofs);
    ofs << "    }" << std::endl;
    ofs << "}" << std::endl;
    CAquariusLogger::Logger(LL_NOTICE, "Success to write CS file: [%s]", pszFilePath);
}

void CCSharpMsgHandlerWriter::WriteMsgHandler( const CMsgTypeStruct* pMsg, std::ofstream& ofs, const std::set<std::string>& setMsgPrefix )
{
    // 1. Check parameter
    if (nullptr == pMsg)
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "nullptr == pMsg");
        return;
    }
    // Skip non-net msg
    if (!pMsg->IsNetPkg())
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "!pMsg->IsNetPkg");
        return;
    }
    bool bIsMatchPrefix = false;
    for each (const std::string& strCurPrefix in setMsgPrefix)
    {
        if (boost::algorithm::starts_with(pMsg->GetMsgName(), strCurPrefix))
        {
            bIsMatchPrefix = true;
            break;
        }
    }
    if (!bIsMatchPrefix)
    {
        CAquariusLogger::Logger(LL_DEBUG, "skip this msg name: [%s]", pMsg->GetMsgName().c_str());
        return;
    }
    // 3. Write msg handle function
    ofs << "        // Default handle of msg: " << pMsg->GetMsgName() << std::endl;
    ofs << "        protected virtual bool On" << pMsg->GetMsgName() << "(" << pMsg->GetMsgName() << " msg)" << std::endl;
    ofs << "        {" << std::endl;
    ofs << "            CSLogger.LogDebug(\"ServerMsgHandler: Recv msg: [\" + msg.ToString() + \"]\");" << std::endl;
    ofs << "            return false;" << std::endl;
    ofs << "        }" << std::endl << std::endl;
    // 4. Add msg dispatch function
    char buffMsgDispatch[512] = { 0 };
    sprintf_s(buffMsgDispatch, "if (msg.msg_id == %s.MSG_ID) return On%s((%s)msg);",
        pMsg->GetMsgName().c_str(),
        pMsg->GetMsgName().c_str(),
        pMsg->GetMsgName().c_str());
    ms_vMsgDispatch.push_back(std::string(buffMsgDispatch));
}

void CCSharpMsgHandlerWriter::WriteFunMsgDispatch(std::ofstream& ofs)
{
    // 1. write comment and function name
    ofs << "        // Msg dispatch function " << std::endl;
    ofs << "        protected bool DispatchMsg(Message msg)" << std::endl;
    ofs << "        {" << std::endl;
    ofs << "            if (null == msg)" << std::endl;
    ofs << "            {" << std::endl;
    ofs << "                return false;" << std::endl;
    ofs << "            }" << std::endl;
    // 2. Loop every msg to write dispatch function
    for each (auto& value in ms_vMsgDispatch)
    {
        ofs << "            " << value << std::endl;
    }
    ofs << "            CSLogger.LogError(\"Recv invalid msg id from GateServer: \" + msg.msg_id);" << std::endl;
    ofs << "            return false;" << std::endl;
    ofs << "        }" << std::endl;
}
