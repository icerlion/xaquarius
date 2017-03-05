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
 * FileName: MsgHandlerTable.cpp
 * 
 * Purpose:  Implement msg handler table
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/2/22 16:13
===================================================================+*/
#include "AquariusCore/Engine/Msg/MsgHandlerTable.h"

std::map<AUINT16, const CMsgHandlerWrapper*> CMsgHandlerTable::ms_mapMsgHandler;

void CMsgHandlerTable::Destroy()
{
    for each (auto& kvp in ms_mapMsgHandler)
    {
        CMsgHandlerWrapperPool::RecycleObject(const_cast<CMsgHandlerWrapper*>(kvp.second));
    }
    ms_mapMsgHandler.clear();
}

const MsgHandlerData& CMsgHandlerTable::GetMsgHandlerData( AUINT16 nMsgId )
{
    // Define invalid msg handler data
    static MsgHandlerData s_stInvalidMsgHandlerData;
    // Find msg handler by id
    auto itFind = ms_mapMsgHandler.find(nMsgId);
    if (itFind == ms_mapMsgHandler.end())
    {
        return s_stInvalidMsgHandlerData;
    }
    const CMsgHandlerWrapper* pMsgHandlerWrapper = itFind->second;
    if (nullptr == pMsgHandlerWrapper)
    {
        return s_stInvalidMsgHandlerData;
    }
    return pMsgHandlerWrapper->GetMsgHandlerData();
}
