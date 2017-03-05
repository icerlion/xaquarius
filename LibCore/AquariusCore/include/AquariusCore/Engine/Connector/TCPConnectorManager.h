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
 * FileName: TCPConnectorManager.h
 * 
 * Purpose:  Hold all TCPConnector object
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/2/22 15:29
===================================================================+*/
#ifndef _TCP_CONNECTOR_MANAGER_H_
#define _TCP_CONNECTOR_MANAGER_H_
#include "AquariusCore/Engine/Connector/TCPConnector.h"
#include "AquariusCore/Util/TypeReDef/TypeReDef.hpp"
#include "AquariusCore/Util/TinyUtil/CommonUtil.h"

class CAquariusServer;
class CTCPConnectorManager
{
public:
    // Initialize connector server manager
    static bool Initialize(CAquariusServer* pAquariusServer);

    // Destroy manager
    static void Destroy();

    /**
     * Description: Wrapper of OnConnectorSessionError
     * Parameter AUINT32 nSessionId                         [in]: session id
     * Returns bool: true if success
     */
    static bool HandleConnectorSessionError(AUINT32 nSessionId);

    /** 
    * Description: Process heart beat msg
    * Parameter AUINT32 nSessionId                  [in]: session id
    * Parameter AUINT16 nRemoteServerType           [in]: remote server type / remote server group id
    * Parameter AUINT16 nHeartBeatIndex             [in]: input heart beat index
    * Returns bool: true if success
    */
    static bool OnSSHeartBeatMsg(AUINT32 nSessionId, AUINT16 nRemoteServerType, AUINT16 nHeartBeatIndex);

    // Insert a server to re-connect list
    static void InsertReConnectList(CTCPConnector* pConnector);

    // Update session id of connector
    static bool UpdateConnectorSessionId(CTCPConnector* pConnector);

    // Get remove server up time
    static AUINT32 GetRemoteServerUpTime(AUINT32 nServerId);

    // Send msg to remote server
    template<typename MsgType>
    static bool SendMsgToRemoteServer(const MsgType& stMsg, AUINT32 nServerId)
    {
        // 1. Check pointer
        CTCPConnector* pConnector = GetRemoteServerConnector(nServerId);
        if (nullptr == pConnector)
        {
            CAquariusLogger::Logger(LL_ERROR_FUN, "nullptr == pConnector, msg name: [%s]", MsgType::GetMsgName());
            return false;
        }
        // 2. Send msg to server
        if (!pConnector->SendBinaryMsg(stMsg))
        {
            CAquariusLogger::Logger(LL_WARNING_FUN, "Failed to send msg to server, msg name: [%s], content: [%s]", MsgType::GetMsgName(), stMsg.ValueToCStr());
            return false;
        }
        return true;
    }

    // Wrapper for CAquariusServer::OnRemoteServerUp
    static bool OnRemoteServerUp(const ServerItem& stServerItem);

    // Wrapper for CAquariusServer::OnRemoteServerDown
    static bool OnRemoteServerDown(const ServerItem& stServerItem);

private:
    // Find connector by server type
    static CTCPConnector* GetRemoteServerConnector(AUINT32 nServerId);

    // Remove connector session id
    static void RemoveConnectorSessionId(AUINT32 nSessionId);

    // Send heart beat msg to connector
    static void TimerSendHeartBeatToConnector();
    
    // Check re-connect connector
    static void TimerCheckOfflineConnector();

    // Connect inner game server and HubServer
    static bool InitConnectorServer();

private:
    typedef std::map<AUINT32, CTCPConnector*> MapConnector;
    typedef std::map<AUINT16, AUINT16> MapHeartBeat;
    // Key: connector session id
    // Value: connector pointer
    static MapConnector ms_mapConnectorBySessionId;
    // Key: server id or server type
    // Value: connector pointer
    static MapConnector ms_mapConnectorByServerId;
    // Re-connect connector list
    static std::set<CTCPConnector*> ms_setOfflineConnector;
    // Key: Connector Server Type, read from config file
    // Value: Heart beat index
    // Record heart beat index for single link-in connector
    static MapHeartBeat ms_mapHeartBeat;
    // Aquarius Server
    static CAquariusServer* ms_pAquariusServer;
};

#endif // _TCP_CONNECTOR_MANAGER_H_
