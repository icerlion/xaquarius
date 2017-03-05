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
 * FileName: CPlusPlusWriter.h
 * 
 * Purpose:  Write hpp file in c-plus-plus lang
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/3/5 14:20
===================================================================+*/
#ifndef _CPLUSPLUS_WRITER_H_
#define _CPLUSPLUS_WRITER_H_

#include <vector>
#include <string>
#include <fstream>
#include "ProtocolHelper/ProtocolParse/MsgTypeDef.h"

class CCPlusPlusWriter
{
public:
    // Write to hpp file
    static bool WriteToHpp(const char* pszFilePath, const std::vector<IMsgType*>& vMsgList, const std::vector<std::string>& vIncludeFile, bool bWriteProtocolVersion );

private:
    // Generate file protected macro by hpp file name
    static std::string GenerateFileMacro( std::string strHPPFileName);

    // Write Enum into file
    static bool WriteEnum(std::ofstream& ofs, const CMsgTypeEnum* pMsgpEnum);

    // Write enum to string function
    static void WriteEnumToCStrFunction(std::ofstream& ofs, const CMsgTypeEnum* pEnum);

    // Write Struct into file
    static bool WriteStruct(std::ofstream& ofs, const CMsgTypeStruct* pStruct);

    // Write constructor for struct
    static void WriteStructConstructor(std::ofstream& ofs, const CMsgTypeStruct* pStruct);

    // Write initialize list for basic member
    static void WriteMemberInitializeList(std::ofstream& ofs, const CMsgTypeStruct* pStruct);

    // Write order function for send/receive
    static void WriteStructOrderFun(std::ofstream& ofs, const CMsgTypeStruct* pStruct, bool bToNet);

    // Get byte order function name
    static std::string GetByteOrderFunName(const StructField& stField, bool bToNet);

    // Write function ot ValueToCStr
    static bool WriteFunValueToCStr(std::ofstream& ofs, const CMsgTypeStruct* pStruct);

};

#endif // _CPLUSPLUS_WRITER_H_
