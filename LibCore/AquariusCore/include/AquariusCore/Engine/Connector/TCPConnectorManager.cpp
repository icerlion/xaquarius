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
 * FileName: TCPConnectorManager.cpp
 * 
 * Purpose:  Implement TCPConnectorManager
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/2/22 15:29
===================================================================+*/
#include "AquariusCore/Engine/Connector/TCPConnectorManager.h"
#include "AquariusCore/Logger/AquariusLogger.h"
#include "AquariusCore/Engine/Timer/TimerManager.h"
#include "AquariusCore/Util/TinyUtil/RandUtil.h"
#include "AquariusCore/Engine/Server/AquariusServer.h"
#include "AquariusCore/Engine/Config/HubServerConfig.h"

CTCPConnectorManager::MapConnector CTCPConnectorManager::ms_mapConnectorBySessionId;
CTCPConnectorManager::MapConnector CTCPConnectorManager::ms_mapConnectorByServerId;
std::set<CTCPConnector*> CTCPConnectorManager::ms_setOfflineConnector;
CTCPConnectorManager::MapHeartBeat CTCPConnectorManager::ms_mapHeartBeat;
CAquariusServer* CTCPConnectorManager::ms_pAquariusServer = nullptr;

bool CTCPConnectorManager::OnSSHeartBeatMsg(AUINT32 nSessionId, AUINT16 nRemoteServerType, AUINT16 nHeartBeatIndex)
{
    // Find server item config
    ServerItem stRemoteServerItem;
    // If self server is HubServer, remote server type is server group id
    // Or, just find remote server from ServerGroupConfig
    if (EAquariusServerType::HubServer == CAquariusServer::GetServerType())
    {
        stRemoteServerItem = CHubServerConfig::GetServerItemByGroupId(nRemoteServerType);
    }
    else
    {
        stRemoteServerItem = CServerGroupConfig::GetServerItemByType(nRemoteServerType);
    }
    if (EAquariusServerType::Invalid == stRemoteServerItem.nServerType)
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "Invalid RemoteServerType: [%d]", nRemoteServerType);
        return false;
    }
    // Add log for the first HB
    if (0 == nHeartBeatIndex)
    {
        CAquariusLogger::Logger(LL_NOTICE, "%s Is InComming, ServerInfo: [%s:%d], SessionId: [%u]", stRemoteServerItem.strName.c_str(), stRemoteServerItem.stPrivateListener.strListenerIP.c_str(), stRemoteServerItem.stPrivateListener.nListenerPort, nSessionId);
    }
    // Check connector type
    if (stRemoteServerItem.setConnector.end() == stRemoteServerItem.setConnector.find(CAquariusServer::GetServerType()))
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "Invalid incoming HeartBeat, IncomingServerType: [%hu]", nRemoteServerType);
        return false;
    }
    // Compare heart beat index
    auto itFind = ms_mapHeartBeat.find(nRemoteServerType);
    if (itFind == ms_mapHeartBeat.end())
    {
        CAquariusLogger::Logger(LL_DEBUG_FUN, "[%s]: Input HB [%hu] - Server: [0]", stRemoteServerItem.strName.c_str(), nHeartBeatIndex);
        AUINT16 nNextHBIndex = nHeartBeatIndex + 1;
        if (0 == nNextHBIndex)
        {
            nNextHBIndex = 1;
        }
        ms_mapHeartBeat.insert(std::make_pair(nRemoteServerType, nNextHBIndex));
    }
    else
    {
        if (itFind->second != nHeartBeatIndex)
        {
            if (0 == nHeartBeatIndex)
            {
                CAquariusLogger::Logger(LL_WARNING, "[%s] Re-Coming, Last HB: [%hu]", stRemoteServerItem.strName.c_str(),  itFind->second);
            }
            else
            {
                CAquariusLogger::Logger(LL_ERROR_FUN, "[%s]: Input HB [%hu] - Server: [%hu]", stRemoteServerItem.strName.c_str(),  nHeartBeatIndex, itFind->second);
            }
            itFind->second = nHeartBeatIndex;
        }
        else
        {
            CAquariusLogger::Logger(LL_DEBUG_FUN, "[%s]: Input HB [%hu] - Server: [%hu]", stRemoteServerItem.strName.c_str(),  nHeartBeatIndex, itFind->second);
        }
        ++(itFind->second);
        if (0 == itFind->second)
        {
            itFind->second = 1;
        }
    }
    return true;
}

bool CTCPConnectorManager::InitConnectorServer()
{
    // 1. Init result flag
    bool bResult = true;
    const ServerItem& stSelfConfig = CAquariusServer::GetServerConfig();
    MapServerItem mapServerItem;
    if (EAquariusServerType::HubServer == stSelfConfig.nServerType)
    {
        mapServerItem = CHubServerConfig::GetLinkGateServerMap();
    }
    else
    {
        mapServerItem = CServerGroupConfig::GetGroupServerItemList();
    }
    for each (auto& kvp in mapServerItem)
    {
        const ServerItem& stItem = kvp.second;
        // 2.1 Skip self and same type server
        if (stItem.nServerType == stSelfConfig.nServerType
            || stSelfConfig.setConnector.end() == stSelfConfig.setConnector.find(stItem.nServerType))
        {
            continue;
        }
        // 2.2 Make new connector and init it
        CTCPConnector* pConnector = new CTCPConnector();
        if (!pConnector->Initialize(stItem))
        {
            CAquariusLogger::Logger(LL_ERROR_FUN, "Failed to init CTCPConnector");
            bResult = false;
            break;
        }
        // 2.3 Check session id
        if (0 == pConnector->GetSessionId())
        {
            CAquariusLogger::Logger(LL_ERROR_FUN, "0 == pConnector->GetSessionId()");
            bResult = false;
            break;
        }
        // 2.4 Insert to connector server list
        if (!ms_mapConnectorByServerId.insert(std::make_pair(pConnector->GetConnectorServerId(), pConnector)).second)
        {
            CAquariusLogger::Logger(LL_ERROR_FUN, "Insert connector server type failed: [%d]", pConnector->GetConnectorServerId());
            bResult = false;
            break;
        }
    }
    return bResult;
}

bool CTCPConnectorManager::Initialize(CAquariusServer* pAquariusServer)
{
    // 1. Set self server config
    ms_pAquariusServer = pAquariusServer;
    const ServerItem& stSelfConfig = CAquariusServer::GetServerConfig();
    if (stSelfConfig.setConnector.empty())
    {
        return true;
    }
    // 2. Get the server group list and loop it
    if (!InitConnectorServer())
    {
        return false;
    }
    // 3. Register Timer
    const static int CONST_SS_KEEP_AVLIE_HEARTBEAT_INTERVAL = 5000;
    const static int CONST_SS_KEEP_AVLIE_CHECK_OFFLINE = 3000;
    CTimerManager::RegisterTimer(boost::bind(&CTCPConnectorManager::TimerSendHeartBeatToConnector), CONST_SS_KEEP_AVLIE_HEARTBEAT_INTERVAL);
    CTimerManager::RegisterTimer(boost::bind(&CTCPConnectorManager::TimerCheckOfflineConnector), CONST_SS_KEEP_AVLIE_CHECK_OFFLINE);
    return true;
}

void CTCPConnectorManager::TimerSendHeartBeatToConnector()
{
    if (ms_mapConnectorByServerId.empty())
    {
        return;
    }
    if (nullptr == ms_pAquariusServer)
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "nullptr == ms_pAquariusServer");
        return;
    }
    // Send heart beat to every connector
    for each (auto& kvp in ms_mapConnectorByServerId)
    {
        ms_pAquariusServer->TimerSendSSHeartBeatMsg(kvp.second);
    }
}

void CTCPConnectorManager::Destroy()
{
    for each (auto& kvp in ms_mapConnectorByServerId)
    {
        delete kvp.second;
    }
    ms_mapConnectorByServerId.clear();
    ms_mapConnectorBySessionId.clear();
    ms_setOfflineConnector.clear();
    ms_mapHeartBeat.clear();
    ms_pAquariusServer = nullptr;
}

void CTCPConnectorManager::TimerCheckOfflineConnector()
{
    // If the server is stop, won't check offline connector
    if (CAquariusServer::IsServerInStop() || 0xFFFFFFFF != CAquariusServer::GetShutDownServerTime())
    {
        return;
    }
    // Loop every re-connect connector
    for each (auto& pConnector in ms_setOfflineConnector)
    {
        if (nullptr == pConnector)
        {
            CAquariusLogger::Logger(LL_ERROR_FUN, "nullptr == pConnector");
            continue;
        }
        pConnector->OpenConnector();
    }
    ms_setOfflineConnector.clear();
}


void CTCPConnectorManager::RemoveConnectorSessionId( AUINT32 nSessionId )
{
    // Find the connector server
    auto itFind = ms_mapConnectorBySessionId.find(nSessionId);
    if (itFind == ms_mapConnectorBySessionId.end())
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "No SessionId: [%u]", nSessionId);
        return;
    }
    // Erase old session id
    ms_mapConnectorBySessionId.erase(itFind);
}

bool CTCPConnectorManager::UpdateConnectorSessionId( CTCPConnector* pConnector )
{
    // Check pointer
    if (nullptr == pConnector)
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "nullptr == pConnector");
        return false;
    }
    // Update session id
    return ms_mapConnectorBySessionId.insert(std::make_pair(pConnector->GetSessionId(), pConnector)).second;
}

bool CTCPConnectorManager::HandleConnectorSessionError( AUINT32 nSessionId )
{
    // 1. Maybe the session has been recycled!
    if (0 == nSessionId)
    {
        return false;
    }
    // 2. Find session by id
    auto itFind = ms_mapConnectorBySessionId.find(nSessionId);
    if (ms_mapConnectorBySessionId.end() == itFind)
    {
        // Maybe the connector has been removed!
        return true;
    }
    // 3. Process error routine
    CTCPConnector* pConnector = itFind->second;
    if (nullptr == pConnector)
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "nullptr == pConnector, SessionId: [%u]", nSessionId);
        return false;
    }
    // 4. Process session error on connector
    pConnector->OnConnectorSessionError();
    return true;
}

void CTCPConnectorManager::InsertReConnectList( CTCPConnector* pConnector )
{
    if (nullptr == pConnector)
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "nullptr == pConnector");
        return;
    }
    RemoveConnectorSessionId(pConnector->GetSessionId());
    ms_setOfflineConnector.insert(pConnector);
}

CTCPConnector* CTCPConnectorManager::GetRemoteServerConnector( AUINT32 nServerId )
{
    auto itFind = ms_mapConnectorByServerId.find(nServerId);
    if (itFind == ms_mapConnectorByServerId.end())
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "Invalid server id: [%d]", nServerId);
        return nullptr;
    }
    return itFind->second;
}

AUINT32 CTCPConnectorManager::GetRemoteServerUpTime( AUINT32 nServerId )
{
    CTCPConnector* pTCPConnector = GetRemoteServerConnector(nServerId);
    if (nullptr == pTCPConnector)
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "ServerId: [%d] nullptr == pTCPConnector", nServerId);
        return INVALID_AUINT32;
    }
    return pTCPConnector->GetServerUpTime();
}

bool CTCPConnectorManager::OnRemoteServerUp( const ServerItem& stServerItem )
{
    if (nullptr == ms_pAquariusServer)
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "Impossible: nullptr == ms_pAquariusServer");
        return false;
    }
    return ms_pAquariusServer->OnRemoteServerUp(stServerItem);
}

bool CTCPConnectorManager::OnRemoteServerDown( const ServerItem& stServerItem )
{
    if (nullptr == ms_pAquariusServer)
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "Impossible: nullptr == ms_pAquariusServer");
        return false;
    }
    return ms_pAquariusServer->OnRemoteServerDown(stServerItem);
}

