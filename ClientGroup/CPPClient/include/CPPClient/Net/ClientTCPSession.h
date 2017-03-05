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
 * FileName: ClientTCPSession.h
 * 
 * Purpose:  Hold session between client and server
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/3/5 16:31
===================================================================+*/
#ifndef _CLIENT_TCPSESSION_H_
#define _CLIENT_TCPSESSION_H_

#include "boost/asio.hpp"
#include "AquariusCore/Engine/Session/TCPSession.h"
#include "AquariusCore/Engine/Msg/ReceivedDataHandler.h"
#include "AquariusCore/Util/TypeReDef/TypeReDef.hpp"
#include <string>
#include "Protocol/CSProtocol.hpp"
#include "boost/thread.hpp"
#include "boost/chrono.hpp"

class CClientTCPSession : public CTCPSession
{
public:
    // Define session state
    enum ESessionState
    {
        Session_OnLine,
        Session_InError,
        Session_OffLine,
    };

    // Constructor
    CClientTCPSession(boost::asio::io_service& io_service, int nClientId);

    // Destructor
    virtual ~CClientTCPSession(void) override final;

    // Set server address
    void SetServerAddress(const std::string& strIP, int nPort);

    // Start this client
    void PollOnceIOService();

    // Statistic recv msg
    virtual void StatRecvMsg(AUINT16);

    // Send msg to server
    template<typename MsgStruct>
    bool SendTypeMsg(const MsgStruct& stMsg)
    {
        // Check session state
        if (Session_OnLine != m_nSessionState)
        {
            CAquariusLogger::Logger(LL_ERROR, "SendTypeMsg: invalid session state:[%d]", m_nSessionState);
            return false;
        }
        // Check socket state
        if (!m_boostTCPSocket.is_open())
        {
            SetSessionInError();
            CAquariusLogger::Logger(LL_ERROR, "SendTypeMsg: socket is closed");
            return false;
        }
        // Start write
        CAquariusLogger::Logger(LL_DEBUG, "SendMsg: %s", typeid(MsgStruct).name());
        MsgStruct stMsgCopy = stMsg;
        stMsgCopy.HostToNetStruct();
        boost::asio::async_write(m_boostTCPSocket,
            boost::asio::buffer((char*)(&stMsgCopy), sizeof(stMsgCopy)),
            boost::bind(&CClientTCPSession::HandleWrite, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
        StartAsynRead();
        return true;
    }

    // Get state of this session
    ESessionState GetSessionState() const;

    // Open this session
    bool OpenThisSession(bool bUserMutual);

    // Close this session
    bool CloseThisSession();

    // Session in error
    void SetSessionInError();

    // Implement pure virtual function
    virtual void UpdateRemoteEndPointString();
    
private:
    // Handle end of write 
    void HandleWrite( const boost::system::error_code& error_code, size_t bytes_transferred );

    // Destroy TCP socket
    void DestroyTCPSocket();
    
    /**
     * Description: Handle read data
     * Parameter const boost::system::error_code & error_code   [in]: error code
     * Parameter std::size_t bytes_transferred                  [in]: bytes transferred
     * Returns void:
     */
    void HandlerRead(const boost::system::error_code& error_code, std::size_t bytes_transferred);

    // Check the write speed
    void CheckWriteDataSpeed();

    // Start read async
    void StartAsynRead();

protected:
    // Export local end point
    std::string ExportLocalEndPoint(boost::asio::ip::tcp::socket& tcp_socket);

protected:
    std::string m_strServerIP;
    unsigned short m_nServerPort;
    boost::asio::io_service& m_boostIOService;
    boost::asio::ip::tcp::socket m_boostTCPSocket;
    CReceivedDataHandler m_hReceivedData;
    bool m_bInAysnReadData;
    ESessionState m_nSessionState;
};

#endif // _CLIENT_TCPSESSION_H_
