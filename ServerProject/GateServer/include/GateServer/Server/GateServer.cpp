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
 * FileName: GateServer.cpp
 * 
 * Purpose:  Implement GateServer
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/2/22 15:23
===================================================================+*/
 
#include "GateServer/Server/GateServer.h"
#include "GateServer/MsgHandle/MsgHandleGate.h"
#include "AquariusCore/Engine/Timer/TimerManager.h"
#include "AquariusCore/Engine/Msg/MsgStatistic.h"
#include "Protocol/MsgUtil.hpp"
#include "Protocol/SSProtocol.hpp"
#include "AquariusCore/Engine/Config/ServerGroupConfig.h"

CGateServer::CGateServer()
    :CAquariusServer(EAquariusServerType::GateServer)
{
}

CGateServer::~CGateServer()
{
}

bool CGateServer::OnInit()
{
    CMsgStatistic::InitMsgStatistic(CMsgUtil::GetMsgInfo);
    if (!CMsgHandleGate::InitializeMsgHandle())
    {
        return false;
    }
    // Demo code, show how to register timer function
    CTimerManager::RegisterTimer(&CMsgHandleGate::TimerUpdate, 1000);
    return true;
}


bool CGateServer::OnDestroy()
{
    return true;
}

bool CGateServer::TimerSendSSHeartBeatMsg(CTCPConnector* pTCPConnector)
{
    RETURN_ON_NULL_TCP_CONNECTOR(false);
    return pTCPConnector->SendSSHeartBeatMsg<SS_HeartBeat_Msg>();
}
