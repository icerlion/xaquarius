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
 * FileName: TCPConnector.cpp
 * 
 * Purpose:  Implement TCPConnector
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/2/22 15:31
===================================================================+*/
#include "AquariusCore/Engine/Connector/TCPConnector.h"
#include "AquariusCore/Engine/Session/TCPSessionManager.h"
#include "AquariusCore/Engine/Connector/TCPConnectorManager.h"
#include "AquariusCore/Engine/Server/AquariusServer.h"

bool CTCPConnector::Initialize( const ServerItem& stConfig)
{
    m_nSelfServerType = (AUINT16)CAquariusServer::GetServerType();
    if (!stConfig.stPrivateListener.IsAvailable())
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "[%s] PrivateTCP is not available", stConfig.strName.c_str());
        return false;
    }
    // Assign config data
    m_stRemoteServerConfig = stConfig;
    // Parse ip address, the connector should connect to the private ip
    boost::system::error_code stErrorCode;
    boost::asio::ip::address ip_address = boost::asio::ip::address::from_string(stConfig.stPrivateListener.strListenerIP, stErrorCode);
    if (stErrorCode)
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "Failed to parse ip: [%s], error value: [%d]", m_stRemoteServerConfig.stPrivateListener.strListenerIP.c_str(), stErrorCode.value());
        return false;
    }
    m_stEndPoint.address(ip_address);
    m_stEndPoint.port(static_cast<unsigned short>(stConfig.stPrivateListener.nListenerPort));
    // Try to open connector
    if (!OpenConnector())
    {
        return false;
    }
    return true;
}

bool CTCPConnector::OpenConnector()
{
    // 1. Check session pointer
    m_nServerState = EConnectorState::ServerDown;
    m_nHeartBeatIndex = 0;
    if (nullptr != m_pTCPSession)
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "nullptr != m_pTCPSession");
        CTCPSessionPool::RecycleObject(m_pTCPSession);
    }
    // 2. Create TCP session
    m_pTCPSession = CTCPSessionManager::CreateTCPSession();
    if (nullptr == m_pTCPSession)
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "nullptr == m_pTCPSession");
        return false;
    }
    m_pTCPSession->SetFunctionHandleSessionError(&CTCPConnectorManager::HandleConnectorSessionError);
    // 3. Update session id
    CTCPConnectorManager::UpdateConnectorSessionId(this);
    // 4. Connect remote address
    m_pTCPSession->GetSocket().async_connect(m_stEndPoint, boost::bind(&CTCPConnector::HandleConnect, this, boost::asio::placeholders::error));
    CAquariusLogger::Logger(LL_DEBUG_FUN, "Try To Connect [%s-%d], address: [%s:%d], SessionId: [%u]", m_stRemoteServerConfig.strName.c_str(), m_stRemoteServerConfig.nId, m_stRemoteServerConfig.stPrivateListener.strListenerIP.c_str(), m_stRemoteServerConfig.stPrivateListener.nListenerPort, m_pTCPSession->GetSessionId());
    return true;
}

bool CTCPConnector::HandleConnect( const boost::system::error_code& stErrorCode)
{
    if (stErrorCode)
    {
        CAquariusLogger::Logger(LL_DEBUG_FUN, "%s connect error, address: [%s:%d], error, error code: [%d]", m_stRemoteServerConfig.strName.c_str(), m_stRemoteServerConfig.stPrivateListener.strListenerIP.c_str(), m_stRemoteServerConfig.stPrivateListener.nListenerPort, stErrorCode.value());
        // Process connect failed
        CTCPConnectorManager::HandleConnectorSessionError(GetSessionId());
        return true;
    }
    if (nullptr == m_pTCPSession)
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "%s connect accepted, address: [%s:%d] logic error, nullptr == m_pTCPSession", m_stRemoteServerConfig.strName.c_str(), m_stRemoteServerConfig.stPrivateListener.strListenerIP.c_str(), m_stRemoteServerConfig.stPrivateListener.nListenerPort);
        // Set to connected failed!
        CTCPConnectorManager::HandleConnectorSessionError(GetSessionId());
        return false;
    }
    // If connect success, just try to read remote stream
    m_nServerState = EConnectorState::ServerUp;
    // Set server up time
    m_nServerUpTime = (AUINT32)time(nullptr);
    m_nHeartBeatIndex = 0;
    m_pTCPSession->OnSessionConnected();
    CAquariusLogger::Logger(LL_NOTICE, "%s-%d Connect Success, Address: [%s:%d], SessionId: [%u]", m_stRemoteServerConfig.strName.c_str(), m_stRemoteServerConfig.nId, m_stRemoteServerConfig.stPrivateListener.strListenerIP.c_str(), m_stRemoteServerConfig.stPrivateListener.nListenerPort, m_pTCPSession->GetSessionId());
    CTCPConnectorManager::OnRemoteServerUp(m_stRemoteServerConfig);
    return true;
}

CTCPConnector::~CTCPConnector()
{
    if (nullptr != m_pTCPSession)
    {
        CTCPSessionManager::DestroyTCPSession(m_pTCPSession);
        m_pTCPSession = nullptr;
    }
}

void CTCPConnector::OnConnectorSessionError()
{
    // Check server state
    if (EConnectorState::ServerUp == m_nServerState)
    {
        CTCPConnectorManager::OnRemoteServerDown(m_stRemoteServerConfig);
    }
    // Update server state
    m_nServerState = EConnectorState::ServerDown;
    m_nServerUpTime = INVALID_AUINT32;
    m_nHeartBeatIndex = 0;
    // Add reconnect list
    AUINT32 nSessionId = 0;
    CTCPConnectorManager::InsertReConnectList(this);
    if (nullptr == m_pTCPSession)
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "nullptr == m_pTCPSession");
    }
    else
    {
        // release session
        nSessionId = m_pTCPSession->GetSessionId();
        CTCPSessionManager::DestroyTCPSession(m_pTCPSession);
        m_pTCPSession = nullptr;
    }
    if (INVALID_AUINT32 == CAquariusServer::GetShutDownServerTime())
    {
        CAquariusLogger::Logger(LL_WARNING, "%s-%d Connect Failed, ServerInfo: [%s:%d], SessionId: [%u]", m_stRemoteServerConfig.strName.c_str(), m_stRemoteServerConfig.nId, m_stRemoteServerConfig.stPrivateListener.strListenerIP.c_str(), m_stRemoteServerConfig.stPrivateListener.nListenerPort, nSessionId);
    }
}

AUINT32 CTCPConnector::GetSessionId() const
{
    if (nullptr == m_pTCPSession)
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "nullptr == m_pTCPSession, ServerName: [%s]", m_stRemoteServerConfig.strName.c_str());
        return 0;
    }
    return m_pTCPSession->GetSessionId();
}

CTCPConnector::CTCPConnector() 
    :m_nSelfServerType(0),
    m_nServerState(EConnectorState::ServerDown),
    m_pTCPSession(nullptr),
    m_nHeartBeatIndex(0),
    m_nServerUpTime(INVALID_AUINT32)
{

}
