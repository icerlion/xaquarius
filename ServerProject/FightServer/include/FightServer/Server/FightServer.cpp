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
 * FileName: FightServer.cpp
 * 
 * Purpose:  Implement FightServer
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/2/22 16:21
===================================================================+*/
#include "FightServer/Server/FightServer.h"
#include "FightServer/MsgHandle/MsgHandleFight.h"
#include "AquariusCore/Engine/Msg/MsgStatistic.h"
#include "Protocol/MsgUtil.hpp"
#include "AquariusCore/Engine/Config/ServerGroupConfig.h"
#include "AquariusCore/Engine/Task/DBController/DBTaskController.hpp"
#include "AquariusCore/Engine/Task/HTTPController/HTTPTaskController.hpp"

bool CFightServer::OnInit()
{
    CMsgStatistic::InitMsgStatistic(CMsgUtil::GetMsgInfo);
    if (!CMsgHandleFight::InitializeMsgHandle())
    {
        return false;
    }
    const static unsigned int DB_WORK_THREAD_COUNT = 1;
    if (!CDBTaskController::GetSingleton().Initialize(DB_WORK_THREAD_COUNT, (void*)(CServerGroupConfig::GetDBConfig())))
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "CDBTaskController.Initialize failed");
        return false;
    }
    const static AUINT32 CONST_HTTP_THREAD_COUNT = 2;
    if (!CHTTPTaskController::GetSingleton().Initialize(CONST_HTTP_THREAD_COUNT, nullptr))
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "CHTTPTaskController init failed!");
        return false;
    }
    return true;
}

bool CFightServer::OnDestroy()
{
    return true;
}

CFightServer::CFightServer() 
    :CAquariusServer(EAquariusServerType::FightServer)
{

}

bool CFightServer::TimerSendSSHeartBeatMsg( CTCPConnector* pTCPConnector )
{
    RETURN_ON_NULL_TCP_CONNECTOR(false);
    return pTCPConnector->SendSSHeartBeatMsg<SS_HeartBeat_Msg>();
}

CFightServer::~CFightServer()
{

}
