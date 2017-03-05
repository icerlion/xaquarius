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
 * FileName: CSharpProtocolWriter.h
 * 
 * Purpose:  Write protocol using C# language
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/3/5 14:21
===================================================================+*/
#ifndef _CSHARP_PROTOCOL_WRITER_H_
#define _CSHARP_PROTOCOL_WRITER_H_

#include "ProtocolHelper/ProtocolParse/MsgTypeDef.h"
#include <fstream>
#include <vector>

class CCSharpProtocolWriter
{
public:
    // Write client msg to CShape file
    static void WriteCSProtocolFile(const std::vector<IMsgType*>& vClientMsgList, const char* pszFilePath);

private:
    // Write the base msg class in cs file.
    static void WriteBaseMessage(std::ofstream& ofs);

    // Write message factory for all client msg
    static void WriteMessageFactory(std::ofstream& ofs, const std::vector<IMsgType*>& vClientMsgList);

    // Write enum struct to cs file.
    static bool WriteEnum( std::ofstream& ofs, const CMsgTypeEnum* pEnum );

    // Write struct to cs file
    static bool WriteStruct( std::ofstream& ofs, const CMsgTypeStruct* pStruct );

    // Write ToBytes function for a struct
    static void WriteToBytesFun( std::ofstream& ofs, const CMsgTypeStruct* pStruct );

    // Write FromBytes function for a struct
    static void WriteFromBytesFun( std::ofstream& ofs, const CMsgTypeStruct* pStruct );

    // Get the read function name for a type
    static std::string GetReadFunction(const std::string& strTypeName);

    // Write constructor function for a struct
    static void WriteConstructor( std::ofstream& ofs, const CMsgTypeStruct* pStruct );
};

#endif // _CSHARP_PROTOCOL_WRITER_H_
