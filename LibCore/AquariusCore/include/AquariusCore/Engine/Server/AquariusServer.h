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
 * FileName: AquariusServer.h
 * 
 * Purpose:  Aquarius Server, you can define your own Server class which 
 *           derived from CAquariusServer.
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/2/22 16:01
===================================================================+*/
#ifndef _AQUARIUS_SERVER_H_
#define _AQUARIUS_SERVER_H_
#include "AquariusCore/Engine/Listener/TCPListener.h"
#include "AquariusCore/Engine/Connector/TCPConnectorManager.h"

class CTCPConnector;
class CAquariusServer
{
public:
    // Start server
    bool StartThisServer();

    // Get io service reference
    static inline boost::asio::io_service& GetBoostIOService()
    {
        return ms_boostIOService;
    }

    // Get server group id
    static AUINT32 GetServerGroupId();

    // Get server type
    static inline EAquariusServerType GetServerType()
    {
        return ms_stServerConfig.nServerType;
    }

    // Get server name
    static inline const std::string& GetServerName()
    {
        return ms_stServerConfig.strName;
    }

    // Get server start time
    static inline time_t GetServerStartTime()
    {
        return ms_nServerStartTime;
    }

    // Get stop flag
    static bool IsServerInStop()
    {
        return !ms_bStartThisServer;
    }

    // Get server config
    static const ServerItem& GetServerConfig() 
    {
        return ms_stServerConfig;
    }

    // Stop this server, just set the start flag
    static void StopThisServer();

    // Set shutdown server time
    static bool SignalShutDownServerTime();

    // Get ShutDownServerTime
    static AUINT32 GetShutDownServerTime();

    // Delay ServerShutDownTime
    static void DelayShutDownServerTime(AUINT32 nDelayTime);

    // Send msg to gate server
    template<typename MsgType>
    static inline bool SendMsgToGateServer(const MsgType& stMsg)
    {
        return CTCPConnectorManager::SendMsgToRemoteServer(stMsg, (AUINT16)EAquariusServerType::GateServer);
    }

    // Send msg to fight server
    template<typename MsgType>
    static inline bool SendMsgToFightServer(const MsgType& stMsg)
    {
        return CTCPConnectorManager::SendMsgToRemoteServer(stMsg, (AUINT16)EAquariusServerType::FightServer);
    }

    // Send msg to hub server
    template<typename MsgType>
    static bool SendMsgToHubServer(const MsgType& stMsg)
    {
        return CTCPConnectorManager::SendMsgToRemoteServer(stMsg, (AUINT16)EAquariusServerType::HubServer);
    }

    // Send msg to gate server, only for HubServer
    template<typename MsgType>
    static inline bool SendMsgToGateServer(const MsgType& stMsg, AUINT32 nServerGroupId)
    {
        return CTCPConnectorManager::SendMsgToRemoteServer(stMsg, nServerGroupId);
    }

    // Process server down event
    virtual bool OnRemoteServerUp(const ServerItem& stServerItem);

    // Process server down event
    virtual bool OnRemoteServerDown(const ServerItem& stServerItem);

    // Send SSHeartBeat
    virtual bool TimerSendSSHeartBeatMsg(CTCPConnector* pTCPConnector) = 0;

    // Return true if this server is ready
    static bool IsServerReady();

protected:
    // Constructor
    CAquariusServer(EAquariusServerType nType);

    // Destructor
    virtual ~CAquariusServer();

    // Implement OpenThisServer
    bool OpenThisServer();

    // Implement CloseThisServer
    bool CloseThisServer();

    // Called by Initialize, the real server should implement this function
    virtual bool OnInit() = 0;

    // Called on CloseThisServer, the real server should implement this function
    virtual bool OnDestroy() = 0;

private:
    // Initialize this server
    bool Initialize();

    // Init self server config
    bool InitSelfServerConfig( );

    /**
     * Description: Handle signal value
     * Parameter const boost::system::error_code & nErrorCode   [in]: Result of operation.
     * Parameter int nSignalNumber                              [in]:Indicates which signal occurred.
     * Returns void
     */
    void HandleSignal(const boost::system::error_code& nErrorCode, int nSignalNumber);

protected:
    // Public TCP listener
    CTCPListener m_hPublicListener;
    // Private TCP listener
    CTCPListener m_hPrivateListener;
    // start this server flag
    static bool ms_bStartThisServer;
    // server config
    static ServerItem ms_stServerConfig;
    // boost io service
    static boost::asio::io_service ms_boostIOService;
    // boost signal set
    static boost::asio::signal_set ms_boostSignalSet;
    // Time point of stop this server
    static AUINT32 ms_nShutDownServerTime;
    // Server Start Time
    static time_t ms_nServerStartTime;
};

#endif // _TCP_SERVER_HPP_
