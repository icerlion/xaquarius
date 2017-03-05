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
 * FileName: CSharpMsgHandlerWriter.h
 * 
 * Purpose:  Write msg handle function using C# language
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/3/5 14:20
===================================================================+*/
#ifndef _CSHARP_MSG_HANDLER_WRITER_H_
#define _CSHARP_MSG_HANDLER_WRITER_H_
#include <fstream>
#include "ProtocolHelper/ProtocolParse/MsgTypeDef.h"

class CCSharpMsgHandlerWriter
{
public:
    // Write client msg to .cs file
    static void WriteCSMsgHandlerFile(const std::vector<IMsgType*>& vClientMsgList, const char* pszFilePath, const std::set<std::string>& setMsgPrefix );

private:
    // Write msg handle for a msg
    static void WriteMsgHandler(const CMsgTypeStruct* pMsg, std::ofstream& ofs, const std::set<std::string>& setMsgPrefix);

    // Write msg handle function
    static void WriteFunMsgDispatch(std::ofstream& ofs);

private:
    // Msg dispatch function list
    static std::vector<std::string> ms_vMsgDispatch;

};

#endif // _CSHARP_MSG_HANDLER_WRITER_H_
