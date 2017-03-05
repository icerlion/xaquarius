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
 * FileName: MsgTypeDef.cpp
 * 
 * Purpose:  Implement msg type define
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/3/5 14:19
===================================================================+*/
#include "MsgTypeDef.h"

std::string StructField::ShortName() const
{
    std::vector<std::string> vToken = CStringUtil::SplitStringToToken(name, '_');
    std::string strResult;
    for each (auto& strToken in vToken)
    {
        strResult.append(strToken.substr(0, 1));
    }
    if (strResult.empty())
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "Invalid FieldName: [%s]", name.c_str());
    }
    return strResult;
}

bool CMsgTypeEnum::AddField(const EnumField& stField)
{
    m_vFieldList.push_back(stField);
    if (!m_setFieldValue.insert(stField.value).second)
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "Enum [%s] has reduplicate value: [%d], field name: [%s]", m_strMsgName.c_str(), stField.value, stField.name.c_str());
        return false;
    }
    if (!m_setFieldName.insert(stField.name).second)
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "Enum [%s] has reduplicate field name : [%s]", m_strMsgName.c_str(), stField.name.c_str());
        return false;
    }
    return true;
}

void IMsgType::SetDesc(const std::string& desc)
{
    m_strDesc = desc;
}
