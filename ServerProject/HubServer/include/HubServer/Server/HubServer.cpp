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
 * FileName: HubServer.cpp
 * 
 * Purpose:  Implement HubServer
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/2/22 16:19
===================================================================+*/
#include "HubServer/Server/HubServer.h"
#include "HubServer/MsgHandle/MsgHandleHub.h"
#include "AquariusCore/Engine/Timer/TimerManager.h"
#include "AquariusCore/Engine/Msg/MsgStatistic.h"
#include "Protocol/MsgUtil.hpp"
#include "Protocol/SSProtocol.hpp"


CHubServer::CHubServer()
    :CAquariusServer(EAquariusServerType::HubServer)
{
}

CHubServer::~CHubServer()
{
}

bool CHubServer::OnInit()
{
    // Init msg statistic function
    CMsgStatistic::InitMsgStatistic(CMsgUtil::GetMsgInfo);
    // Register msg handler
    if (!CMsgHandleHub::InitializeMsgHandle())
    {
        return false;
    }
    // You can call some other init method in this function.
    return true;
}

bool CHubServer::OnDestroy()
{
    return true;
}

bool CHubServer::TimerSendSSHeartBeatMsg( CTCPConnector* pTCPConnector )
{
    RETURN_ON_NULL_TCP_CONNECTOR(false);
    return pTCPConnector->SendSSHeartBeatMsg<SS_HeartBeat_Msg>();
}
