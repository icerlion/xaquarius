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
 * FileName: CSharpProtocolWriter.cpp
 * 
 * Purpose:  Write protocol file in c# language.
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/3/5 14:21
===================================================================+*/
#include "CSharpProtocolWriter.h"
#include "AquariusCore/Logger/AquariusLogger.h"
#include "ProtocolHelper/ProtocolParse/MsgTypeDef.h"
#include "ProtocolHelper/ProtocolParse/MsgTypeContainer.h"
#include "ProtocolHelper/ProtocolParse/ProtocolParse.h"
#include "AquariusCore/Util/TinyUtil/RandUtil.h"
#include "AquariusCore/Util/TinyUtil/StringUtil.h"

void CCSharpProtocolWriter::WriteCSProtocolFile( const std::vector<IMsgType*>& vClientMsgList, const char* pszFilePath)
{
    // 1. Open file stream
    std::ofstream ofs;
    ofs.open(pszFilePath, std::fstream::out);
    if (!ofs.is_open())
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "Failed to open file: [%s]", pszFilePath);
        return;
    }
    // 2. Write file header
    CProtocolParse::WriteFileSignature(ofs, "//");
    ofs << "using System;" << std::endl;
    ofs << "using System.Text;" << std::endl;
    ofs << "using System.Security.Cryptography;" << std::endl;
    ofs << "using AINT8 = System.SByte;" << std::endl;
    ofs << "using AUINT8 = System.Byte;" << std::endl;
    ofs << "using AINT16 = System.Int16;" << std::endl;
    ofs << "using AUINT16 = System.UInt16;" << std::endl;
    ofs << "using AINT32 = System.Int32;" << std::endl;
    ofs << "using AUINT32 = System.UInt32;" << std::endl;
    ofs << "using AINT64 = System.Int64;" << std::endl;
    ofs << "using AUINT64 = System.UInt64;" << std::endl;
    ofs << std::endl;
    ofs << "namespace NetIOLayer" << std::endl;
    ofs << "{" << std::endl;
    // 3. Write base msg class.
    WriteBaseMessage(ofs);
    // Loop Every msg to write into file
    bool bResult = true;
    // 4. Loop every msg to write msg to cs file.
    for each (IMsgType* pMsg in vClientMsgList)
    {
        bResult = false;
        const CMsgTypeEnum* pMsgTypeEnum = dynamic_cast<const CMsgTypeEnum*>(pMsg);
        const CMsgTypeStruct* pMsgTypeStruct = dynamic_cast<const CMsgTypeStruct*>(pMsg);
        if (nullptr != pMsgTypeEnum)
        {
            bResult = WriteEnum(ofs, pMsgTypeEnum);
        }
        else if (nullptr != pMsgTypeStruct)
        {
            bResult = WriteStruct(ofs, pMsgTypeStruct);
        }
        else
        {
            CAquariusLogger::Logger(LL_ERROR_FUN, "Invalid msg type:%s", pMsg->GetMsgName());
        }
        if (!bResult)
        {
            break;
        }
    }
    // 5. Write msg factory function
    WriteMessageFactory( ofs, vClientMsgList );
    ofs << "}" << std::endl;
    if (bResult)
    {
        CAquariusLogger::Logger(LL_NOTICE, "Success to write CS file: [%s]", pszFilePath);
    }
    else
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "Failed to write CS file: [%s]", pszFilePath);
    }
    return;
}

void CCSharpProtocolWriter::WriteBaseMessage(std::ofstream& ofs)
{
    ofs << "    // Base class of net message" << std::endl;
    ofs << "    public abstract class Message" << std::endl;
    ofs << "    {" << std::endl;
    ofs << "        public static string " << CProtocolParse::GetVersionName() << " = \"" <<  CProtocolParse::GetProtocolVersion() << "\";" << std::endl;
    ofs << "        public static int MSG_BASIC_LEN = 2;" << std::endl;
    ofs << "        public AUINT16 msg_len;" << std::endl;
    ofs << "        public AUINT16 msg_id;" << std::endl;
    ofs << "        public abstract byte[] ToBytes();" << std::endl;
    ofs << "        public abstract bool FromBytes( MBinaryReader mbr );" << std::endl;
    ofs << "    }" << std::endl << std::endl;
}

bool CCSharpProtocolWriter::WriteEnum( std::ofstream& ofs, const CMsgTypeEnum* pEnum )
{
    ofs << "    // enum: " << pEnum->GetDesc() << std::endl;
    ofs << "    public enum " << pEnum->GetMsgName()  <<  " : short"  << std::endl;
    ofs << "    {" << std::endl;
    const std::vector<EnumField>& vField = pEnum->GetFieldList();
    for each (const EnumField& stField in vField)
    {
        ofs << "        // " << stField.desc << std::endl;
        ofs << "        " << stField.name << " = "  <<  stField.value << "," << std::endl;
    }
    ofs << "    }" << std::endl << std::endl;
    return true;
}

bool CCSharpProtocolWriter::WriteStruct( std::ofstream& ofs, const CMsgTypeStruct* pStruct )
{
    // 1. Check input parameter
    if (nullptr == pStruct)
    {
        return false;
    }
    // 2. Check field list
    const std::vector<StructField>& vFieldList = pStruct->GetFieldList();
    size_t nFieldCount = vFieldList.size();
    if (0 == nFieldCount)
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "No field in struct:%s", pStruct->GetMsgName().c_str());
        return false;
    }
    // 3. Write desc
    ofs << "    // " << pStruct->GetDesc() << ". [size: " << pStruct->GetStructSize() << " ]" << std::endl;
    // 4. Write struct content
    ofs << "    public class " << pStruct->GetMsgName();
    if (pStruct->IsNetPkg())
    {
        ofs << " : Message";
    }
    ofs << std::endl;
    ofs << "    {" << std::endl;
    ofs << "        // Msg id of " << pStruct->GetMsgName() << ", dec value: " << pStruct->GetMsgId() << std::endl;
    ofs << "        public const AUINT16 MSG_LEN = " << pStruct->GetStructSize() << ";" << std::endl;
    if (pStruct->IsNetPkg())
    {
        ofs << "        public const AUINT16 MSG_ID = " << CStringUtil::UInt16ToHexString(pStruct->GetMsgId()) << ";" << std::endl;
    }
    // Write constructor
    WriteConstructor(ofs, pStruct);
    // 6. Write field list
    for each (const StructField& refField in vFieldList)
    {
        // Skip msg id and msg len
        if (0 == refField.name.compare("msg_id") || 0 == refField.name.compare("msg_len"))
        {
            continue;
        }
        // 6.1 Write desc
        ofs << "        // " << refField.desc << std::endl;
        // 6.4 For other field, check simple element count
        if (1 == refField.element_count)
        {
            ofs << "        public " << refField.type << " " << refField.name << ";" << std::endl;
            continue;
        }
        // 6.5 Array type
        ofs << "        private static AUINT16 " << refField.name << "_size = " << refField.element_count << ";" << std::endl;
        if ("char" == refField.type)
        {
            // 6.6 Change char array to string
            ofs << "        public string " << refField.name << ";" << std::endl;
        }
        else
        {
            ofs << "        public " << refField.type << "[] " << refField.name << ";" << std::endl;
        }
    }
    WriteToBytesFun(ofs, pStruct);
    WriteFromBytesFun(ofs, pStruct);
    ofs << "    }" << std::endl << std::endl;
    return true;
}

void CCSharpProtocolWriter::WriteToBytesFun( std::ofstream& ofs, const CMsgTypeStruct* pStruct )
{
    if (nullptr == pStruct)
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "nullptr == pStruct");
        return;
    }
    // Check net pkg flag
    ofs << "        // Write msg to output stream" << std::endl;
    if (pStruct->IsNetPkg())
    {
        ofs << "        public override byte[] ToBytes()" << std::endl;
    }
    else
    {
        ofs << "        public byte[] ToBytes()" << std::endl;
    }
    // Write function body
    ofs << "        {" << std::endl;
    ofs << "            MBinaryWriter mbw = new MBinaryWriter();" << std::endl;
    const std::vector<StructField>& vFieldList = pStruct->GetFieldList();
    // Loop every field to write function.
    for each (const StructField& stField in vFieldList)
    {
        // Write string field
        if ("char" == stField.type && stField.element_count > 1)
        {
            ofs << "            mbw.WriteString( " << stField.name << ", " << stField.name << "_size );" << std::endl;
            continue;
        }
        // Write enum
        if (CMsgTypeContainer::IsEnumTypeName(stField.type))
        {
            if (1 == stField.element_count)
            {
                ofs  <<  "            mbw.Write( (Int16)"  <<  stField.name  <<  ");"  <<  std::endl;
            }
            else
            {
                CAquariusLogger::Logger(LL_ERROR_FUN, "Not support enum array: [%s]", pStruct->GetMsgName());
            }
            continue;
        }
        // Write basic type
        if (CMsgTypeContainer::IsBasicTypeName(stField.type))
        {
            // Write single field
            if ( 1 == stField.element_count)
            {
                ofs << "            mbw.Write( " << stField.name << ");" << std::endl;
            }
            else
            {
                // Write array type
                ofs << "            for (Int32 nElementIndex = 0; nElementIndex < " << stField.name << "_size; ++nElementIndex)" << std::endl;
                ofs << "            {" << std::endl;
                ofs << "                mbw.Write( " << stField.name << "[nElementIndex]);" << std::endl;
                ofs << "            }" << std::endl;
            }
            continue;
        }
        if (CMsgTypeContainer::IsStructTypeName(stField.type))
        {
            // For other struct
            if (1 == stField.element_count)
            {
                ofs << "            byte[] " << stField.name << "_bytes = " << stField.name << ".ToBytes();" << std::endl;
                ofs << "            mbw.Write( " << stField.name << "_bytes );" << std::endl;
            }
            else
            {
                ofs << "            for (Int32 nElementIndex = 0; nElementIndex < " << stField.name << "_size; ++nElementIndex)" << std::endl;
                ofs << "            {" << std::endl;
                ofs << "                byte[] " << stField.name << "_bytes = " << stField.name << "[nElementIndex].ToBytes();" << std::endl;
                ofs << "                mbw.Write( " << stField.name << "_bytes);" << std::endl;
                ofs << "            }" << std::endl;
            }
            continue;
        }
        CAquariusLogger::Logger(LL_ERROR_FUN, "Invalid Struct Field Type: [%s]", pStruct->GetMsgName().c_str());
    }
    // Write end code of the function
    ofs << "            byte[] " << pStruct->GetMsgName() << "_bytes = mbw.ToArray();" << std::endl;
    ofs << "            mbw.Close();" << std::endl;
    ofs << "            mbw = null;" << std::endl;
    ofs << "            // Verify the length of msg " << std::endl;
    ofs << "            if ( " <<  pStruct->GetMsgName()  << "_bytes.Length != " << pStruct->GetStructSize() << " ) " << std::endl;
    ofs << "            {" << std::endl;
    ofs << "                Console.WriteLine(\"Invalid msg len, expect:" << pStruct->GetStructSize() << ", real:" << pStruct->GetMsgName()  << "_bytes.Length\");" << std::endl;
    ofs << "                return null;" << std::endl;
    ofs << "            }" << std::endl;
    ofs << "            return " << pStruct->GetMsgName() << "_bytes;" << std::endl;
    ofs << "        }" << std::endl;
}

void CCSharpProtocolWriter::WriteFromBytesFun( std::ofstream& ofs, const CMsgTypeStruct* pStruct )
{
    if (nullptr == pStruct)
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "nullptr == pStruct");
        return;
    }
    ofs << "        // Read msg to input stream" << std::endl;
    if (pStruct->IsNetPkg())
    {
        ofs << "        public override bool FromBytes( MBinaryReader mbr )" << std::endl;
    }
    else
    {
        ofs << "        public bool FromBytes( MBinaryReader mbr )" << std::endl;
    }
    
    ofs << "        {" << std::endl;
    ofs << "            // Skip msg_id and msg_len as it has been read by outside." << std::endl;
    const std::vector<StructField>& vFieldList = pStruct->GetFieldList();
    for each (const StructField& stField in vFieldList )
    {
        // Skip the msg id and msg len
        if (0 == stField.name.compare("msg_id") || 0 == stField.name.compare("msg_len"))
        {
            continue;
        }
        // Read string field
        if ("char" == stField.type && stField.element_count > 1)
        {
            ofs << "            " << stField.name << " = mbr.ReadString( " << stField.name << "_size );" << std::endl;
            continue;
        }
        // Write enum
        if (CMsgTypeContainer::IsEnumTypeName(stField.type))
        {
            if (1 == stField.element_count)
            {
                ofs  <<  "            "  <<  stField.name  <<  " = ("  <<  stField.type  <<  ")mbr.ReadInt16();"  <<  std::endl;
            }
            else
            {
                CAquariusLogger::Logger(LL_ERROR_FUN, "Not support enum array in msg: [%s]", pStruct->GetMsgName().c_str());
            }
            continue;
        }
        // Read basic type
        if (CMsgTypeContainer::IsBasicTypeName(stField.type))
        {
            // Write single field
            std::string strReadFun = GetReadFunction(stField.type);
            if ( 1 == stField.element_count)
            {
                ofs << "            " << stField.name << " = mbr." << strReadFun << "();" << std::endl;
            }
            else
            {
                // Write array type
                ofs << "            for (Int32 nElementIndex = 0; nElementIndex < " << stField.name << "_size; ++nElementIndex)" << std::endl;
                ofs << "            {" << std::endl;
                ofs << "                " << stField.name << "[nElementIndex] = mbr." << strReadFun << "();" << std::endl;
                ofs << "            }" << std::endl;
            }
            continue;
        }
        if (CMsgTypeContainer::IsStructTypeName(stField.type))
        {
            // For other struct
            if (1 == stField.element_count)
            {
                ofs << "            " << stField.name << ".FromBytes(mbr);" << std::endl;
            }
            else
            {
                ofs << "            for (Int32 nElementIndex = 0; nElementIndex < " << stField.name << "_size; ++nElementIndex)" << std::endl;
                ofs << "            {" << std::endl;
                ofs << "                " << stField.name << "[nElementIndex].FromBytes(mbr);" << std::endl;
                ofs << "            }" << std::endl;
            }
            continue;
        }
        CAquariusLogger::Logger(LL_ERROR_FUN, "Invalid FieldType in Msg: [%s]", pStruct->GetMsgName().c_str());
    }
    if (pStruct->IsNetPkg())
    {
        ofs << "            if (msg_id != MSG_ID)" << std::endl;
        ofs << "            {" << std::endl;
        ofs << "                Console.WriteLine(\"Invalid msg id when parse msg type [" << pStruct->GetMsgName() << "], expect:[" << pStruct->GetMsgId() << "], real:[\" + msg_id + \"]\");" << std::endl;
        ofs << "                return false;" << std::endl;
        ofs << "            }" << std::endl;
        std::string strMsgLen = CStringUtil::Int32ToDecString((int)pStruct->GetStructSize());
        ofs << "            if (msg_len != MSG_LEN)" << std::endl;
        ofs << "            {" << std::endl;
        ofs << "                Console.WriteLine(\"Invalid msg len when parse msg type [" << pStruct->GetMsgName() << "], expect:[" << pStruct->GetStructSize() << "], real:[\" + msg_len + \"]\");" << std::endl;
        ofs << "                return false;" << std::endl;
        ofs << "            }" << std::endl;
    }
    ofs << "            return true;" << std::endl;
    ofs << "        }" << std::endl;
}

std::string CCSharpProtocolWriter::GetReadFunction( const std::string& strTypeName )
{
    if (0 == strTypeName.compare("char")) { return "ReadChar"; }
    if (0 == strTypeName.compare("AINT8")) { return "ReadSByte"; }
    if (0 == strTypeName.compare("AUINT8")) { return "ReadByte"; }
    if (0 == strTypeName.compare("AINT16")) { return "ReadInt16"; }
    if (0 == strTypeName.compare("AUINT16")) { return "ReadUInt16"; }
    if (0 == strTypeName.compare("AINT32")) { return "ReadInt32"; }
    if (0 == strTypeName.compare("AUINT32")) { return "ReadUInt32"; }
    CAquariusLogger::Logger(LL_ERROR_FUN, "Invalid basic type:%s", strTypeName.c_str());
    return "";
}


void CCSharpProtocolWriter::WriteConstructor( std::ofstream& ofs, const CMsgTypeStruct* pStruct )
{
    ofs << "        // Constructor" << std::endl;
    ofs << "        public " << pStruct->GetMsgName() << "()" << std::endl;
    ofs << "        {" << std::endl;
    const std::vector<StructField>& vFieldList = pStruct->GetFieldList();
    for each (const StructField& stField in vFieldList)
    {
        if (0 == stField.name.compare("msg_id"))
        {
            ofs << "            msg_id = MSG_ID;" << std::endl;
            continue;
        }
        if (0 == stField.name.compare("msg_len"))
        {
            ofs << "            msg_len = MSG_LEN;" << std::endl;
            continue;
        }
        if (0 == stField.type.compare("char") && stField.element_count > 1)
        {
            ofs << "            " << stField.name << " = \"\";" << std::endl;
            continue;
        }
        if (CMsgTypeContainer::IsEnumTypeName(stField.type))
        {
            if (1 == stField.element_count)
            {
                ofs << "            " << stField.name << " = (" << stField.type << ") 0;" << std::endl;
            }
            else
            {
                CAquariusLogger::Logger(LL_ERROR_FUN, "Not support enum array, struct:%s", pStruct->GetMsgName().c_str());
            }
            continue;
        }
        if (CMsgTypeContainer::IsBasicTypeName(stField.type))
        {
            if (1 == stField.element_count)
            {
                if ("char" == stField.type)
                {
                    ofs << "            " << stField.name << " = '\\0';" << std::endl;
                }
                else
                {
                    ofs << "            " << stField.name << " = 0;" << std::endl;
                }
                
            }
            else
            {
                ofs << "            " << stField.name << " = new " << stField.type << "[ " << stField.name << "_size ];" << std::endl;
            }
            continue;
        }
        if (CMsgTypeContainer::IsStructTypeName(stField.type))
        {
            // Common msg type
            if (1 == stField.element_count)
            {
                ofs << "            " << stField.name << " = new " << stField.type << "();" << std::endl;
            }
            else
            {
                ofs << "            " << stField.name << " = new " << stField.type << "[ " << stField.name << "_size ];" << std::endl;
                ofs << "            for (int element_index = 0; element_index < " << stField.name << ".Length; ++element_index)" << std::endl;
                ofs << "            {" << std::endl;
                ofs << "                 " << stField.name << "[element_index] = new " << stField.type << "();" << std::endl;
                ofs << "            }" << std::endl;
            }
            continue;
        }
        CAquariusLogger::Logger(LL_ERROR_FUN, "Invalid Struct Field Type: [%s]", pStruct->GetMsgName().c_str());
    }
    ofs << "        }" << std::endl << std::endl;
}

void CCSharpProtocolWriter::WriteMessageFactory( std::ofstream& ofs, const std::vector<IMsgType*>& vClientMsgList )
{
    ofs << "    // Message factory class" << std::endl;
    ofs << "    public class MessageFactory" << std::endl;
    ofs << "    {" << std::endl;
    ofs << "        public static Message CreateMessageById(AUINT16 nMsgId)" << std::endl;
    ofs << "        {" << std::endl;
    ofs << "            Message msg = null;" << std::endl;
    ofs << "            switch(nMsgId)" << std::endl;
    ofs << "            {" << std::endl;
    for each (auto& value in vClientMsgList)
    {
        const CMsgTypeStruct* pStruct = dynamic_cast<const CMsgTypeStruct*>(value);
        if (nullptr == pStruct || !pStruct->IsNetPkg())
        {
            continue;
        }
        ofs << "            case " << pStruct->GetMsgName() << ".MSG_ID:" << std::endl;
        ofs << "                msg = new " << pStruct->GetMsgName() << "();" << std::endl;
        ofs << "                break;" << std::endl;
    }
    ofs << "            default:" << std::endl;
    ofs << "                Console.WriteLine(\"Invalid msg id:\" + nMsgId);" << std::endl;
    ofs << "                break;" << std::endl;
    ofs << "            }" << std::endl;
    ofs << "            return msg;" << std::endl;
    ofs << "        }" << std::endl;
    ofs << "    }" << std::endl << std::endl;
}
