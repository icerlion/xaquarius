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
 * FileName: MsgHandleFight.cpp
 * 
 * Purpose:  Register msg handler and implement it.
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/2/22 16:21
===================================================================+*/
#include "FightServer/MsgHandle/MsgHandleFight.h"
#include "AquariusCore/Engine/Msg/MsgDispatch.h"
#include "AquariusCore/Engine/Connector/TCPConnectorManager.h"
#include "AquariusCore/Engine/Server/AquariusServer.h"
#include "AquariusCore/Engine/Task/DBController/DBTaskController.hpp"
#include "AquariusCore/Engine/Task/HTTPController/HTTPTaskController.hpp"
#include "FightServer/DBTask/DBTaskDemo.h"
#include "FightServer/HTTPTask/HTTPTaskDemo.h"
#include "Protocol/SSProtocol.hpp"

bool CMsgHandleFight::InitializeMsgHandle()
{
    REGISTER_MSG_HANDLE(CMsgHandleFight, SS_HeartBeat_Msg);
    REGISTER_MSG_HANDLE(CMsgHandleFight, GF_Echo_Request);
    REGISTER_MSG_HANDLE(CMsgHandleFight, GF_Chat_Cmd);
    return true;
}

bool CMsgHandleFight::OnSS_HeartBeat_Msg(const char* pData, AUINT32 nDataLen, CTCPSession* pTCPSession)
{
    PARSE_TCP_MSG(SS_HeartBeat_Msg);
    return CTCPConnectorManager::OnSSHeartBeatMsg(pTCPSession->GetSessionId(), pMsg->server_type, pMsg->heartbeat_index);
}

bool CMsgHandleFight::OnGF_Echo_Request(const char* pData, AUINT32 nDataLen, CTCPSession* pTCPSession)
{
    PARSE_TCP_MSG(GF_Echo_Request);
    //////////////////////////////////////////////////////////////////////////
    // Begin of demo code
    CAquariusLogger::Logger(LL_NOTICE_FUN, "ClientSessionId: [%u] called", pMsg->client_session_id);
    FG_Echo_Response stFGRes;
    stFGRes.client_session_id = pMsg->client_session_id;
    stFGRes.data = pMsg->data;
    CAquariusServer::SendMsgToGateServer(stFGRes);
    // End of demo code
    //////////////////////////////////////////////////////////////////////////
    return true;
}

bool CMsgHandleFight::OnGF_Chat_Cmd(const char* pData, AUINT32 nDataLen, CTCPSession* pTCPSession)
{
    PARSE_TCP_MSG(GF_Chat_Cmd);
    //////////////////////////////////////////////////////////////////////////
    // Begin of demo code
    // Demo how to use DBTask
    CDBTaskDemo* pDBTask = CDBTaskDemoPool::AcquireObject();
    if (nullptr != pDBTask)
    {
        pDBTask->InitDBTask(pMsg->data);
        CDBTaskController::InputDBTask(pDBTask);
    }
    // Demo how to use HTTPTask
    CHTTPTaskDemo* pHTTPTask = CHTTPTaskDemoPool::AcquireObject();
    if (nullptr != pHTTPTask)
    {
        pHTTPTask->InitHTTPTask();
        CHTTPTaskController::InputHTTPTask(pHTTPTask);
    }
    // End of demo code
    //////////////////////////////////////////////////////////////////////////
    return true;
}
