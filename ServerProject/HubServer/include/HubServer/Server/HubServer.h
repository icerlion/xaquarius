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
 * FileName: HubServer.h
 * 
 * Purpose:  Define HubServer
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/2/22 16:19
===================================================================+*/
#ifndef _HUBSERVER_H_
#define _HUBSERVER_H_
#include "AquariusCore/Engine/Connector/TCPConnector.h"
#include "AquariusCore/Engine/Server/AquariusServer.h"
#include "AquariusCore/Util/Singleton/ISingleton.hpp"

class CHubServer : public CAquariusServer, public ISingleton<CHubServer>
{
public:
    // Constructor
    CHubServer();

    // Destructor
    virtual ~CHubServer() override final;

    // Rewrite CAquariusServer::OnInit
    virtual bool OnInit() override final;

    // Rewrite CAquariusServer::OnStop
    virtual bool OnDestroy() override final;

private:
    // Implement TimerSendSSHeartBeatMsg
    virtual bool TimerSendSSHeartBeatMsg(CTCPConnector* pTCPConnector) override final;
};


#endif // _HUBSERVER_H_
