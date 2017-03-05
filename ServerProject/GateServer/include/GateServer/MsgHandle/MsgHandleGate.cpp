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
 * FileName: MsgHandleGate.cpp
 * 
 * Purpose:  Register msg handler and implement it.
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/2/22 15:26
===================================================================+*/
 
#include "GateServer/MsgHandle/MsgHandleGate.h"
#include "AquariusCore/Engine/Msg/MsgDispatch.h"
#include "Protocol/CSProtocol.hpp"
#include "Protocol/SSProtocol.hpp"
#include "AquariusCore/Logger/AquariusLogger.h"
#include "AquariusCore/Engine/Session/TCPSessionManager.h"
#include "GateServer/Server/GateServer.h"

std::set<AUINT32> CMsgHandleGate::ms_setClientSessionId;

bool CMsgHandleGate::InitializeMsgHandle()
{
    REGISTER_MSG_HANDLE(CMsgHandleGate, CG_HeartBeat_Msg);
    REGISTER_MSG_HANDLE(CMsgHandleGate, SS_HeartBeat_Msg);
    REGISTER_MSG_HANDLE(CMsgHandleGate, CG_Echo_Request);
    REGISTER_MSG_HANDLE(CMsgHandleGate, FG_Echo_Response);
    REGISTER_MSG_HANDLE(CMsgHandleGate, HG_Echo_Response);
    REGISTER_MSG_HANDLE(CMsgHandleGate, CG_Chat_Cmd);
    return true;
}

void CMsgHandleGate::TimerUpdate()
{
    static int s_nClientCount = 0;
    if (s_nClientCount != ms_setClientSessionId.size())
    {
        s_nClientCount = (int)ms_setClientSessionId.size();
        CAquariusLogger::Logger(LL_NOTICE, "ClientCount: %d", s_nClientCount);
    }
}

bool CMsgHandleGate::OnCG_HeartBeat_Msg(const char* pData, AUINT32 nDataLen, CTCPSession* pTCPSession)
{
    PARSE_TCP_MSG(CG_HeartBeat_Msg);
    // You can define your own logic code here
    ms_setClientSessionId.insert(pTCPSession->GetSessionId());
    // You can update SetPFNHandleSessionError
    pTCPSession->SetFunctionHandleSessionError(CMsgHandleGate::OnClientSessionError);
    CAquariusLogger::Logger(LL_NOTICE, "ClientSessionId: [%u], HeartBeatIndex: [%hu]", pTCPSession->GetSessionId(), pMsg->heartbeat_index);
    return pTCPSession->OnHeartBeatRequest<GC_HeartBeat_Msg>(pMsg->heartbeat_index);
}

bool CMsgHandleGate::OnSS_HeartBeat_Msg(const char* pData, AUINT32 nDataLen, CTCPSession* pTCPSession)
{
    PARSE_TCP_MSG(SS_HeartBeat_Msg);
    return CTCPConnectorManager::OnSSHeartBeatMsg(pTCPSession->GetSessionId(), pMsg->server_type, pMsg->heartbeat_index);
}

bool CMsgHandleGate::OnCG_Echo_Request(const char* pData, AUINT32 nDataLen, CTCPSession* pTCPSession)
{
    PARSE_TCP_MSG(CG_Echo_Request);
    //////////////////////////////////////////////////////////////////////////
    // Begin of demo code
    CAquariusLogger::Logger(LL_NOTICE_FUN, "ClientSessionId: [%u], data: [%s]", pTCPSession->GetSessionId(), pMsg->data.ValueToCStr());
    if (0 != AQUARIUS_PROTOCOL_VERSION.compare(pMsg->data.protocol_version))
    {
        CAquariusLogger::Logger(LL_WARNING_FUN, "ClientSessionId: [%u], Invalid Protocol Version", pTCPSession->GetSessionId());
        return false;
    }
    GF_Echo_Request stGFReq;
    stGFReq.data = pMsg->data;
    stGFReq.client_session_id = pTCPSession->GetSessionId();
    CGateServer::SendMsgToFightServer(stGFReq);
    // End of demo code
    //////////////////////////////////////////////////////////////////////////
    return true;
}

bool CMsgHandleGate::OnFG_Echo_Response(const char* pData, AUINT32 nDataLen, CTCPSession* pTCPSession)
{
    PARSE_TCP_MSG(FG_Echo_Response);
    //////////////////////////////////////////////////////////////////////////
    // Begin of demo code
    GH_Echo_Request stGHReq;
    stGHReq.data = pMsg->data;
    stGHReq.client_session_id = pMsg->client_session_id;
    stGHReq.server_group_id = CAquariusServer::GetServerGroupId();
    CGateServer::SendMsgToHubServer(stGHReq);
    // End of demo code, you can delete it
    //////////////////////////////////////////////////////////////////////////
    return true;
}

bool CMsgHandleGate::OnHG_Echo_Response(const char* pData, AUINT32 nDataLen, CTCPSession* pTCPSession)
{
    PARSE_TCP_MSG(HG_Echo_Response);
    //////////////////////////////////////////////////////////////////////////
    // Begin of demo code, which show how to process msg which is sent from inner server
    CTCPSession* pClientTCPSession = CTCPSessionManager::GetTCPSession(pMsg->client_session_id);
    if (nullptr != pClientTCPSession)
    {
        GC_Echo_Response stGCRes;
        stGCRes.data = pMsg->data;
        stGCRes.response_code = pMsg->response_code;
        pClientTCPSession->SendBinaryMsg(stGCRes);
    }
    else
    {
        CAquariusLogger::Logger(LL_NOTICE_FUN, "ClientSessionId: [%u] is offline", pMsg->client_session_id);
    }
    // End of demo code, you can delete it
    //////////////////////////////////////////////////////////////////////////
    return true;
}

bool CMsgHandleGate::OnCG_Chat_Cmd(const char* pData, AUINT32 nDataLen, CTCPSession* pTCPSession)
{
    PARSE_TCP_MSG(CG_Chat_Cmd);
    //////////////////////////////////////////////////////////////////////////
    // Begin of demo code, which show how to process client msg
    // Broadcast chat msg to every client
    GC_Chat_Msg stGCMsg;
    stGCMsg.data = pMsg->data;
    stGCMsg.data.speaker_id = pTCPSession->GetSessionId();
    stGCMsg.data.time = (AUINT32)time(nullptr);
    for each (AUINT32 nClientSessionId in ms_setClientSessionId)
    {
        CTCPSession* pClientTCPSession = CTCPSessionManager::GetTCPSession(nClientSessionId);
        if (nullptr != pClientTCPSession)
        {
            pClientTCPSession->SendBinaryMsg(stGCMsg);
        }
    }
    // Send msg to FightServer
    GF_Chat_Cmd stGFCmd;
    stGFCmd.data = stGCMsg.data;
    CAquariusServer::SendMsgToFightServer(stGFCmd);
    // End of demo code, you can delete it
    //////////////////////////////////////////////////////////////////////////
    return true;
}

bool CMsgHandleGate::OnClientSessionError(AUINT32 nSessionId)
{
    ms_setClientSessionId.erase(nSessionId);
    return true;
}
