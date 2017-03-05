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
 * FileName: CSharpVerifyWriter.cpp
 * 
 * Purpose:  Write verify function in cs file.
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/3/5 14:20
===================================================================+*/
#include "CSharpVerifyWriter.h"
#include "AquariusCore/Logger/AquariusLogger.h"
#include "AquariusCore/Util/TinyUtil/CommonUtil.h"
#include "AquariusCore/Util/TinyUtil/RandUtil.h"
#include "ProtocolHelper/ProtocolParse/MsgTypeContainer.h"
#include "ProtocolHelper/ProtocolParse/ProtocolParse.h"

std::vector<std::string> CCSharpVerifyWriter::ms_vVerifyFun;

void CCSharpVerifyWriter::WriteCSVerifyFile( const std::vector<IMsgType*>& vClientMsgList, const char* pszFilePath )
{
    // Open file stream
    std::ofstream ofs(pszFilePath);
    if (!ofs.is_open())
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "Failed to open file: [%s]", pszFilePath);
        return;
    }
    // Write file header
    CProtocolParse::WriteFileSignature(ofs, "//");
    ofs<<"using System;"<<std::endl<<std::endl;
    ofs<<"namespace NetIOLayer"<<std::endl;
    ofs<<"{"<<std::endl;
    ofs<<"    class VerifyProtocolParse"<<std::endl;
    ofs<<"    {"<<std::endl;
    // Loop every client msg to write verify function
    for each (const IMsgType* pMsg in vClientMsgList)
    {
        const CMsgTypeStruct* pStruct = dynamic_cast<const CMsgTypeStruct*>(pMsg);
        if (nullptr != pStruct)
        {
            WriteStructVerifyFun(ofs, pStruct);
        }
    }
    // Write run test function
    WriteRunTestFun(ofs);
    ofs<<"    }"<<std::endl;
    ofs<<"}"<<std::endl;
    CAquariusLogger::Logger(LL_NOTICE, "Success to write CS file: [%s]", pszFilePath);
}

void CCSharpVerifyWriter::AssignRandValueForObject(std::ofstream& ofs, const CMsgTypeStruct* pStruct)
{
    ofs<<"            // Make object rand"<<std::endl;
    // Rand object
    const std::vector<StructField>& vFieldList = pStruct->GetFieldList();
    for each (const StructField& stField in vFieldList)
    {
        // Skip msg id and msg len
        if (0 == stField.name.compare("msg_id") || 0 == stField.name.compare("msg_len"))
        {
            continue;
        }
        // if the field was char, just assign as "test"
        if (0 == stField.type.compare("char") && 1 != stField.element_count)
        {
            ofs<<"            stSrc."<<stField.name<<" = \"test\";"<<std::endl;
            continue;
        }
        // Init basic type
        if ( CMsgTypeContainer::IsBasicTypeName(stField.type) && 1 == stField.element_count)
        {
            if ("char" == stField.type)
            {
                ofs<<"            stSrc."<<stField.name<<" = 'a';"<<std::endl;
            }
            else
            {
                ofs<<"            stSrc."<<stField.name<<" = "<<CRandUtil::RandNumber(128)<<";"<<std::endl;
            }
        }
    }
}

void CCSharpVerifyWriter::CompareBasicTypeField(std::ofstream& ofs, const CMsgTypeStruct* pStruct)
{
    // Compare basic type field.
    ofs<<"            // Verify object content "<<std::endl;
    const std::vector<StructField>& vFieldList = pStruct->GetFieldList();
    // Compare every field.
    for each (const StructField& stField in vFieldList)
    {
        if (1 != stField.element_count)
        {
            continue;
        }
        if (CMsgTypeContainer::IsStructTypeName(stField.type))
        {
            continue;
        }
        if (0 == stField.name.compare("msg_id"))
        {
            ofs<<"            if ( stDst.msg_id != "<<pStruct->GetMsgName()<<".MSG_ID )"<<std::endl;
        }
        else if (0 == stField.name.compare("msg_len"))
        {
            ofs<<"            if ( stDst.msg_len != "<<pStruct->GetMsgName()<<".MSG_LEN )"<<std::endl;
        }
        else
        {
            ofs<<"            if (stSrc."<<stField.name<<" != stDst."<<stField.name<<")"<<std::endl;
        }
        ofs<<"            {"<<std::endl;
        ofs<<"                Console.WriteLine(\"Failed to verify field: "<<stField.name<<"\");"<<std::endl;
        ofs<<"            }"<<std::endl;
    }
}

void CCSharpVerifyWriter::WriteStructVerifyFun( std::ofstream& ofs, const CMsgTypeStruct* pStruct )
{
    // Write verify function for a struct
    std::string strFunName = "Verify_" + pStruct->GetMsgName();
    ms_vVerifyFun.push_back(strFunName);
    ofs<<"        // Verify msg type: "<<pStruct->GetMsgName()<<std::endl;
    ofs<<"        private static void "<<strFunName<<"()"<<std::endl;
    ofs<<"        {"<<std::endl;
    ofs<<"            "<<pStruct->GetMsgName()<<" stSrc = new "<<pStruct->GetMsgName()<<"();"<<std::endl;
    // Build the struct object by rand function
    AssignRandValueForObject(ofs, pStruct);
    ofs<<"            byte[] src_bytes = stSrc.ToBytes();"<<std::endl;
    ofs<<"            MBinaryReader mbr = new MBinaryReader(src_bytes);"<<std::endl;
    ofs<<"            "<<pStruct->GetMsgName()<<" stDst = new "<<pStruct->GetMsgName()<<"();"<<std::endl;
    if (pStruct->IsNetPkg())
    {
        ofs<<"            stDst.msg_len = mbr.ReadUInt16();"<<std::endl;
        ofs<<"            stDst.msg_id = mbr.ReadUInt16();"<<std::endl;
    }
    ofs<<"            stDst.FromBytes(mbr);"<<std::endl;
    // Compare the struct
    CompareBasicTypeField(ofs, pStruct);
    CompareStructByBytes(ofs, pStruct);
    ofs<<"        }"<<std::endl<<std::endl;
}

void CCSharpVerifyWriter::CompareStructByBytes(std::ofstream& ofs, const CMsgTypeStruct* pStruct)
{
    // Compare struct by bytes.
    ofs<<"            // Compare object by bytes "<<std::endl;
    ofs<<"            byte[] dst_bytes = stDst.ToBytes();"<<std::endl;
    ofs<<"            if ( dst_bytes.Length != src_bytes.Length)"<<std::endl;
    ofs<<"            {"<<std::endl;
    ofs<<"                Console.WriteLine(\"Failed to verify field: "<<pStruct->GetMsgName()<<" by bytes length\");"<<std::endl;
    ofs<<"            }"<<std::endl;
    ofs<<"            for ( int byte_index = 0; byte_index < dst_bytes.Length; ++byte_index)"<<std::endl;
    ofs<<"            {"<<std::endl;
    ofs<<"                if (src_bytes[byte_index] != dst_bytes[byte_index])"<<std::endl;
    ofs<<"                {"<<std::endl;
    ofs<<"                    Console.WriteLine(\"Failed to verify field: "<<pStruct->GetMsgName()<<" by bytes length\");"<<std::endl;
    ofs<<"                }"<<std::endl;
    ofs<<"            }"<<std::endl;
}

void CCSharpVerifyWriter::WriteRunTestFun( std::ofstream& ofs )
{
    // Write the RunTest function to call all verify function
    ofs<<"        // Run verify function "<<std::endl;
    ofs<<"        public static void RunTest()"<<std::endl;
    ofs<<"        {"<<std::endl;
    // Call all function
    for each (auto& value in ms_vVerifyFun)
    {
        ofs<<"            "<< value <<"();"<<std::endl;
    }
    ofs<<"        }"<<std::endl;
}
