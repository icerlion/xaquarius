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
 * FileName: TCPListener.h
 * 
 * Purpose:  A TCPListener can open a TCP port to accept TCP connection
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/2/22 15:59
===================================================================+*/
#ifndef _TCP_LISTENER_H_
#define _TCP_LISTENER_H_
#include "boost/asio.hpp"
#include "AquariusCore/Engine/Config/ServerItemDef.hpp"

class CTCPSession;
class CTCPListener
{
public:
    // Constructor
    CTCPListener();

    // Destructor
    ~CTCPListener(void);

    // Open listener by config
    bool OpenListener(const ListenerConfig& stListenerConfig);

    // Close this listener, 
    bool CloseListener();

    // Start accept asynchronous
    bool StartAsynAccept();

private:
    // Open TCP port
    bool OpenTCPPort();

    /**
     * Description: CallBack function, called when accept a connection
     * Parameter CTCPSession * pNewSession                     [in]: new session pointer
     * Parameter const boost::system::error_code & error_code  [in]: error code
     * Returns void:
     */
    void HandleAsynAccept(CTCPSession* pNewSession, const boost::system::error_code& error_code);

private:
    // Listener config
    ListenerConfig m_stListenerConfig;
    // TCP acceptor
    boost::asio::ip::tcp::acceptor m_boostAcceptor;
    // flag of call async_accept
    bool m_bInAsyncAccept;
};

#endif // _TCP_LISTENER_H_
