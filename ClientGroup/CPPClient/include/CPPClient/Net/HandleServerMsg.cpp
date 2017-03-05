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
 * FileName: HandleServerMsg.cpp
 * 
 * Purpose:  Handle net msg from Gate
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/3/5 16:31
===================================================================+*/
#include "CPPClient/Net/HandleServerMsg.h"
#include "AquariusCore/Engine/Msg/MsgDispatch.h"
#include "AquariusCore/Logger/AquariusLogger.h"
#include "Protocol/CSProtocol.hpp"
#include "CPPClient/ClientUser/ClientUser.h"


#define HANDLE_GATE_MSG(MsgStruct) \
    auto itFind = ms_mapClientUser.find(pTCPSession->GetSessionId()); \
    if (itFind == ms_mapClientUser.end()) { CAquariusLogger::Logger(LL_ERROR_FUN, "itFind == ms_mapClientUser.end()"); return false; } \
    CClientUser* pUser = itFind->second; \
    if (nullptr == pUser ) { return false; } \
    const MsgStruct* pMsg = CMsgDispatch::ParseTypeMsg<MsgStruct>(pData, nDataLen); \
    if (nullptr == pMsg) { return false; } \
    pUser->On##MsgStruct(*pMsg); \
    return true;
// End of HANDLE_GATE_MSG

CHandleServerMsg::MapClientUser CHandleServerMsg::ms_mapClientUser;

bool CHandleServerMsg::InitializeMsgHandle()
{
    REGISTER_MSG_HANDLE(CHandleServerMsg, GC_HeartBeat_Msg);
    REGISTER_MSG_HANDLE(CHandleServerMsg, GC_Echo_Response);
    REGISTER_MSG_HANDLE(CHandleServerMsg, GC_Chat_Msg);
    return true;
}

bool CHandleServerMsg::OnGC_HeartBeat_Msg(const char* pData, AUINT32 nDataLen, CTCPSession* pTCPSession)
{
    HANDLE_GATE_MSG(GC_HeartBeat_Msg);
}

bool CHandleServerMsg::OnGC_Echo_Response(const char* pData, AUINT32 nDataLen, CTCPSession* pTCPSession)
{
    HANDLE_GATE_MSG(GC_Echo_Response);
}

bool CHandleServerMsg::OnGC_Chat_Msg(const char* pData, AUINT32 nDataLen, CTCPSession* pTCPSession)
{
    HANDLE_GATE_MSG(GC_Chat_Msg);
}
