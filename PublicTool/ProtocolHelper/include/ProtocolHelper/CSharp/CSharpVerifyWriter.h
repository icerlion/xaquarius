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
 * FileName: CSharpVerifyWriter.h
 * 
 * Purpose:  Write verify file for the msg in c# language.
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/3/5 14:19
===================================================================+*/
#ifndef _CSHARP_VERIFY_WRITER_H_
#define _CSHARP_VERIFY_WRITER_H_
#include "ProtocolHelper/ProtocolParse/MsgTypeDef.h"
#include <fstream>
#include <vector>

class CCSharpVerifyWriter
{
public:
    // Write verify file for client msg list.
    static void WriteCSVerifyFile(const std::vector<IMsgType*>& vClientMsgList, const char* pszFilePath);

private:
    // Write verify function for a struct
    static void WriteStructVerifyFun(std::ofstream& ofs, const CMsgTypeStruct* pStruct);

    // Assign value for a struct by rand function
    static void AssignRandValueForObject(std::ofstream& ofs, const CMsgTypeStruct* pStruct);

    // Compare struct file.
    static void CompareBasicTypeField(std::ofstream& ofs, const CMsgTypeStruct* pStruct);

    // Compare struct by bytes stream
    static void CompareStructByBytes(std::ofstream& ofs, const CMsgTypeStruct* pStruct);

    // Write function of RunTest to cs file.
    static void WriteRunTestFun(std::ofstream& ofs);

private:
    // function name list
    static std::vector<std::string> ms_vVerifyFun;
};

#endif // _CSHARP_VERIFY_WRITER_H_
