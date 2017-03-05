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
 * FileName: CPlusPlusWriter.cpp
 * 
 * Purpose:  Implement CPlusPlusWriter
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/3/5 14:20
===================================================================+*/
#include "CPlusPlusWriter.h"
#include "AquariusCore/Logger/AquariusLogger.h"
#include "AquariusCore/Util/TinyUtil/StringUtil.h"
#include "AquariusCore/Util/TinyUtil/TimeUtil.h"
#include "ProtocolHelper/CPlusPlus/MsgUtilWriter.h"
#include "ProtocolHelper/ProtocolParse/MsgTypeContainer.h"
#include "ProtocolHelper/ProtocolParse/ProtocolParse.h"

bool CCPlusPlusWriter::WriteToHpp(const char* pszFilePath, const std::vector<IMsgType*>& vMsgList, const std::vector<std::string>& vIncludeFile, bool bWriteProtocolVersion)
{
    // 1. Check msg count
    if (vMsgList.empty())
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "No msg type is parsed");
        return false;
    }
    // 2. Begin to write hpp file
    bool bResult = true;
    std::ofstream ofs(pszFilePath);
    if (!ofs.is_open())
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "Failed to open file: [%s]", pszFilePath);
        return false;
    }
    // 2.1 Write file protected macro
    CProtocolParse::WriteFileSignature(ofs, "//");
    std::string strFileMacro = GenerateFileMacro(pszFilePath);
    ofs << "#ifndef " << strFileMacro << std::endl;
    ofs << "#define " << strFileMacro << std::endl << std::endl;
    // 2.2 Write include file
    for each (auto& value in vIncludeFile)
    {
        ofs << "#include \"" << value << "\"" << std::endl;
    }
    ofs << std::endl;
    if (bWriteProtocolVersion)
    {
        ofs << "const static std::string " << CProtocolParse::GetVersionName() << " = \"" << CProtocolParse::GetProtocolVersion() << "\";" << std::endl << std::endl;
    }
    // 2.3 Push package
    ofs << "#pragma pack(push)" << std::endl;
    ofs << "#pragma pack(1)" << std::endl;
    // 2.4 Loop Every msg to write into file
    for each (auto& pMsg in vMsgList)
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
            CAquariusLogger::Logger(LL_ERROR_FUN, "Msg Pointer is nullptr");
        }
        if (!bResult)
        {
            break;
        }
    }
    // 2.5 End of write file
    ofs << "#pragma pack(pop)" << std::endl;
    ofs << "#endif // " << strFileMacro << std::endl;
    ofs.close();
    return bResult;
}

std::string CCPlusPlusWriter::GenerateFileMacro( std::string strHPPFileName )
{
    size_t nPos = strHPPFileName.rfind('/');
    if (std::string::npos != nPos)
    {
        strHPPFileName = strHPPFileName.substr(nPos + 1, strHPPFileName.length());
    }
    std::string strMacro = "_";
    strMacro.append(strHPPFileName);
    strMacro[strHPPFileName.length() - 3] = '_';
    strMacro.append("_");
    std::transform(strMacro.begin(), strMacro.end(), strMacro.begin(), toupper);
    return strMacro;
}

bool CCPlusPlusWriter::WriteEnum( std::ofstream& ofs, const CMsgTypeEnum* pEnum)
{
    // 1. Check input parameter
    if (nullptr == pEnum)
    {
        return false;
    }
    // 2. Check field list
    const std::vector<EnumField>& vFieldList = pEnum->GetFieldList();
    if (vFieldList.empty())
    {
        return false;
    }
    // 3. Try to write desc.
    ofs << "// " << pEnum->GetDesc() << std::endl;
    ofs << "enum class " << pEnum->GetMsgName() << " : short " << std::endl;
    ofs << "{" << std::endl;
    // 4. Write field list
    for each (const EnumField& refField in vFieldList)
    {
        ofs << "    // " << refField.desc << std::endl;
        ofs << "    " << refField.name << " = " <<  refField.value << ","  << std::endl;
    }
    ofs << "};" << std::endl << std::endl;
    // 5. Write Enum to string function
    WriteEnumToCStrFunction(ofs, pEnum);
    return true;
}

void CCPlusPlusWriter::WriteEnumToCStrFunction(std::ofstream& ofs, const CMsgTypeEnum* pEnum)
{
    // Write the function, convert Enum to string
    ofs << "// ToCStr function for type " << pEnum->GetMsgName() << std::endl;
    ofs << "static inline const char* " << pEnum->GetMsgName() << "ToCStr( " << pEnum->GetMsgName() << " nCode )" << std::endl;
    ofs << "{" << std::endl;
    const std::vector<EnumField>& vFieldList = pEnum->GetFieldList();
    for each (const EnumField& refField in vFieldList)
    {
        ofs <<  "    if (" << pEnum->GetMsgName() << "::" << refField.name << " == nCode) { return \"" << refField.name << "\"; }" << std::endl;
    }
    // Process default condition
    ofs <<  "    CAquariusLogger::Logger(LL_ERROR_FUN, \"Invalid enum value: %d\", nCode);" << std::endl;
    ofs <<  "    static char buff[64] = { 0 };" << std::endl;
    ofs <<  "    a_sprintf(buff, \"%hd\", nCode);" << std::endl;
    ofs <<  "    return buff;" << std::endl;
    ofs <<  "}" << std::endl << std::endl;
}

bool CCPlusPlusWriter::WriteStruct( std::ofstream& ofs, const CMsgTypeStruct* pStruct)
{
    // 1. Check input parameter
    if (nullptr == pStruct)
    {
        return false;
    }
    // 2. Check field list
    const std::vector<StructField>& vFieldList = pStruct->GetFieldList();
    if (vFieldList.empty())
    {
        return false;
    }
    // 3. Write desc if it has value
    ofs << "// " <<  pStruct->GetDesc()  << ". [size: " << pStruct->GetStructSize() << " ]" << std::endl;
    const std::string& strName = pStruct->GetMsgName();
    // 4. Write msg id if it was net package
    if (pStruct->IsNetPkg())
    {
        char buff[32] = { 0 };
        sprintf_s(buff, "0x%04x", pStruct->GetMsgId());
        ofs << "static const unsigned short " << strName << "_Id = " << buff << ";" << std::endl;
    }
    // 5. Write struct define
    ofs << "typedef struct " << strName << std::endl;
    ofs << "{" << std::endl;
    // 6. Write msg id if it was net package
    if (pStruct->IsNetPkg())
    {
        // Get msg id
        ofs << "    // Get id of this msg" << std::endl;
        ofs << "    static inline const unsigned short GetMsgId()" << std::endl;
        ofs << "    {" << std::endl;
        ofs << "        return " << strName << "_Id; " << std::endl;
        ofs << "    }" << std::endl;
        // Get watch flag
        ofs << "    // Get inline monitor flag this msg" << std::endl;
        ofs << "    static inline bool GetMonitorFlag()" << std::endl;
        ofs << "    {" << std::endl;
        ofs << "        return " << pStruct->GetMonitorFlag() << "; " << std::endl;
        ofs << "    }" << std::endl;
    }
    // 7. Get msg name
    ofs << "    // Get msg name" << std::endl;
    ofs << "    static inline const char* GetMsgName()" << std::endl;
    ofs << "    {" << std::endl;
    ofs << "        return \"" << pStruct->GetMsgName() << "\"; " << std::endl;
    ofs << "    }" << std::endl;
    // 8. Write constructor
    WriteStructConstructor(ofs, pStruct);
    // 9. Write every field of the struct
    for each ( const StructField& refField in vFieldList)
    {
        ofs << "    // " << refField.desc << std::endl;
        ofs << "    " << refField.type << " " << refField.name;
        if (refField.element_count > 1)
        {
            ofs << "[" << refField.element_count << "]";
        }
        ofs << ";" << std::endl;
    }
    // 10. Write ToNetOrder function for this struct
    WriteStructOrderFun(ofs, pStruct, true);
    WriteStructOrderFun(ofs, pStruct, false);
    WriteFunValueToCStr(ofs, pStruct);
    // 11. End of write struct
    ofs << "} " << strName << ";" << std::endl;
    // 12. Check msg size
    ofs << "static_assert( sizeof( " << strName << " ) == " <<  pStruct->GetStructSize() << ", \"Check struct size failed for ["  << strName  << "]\");"  << std::endl << std::endl;
    // 13. Update msg struct
    CMsgUtilWriter::UpdateMsgUtilHpp(pStruct);
    return true;
}

void CCPlusPlusWriter::WriteStructConstructor( std::ofstream& ofs, const CMsgTypeStruct* pStruct )
{
    if (nullptr == pStruct)
    {
        return;
    }
    // 1. Write constructor comment
    const std::string& strName = pStruct->GetMsgName();
    ofs << "    // Constructor of " << strName << std::endl;
    ofs << "    " << strName << "()" << std::endl;
    // 2. Write init list
    WriteMemberInitializeList(ofs, pStruct);
    ofs << "    {" << std::endl;
    // 3. Init array data member
    const std::vector<StructField>& vStructField = pStruct->GetFieldList();
    for each (const StructField& stStructField in vStructField)
    {
        if (stStructField.element_count > 1)
        {
            ofs << "        memset(" << stStructField.name << ", 0, sizeof(" << stStructField.name << "));" << std::endl;
        }
    }
    ofs << "    }" << std::endl;
}

void CCPlusPlusWriter::WriteMemberInitializeList( std::ofstream& ofs, const CMsgTypeStruct* pStruct )
{
    // 1. Check parameter
    if (nullptr == pStruct)
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "nullptr == pStruct");
        return;
    }
    // 2. Filter basic field
    const std::vector<StructField>& vFieldList = pStruct->GetFieldList();
    std::vector<StructField> vBasicFieldList;
    for each (const StructField& stStructField in vFieldList )
    {
        // Skip struct and array
        if (!CMsgTypeContainer::IsStructTypeName(stStructField.type) && 1 == stStructField.element_count)
        {
            vBasicFieldList.push_back(stStructField);
        }
    }
    // 3. Init every field of the msg
    bool bIsFirstInitField = true;
    int nFieldIndex = -1;
    for each (auto& refField in vBasicFieldList)
    {
        ++nFieldIndex;
        // 2.2 Write init value of the current data member
        if (bIsFirstInitField)
        {
            ofs << "        :" << refField.name << "(" << refField.init_value << ")";
            bIsFirstInitField = false;
        }
        else
        {
            ofs << "        " << refField.name << "(" << refField.init_value << ")";
        }
        // 2.4 Write "," if it was not the last member 
        if (vBasicFieldList.size() -1 != nFieldIndex)
        {
            ofs << ",";
        }
        ofs << std::endl;
    }
}

void CCPlusPlusWriter::WriteStructOrderFun( std::ofstream& ofs, const CMsgTypeStruct* pStruct, bool bToNet )
{
    // 1. Check parameter
    if (nullptr == pStruct)
    {
        return;
    }
    // 2. Write comment and function by the input parameter: bToNet
    if (bToNet)
    {
        ofs << "    // To net byte order for " << pStruct->GetMsgName() << std::endl;
        ofs << "    inline void HostToNetStruct()" << std::endl;
    }
    else
    {
        ofs << "    // To host byte order for " << pStruct->GetMsgName() << std::endl;
        ofs << "    inline void NetToHostStruct()" << std::endl;
    }
    ofs << "    {" << std::endl;
    // 3. Get the field list of the struct
    const std::vector<StructField>& vFieldList = pStruct->GetFieldList();
    // 4. Loop every field.
    for each (auto& stField in vFieldList)
    {
        const MsgTypeInfo& stMsgTypeInfo = CMsgTypeContainer::GetMsgTypeInfo(stField.type);
        // 4.1 Check the field type
        if (EMsgTypeCateGory::Invalid == stMsgTypeInfo.type)
        {
            CAquariusLogger::Logger(LL_ERROR_FUN, "Invalid type:[%s]", stField.type.c_str());
            continue;;
        }
        // 4.2 Get order function name
        std::string strFunName = GetByteOrderFunName(stField, bToNet);
        if (strFunName.empty())
        {
            continue;
        }
        if (1 == stField.element_count)
        {
            // 4.3 Process no-array field.
            if (EMsgTypeCateGory::TypeStruct == stMsgTypeInfo.type)
            {
                ofs << "        " << stField.name << "." << strFunName << "();" << std::endl;
            }
            else if (EMsgTypeCateGory::TypeEnum == stMsgTypeInfo.type)
            {
                ofs << "        " << stField.name << " = " << strFunName << "((short)" << stField.name << ");" << std::endl;
            }
            else
            {
                ofs << "        " << stField.name << " = " << strFunName << "(" << stField.name << ");" << std::endl;
            }
        }
        else
        {
            // 4.4 Process array field.
            ofs << "        for( size_t nIndex = 0; nIndex < A_ARRAY_SIZE(" << stField.name << "); ++nIndex)" << std::endl;
            ofs << "        {" << std::endl;
            if (EMsgTypeCateGory::TypeStruct == stMsgTypeInfo.type)
            {
                ofs << "            " << stField.name << "[nIndex]." << strFunName << "();" << std::endl;
            }
            else
            {
                ofs << "            " << stField.name << "[nIndex] = " << strFunName << "(" << stField.name << "[nIndex]);" << std::endl;
            }
            ofs << "        }" << std::endl;
        }
    }
    ofs << "    }" << std::endl;
}

std::string CCPlusPlusWriter::GetByteOrderFunName( const StructField& stField, bool bToNet )
{
    // Get type info
    const MsgTypeInfo& stMsgTypeInfo = CMsgTypeContainer::GetMsgTypeInfo(stField.type);
    std::string strFunName = "";
    if (EMsgTypeCateGory::TypeBasic == stMsgTypeInfo.type)
    {
        // Get order function name for basic type
        if (bToNet)
        {
            strFunName = stMsgTypeInfo.net_order_fun;
        }
        else
        {
            strFunName = stMsgTypeInfo.host_order_fun;
        }
    }
    else if (EMsgTypeCateGory::TypeEnum == stMsgTypeInfo.type)
    {
        // Get order function name for enum type
        if (bToNet)
        {
            strFunName = "(" + stField.type + ")CCommonUtil::HostToNet16";
        }
        else
        {
            strFunName = "(" + stField.type + ")CCommonUtil::NetToHost16";
        }
    }
    else if (EMsgTypeCateGory::TypeStruct == stMsgTypeInfo.type)
    {
        // Get order function name for struct type
        if (bToNet)
        {
            strFunName = "HostToNetStruct";
        }
        else
        {
            strFunName = "NetToHostStruct";
        }
    }
    else
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "stBasicTypeInfo.type is invalid, field type:[%s]", stField.type.c_str());
    }
    return strFunName;
}


bool CCPlusPlusWriter::WriteFunValueToCStr(std::ofstream& ofs, const CMsgTypeStruct* pStruct)
{
    if (nullptr == pStruct)
    {
        return false;
    }
    bool bResult = true;
    ofs << "    // Write function of ValueToCStr for " << pStruct->GetMsgName() << std::endl;
    ofs << "    inline const char* ValueToCStr() const" << std::endl;
    ofs << "    {" << std::endl;
    ofs << "        static std::string s_strMsgValue;" << std::endl;
    ofs << "        s_strMsgValue.clear();" << std::endl;
    ofs << "        s_strMsgValue.reserve(2048);" << std::endl;
    const std::vector<StructField>& vFieldList = pStruct->GetFieldList();
    for each (const StructField& stStructField in vFieldList)
    {
        ofs << "        // Append Field " << stStructField.name << std::endl;
        ofs << "        s_strMsgValue.append(\"{" << stStructField.ShortName() << ":\");" << std::endl;
        if (1 == stStructField.element_count)
        {
            if ((0 == stStructField.type.compare("AUINT32") || 0 == stStructField.type.compare("AUINT16") || 0 == stStructField.type.compare("AUINT8")))
            {
                ofs << "        s_strMsgValue.append(CStringUtil::UInt32ToDecString(" << stStructField.name << "));" << std::endl;
            }
            else if ((0 == stStructField.type.compare("AINT32") || 0 == stStructField.type.compare("AINT16") || 0 == stStructField.type.compare("AINT8")))
            {
                ofs << "        s_strMsgValue.append(CStringUtil::Int32ToDecString(" << stStructField.name << "));" << std::endl;
            }
            else if (CMsgTypeContainer::IsEnumTypeName(stStructField.type))
            {
                ofs << "        s_strMsgValue.append(" << stStructField.type << "ToCStr(" << stStructField.name << "));" << std::endl;
            }
            else if (CMsgTypeContainer::IsStructTypeName(stStructField.type))
            {
                ofs << "        s_strMsgValue.append(" << stStructField.name << ".ValueToCStr());" << std::endl;
            }
            else
            {
                CAquariusLogger::Logger(LL_ERROR_FUN, "UnHandleFieldType, Type: [%s], ElementCount Is 1", stStructField.type.c_str());
                bResult = false;
                break;
            }
        }
        else
        {
            if (0 == stStructField.type.compare("char"))
            {
                ofs << "        s_strMsgValue.append(CStringUtil::BuildSafeString(" << stStructField.name << "));" << std::endl;
            }
            else if ((0 == stStructField.type.compare("AUINT32") || 0 == stStructField.type.compare("AUINT16") || 0 == stStructField.type.compare("AUINT8")))
            {
                for (int nElementIndex = 0; nElementIndex < stStructField.element_count; ++nElementIndex)
                {
                    ofs << "        s_strMsgValue.append(CStringUtil::UInt32ToDecString(" << stStructField.name << "[" << nElementIndex << "])).append(\";\");" << std::endl;
                }
            }
            else if ((0 == stStructField.type.compare("AINT32") || 0 == stStructField.type.compare("AINT16") || 0 == stStructField.type.compare("AINT8")))
            {
                for (int nElementIndex = 0; nElementIndex < stStructField.element_count; ++nElementIndex)
                {
                    ofs << "        s_strMsgValue.append(CStringUtil::Int32ToDecString(" << stStructField.name << "[" << nElementIndex << "])).append(\";\");" << std::endl;
                }
            }
            else if (CMsgTypeContainer::IsStructTypeName(stStructField.type))
            {
                ofs << "        int nIndex = -1; " << std::endl;
                ofs << "        for each ( const " << stStructField.type << "& stValue in " << stStructField.name << ")" << std::endl;
                ofs << "        {" << std::endl;
                ofs << "            s_strMsgValue.append(\"<E\").append(CStringUtil::Int32ToDecString(++nIndex)).append(\":\");" << std::endl;
                ofs << "            s_strMsgValue.append(stValue.ValueToCStr());" << std::endl;
                ofs << "            s_strMsgValue.append(\">\");" << std::endl;
                ofs << "        }" << std::endl;
            }
            else
            {
                CAquariusLogger::Logger(LL_ERROR_FUN, "UnHandleFieldType, Type: [%s], ElementCount: [%d]", stStructField.type.c_str(), stStructField.element_count);
                bResult = false;
                break;
            }
        }
        ofs << "        s_strMsgValue.append(\"}\");" << std::endl;
    }
    ofs << "        return s_strMsgValue.c_str();" << std::endl;
    ofs << "    }" << std::endl;
    return bResult;
}
