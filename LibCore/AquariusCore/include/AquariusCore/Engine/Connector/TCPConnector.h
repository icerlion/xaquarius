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
 * FileName: TCPConnector.h
 * 
 * Purpose:  A TCPConnector object hold a session which connected to
 *           remote cooperative server, you can send msg to remote
 *           by TCPConnector.
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/2/22 15:28
===================================================================+*/
#ifndef _TCP_CONNECTOR_H_
#define _TCP_CONNECTOR_H_
#include "boost/asio.hpp"
#include "boost/bind.hpp"
#include "AquariusCore/Engine/Session/TCPSession.h"
#include "AquariusCore/Engine/Config/ServerGroupConfig.h"
#include "AquariusCore/Util/TypeReDef/TypeReDef.hpp"
#include "AquariusCore/Logger/AquariusLogger.h"

// Server state
enum class EConnectorState : int
{
    ServerDown = 0,
    ServerUp = 1,
};

class CTCPConnector
{
public:
    // Constructor
    CTCPConnector();

    // Send type msg to this server
    template<typename MsgType>
    bool SendBinaryMsg(const MsgType& stMsg)
    {
        // 1. Check available
        if (EConnectorState::ServerUp != m_nServerState)
        {
            CAquariusLogger::Logger(LL_DEBUG_FUN, "[%s] is down, msg name: [%s]", m_stRemoteServerConfig.strName.c_str(), MsgType::GetMsgName());
            return false;
        }
        // 2. Check session pointer
        if (nullptr == m_pTCPSession)
        {
            CAquariusLogger::Logger(LL_ERROR_FUN, "[%s] is logic error, nullptr == m_pTCPSession, msg name: [%s]", m_stRemoteServerConfig.strName.c_str(), MsgType::GetMsgName());
            return false;
        }
        // 3. Send msg by session
        return m_pTCPSession->SendBinaryMsg(stMsg);
    }

    // Destructor
    ~CTCPConnector();

    // Init by remote server item config
    bool Initialize(const ServerItem& stConfig);

    // Open connector
    bool OpenConnector();

    // Get server type / id
    inline AUINT32 GetConnectorServerId() const
    {
        return m_stRemoteServerConfig.nId;
    }

    // Get session id of this connector
    AUINT32 GetSessionId() const;

    // Process connect failed event
    void OnConnectorSessionError();

    // Called by CAquariusServer::HandleSendSSHeartBeatMsg
    template<typename SSMsgType>
    bool SendSSHeartBeatMsg()
    {
        SSMsgType stSSMsg;
        // Assign ServerType by group id or type
        if ((AUINT16)EAquariusServerType::GateServer == m_nSelfServerType && EAquariusServerType::HubServer == m_stRemoteServerConfig.nServerType)
        {
            // GateToHub, assign group id
            stSSMsg.server_type = (AUINT16)CServerGroupConfig::GetServerGroupId();
        }
        else
        {
            // Other server, assign self server type / id
            stSSMsg.server_type = m_nSelfServerType;
        }
        stSSMsg.heartbeat_index = m_nHeartBeatIndex;
        if (!SendBinaryMsg(stSSMsg))
        {
            return false;
        }
        if (0 == m_nHeartBeatIndex % 10)
        {
            CAquariusLogger::Logger(LL_DEBUG_FUN, "Output HB [%hu] to [%s]", stSSMsg.heartbeat_index, m_stRemoteServerConfig.strName.c_str());
        }
        ++m_nHeartBeatIndex;
        if (0 == m_nHeartBeatIndex)
        {
            m_nHeartBeatIndex = 1;
        }
        return true;
    }

    // Get server up time
    inline AUINT32 GetServerUpTime()
    {
        return m_nServerUpTime;
    }

private:
    // Handle connected event
    bool HandleConnect(const boost::system::error_code& stErrorCode);

private:
    // Self Server Type
    AUINT16 m_nSelfServerType;
    // Server config
    ServerItem m_stRemoteServerConfig;
    // Server state
    EConnectorState m_nServerState;
    // Remote end point
    boost::asio::ip::tcp::endpoint m_stEndPoint;
    // Session pointer
    CTCPSession* m_pTCPSession;
    // Heart beat index for output
    AUINT16 m_nHeartBeatIndex;
    // Server Up Time, 0xffffffff if server down
    AUINT32 m_nServerUpTime;
};

#define RETURN_ON_NULL_TCP_CONNECTOR(RET_VAL) if (nullptr == pTCPConnector) { CAquariusLogger::Logger(LL_ERROR_FUN, "Impossible: nullptr == pTCPConnector"); return RET_VAL; }

#endif // _TCP_CONNECTOR_H_
