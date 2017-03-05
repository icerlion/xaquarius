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
 * FileName: MsgHandleHub.cpp
 * 
 * Purpose:  Register msg handler and implement it.
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/2/22 16:19
===================================================================+*/
#include "HubServer/MsgHandle/MsgHandleHub.h"
#include "AquariusCore/Engine/Msg/MsgDispatch.h"
#include "AquariusCore/Engine/Server/AquariusServer.h"
#include "AquariusCore/Engine/Connector/TCPConnectorManager.h"
#include "Protocol/SSProtocol.hpp"

bool CMsgHandleHub::InitializeMsgHandle()
{
    REGISTER_MSG_HANDLE(CMsgHandleHub, SS_HeartBeat_Msg);
    REGISTER_MSG_HANDLE(CMsgHandleHub, GH_Echo_Request);
    return true;
}


bool CMsgHandleHub::OnSS_HeartBeat_Msg(const char* pData, AUINT32 nDataLen, CTCPSession* pTCPSession)
{
    PARSE_TCP_MSG(SS_HeartBeat_Msg);
    return CTCPConnectorManager::OnSSHeartBeatMsg(pTCPSession->GetSessionId(), pMsg->server_type, pMsg->heartbeat_index);
}

bool CMsgHandleHub::OnGH_Echo_Request(const char* pData, AUINT32 nDataLen, CTCPSession* pTCPSession)
{
    PARSE_TCP_MSG(GH_Echo_Request);
    HG_Echo_Response stHGRes;
    stHGRes.data = pMsg->data;
    stHGRes.client_session_id = pMsg->client_session_id;
    stHGRes.response_code = EResponseCode::G_Success;
    return CAquariusServer::SendMsgToGateServer(stHGRes, pMsg->server_group_id);
}
