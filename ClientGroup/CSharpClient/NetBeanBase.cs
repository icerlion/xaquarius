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
 * FileName: netbeanbase.cs
 * 
 * Purpose:  Define base class for net msg handler
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/3/5 16:40
===================================================================+*/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace NetIOLayer
{
    public class NetBeanBase : ServerMsgHandler
    {
        protected string m_server_ip;                   // GateServer IP
        protected int m_server_port;                    // GateServer Port
        private ClientSocket m_server_socket;           // Socket of connect to gate
        protected UInt16 m_heartbeat_index;             // Heart beat index
        protected DateTime m_send_heartbeat_time;       // Time of send heart beat

        // Init bean, support server address
        public bool Init(string strSrvIP, int nSrvPort)
        {
            CSLogger.Init();
            m_server_ip = strSrvIP;
            m_server_port = nSrvPort;
            return true;
        }

        // Return true if server is connected
        public bool IsServerConnected()
        {
            if (null == m_server_socket)
            {
                return false;
            }
            return m_server_socket.IsConnected();
        }

        // Async connect
        public void AsyncConnect()
        {
            Disconnect();
            m_server_socket = new ClientSocket();
            m_server_socket.Awake();
            m_server_socket.Connect(m_server_ip, m_server_port);
        }

        // Called after connect failed
        public void OnAsyncConnectFailed()
        {
            m_heartbeat_index = 0;
            m_send_heartbeat_time = DateTime.UtcNow;
            Disconnect();
        }

        // Called after connect success
        public void OnAsyncConnectSuccess()
        {
            CSLogger.LogNotice("Success to connect " + m_server_ip + ":" + m_server_port);
            m_heartbeat_index = 0;
            m_send_heartbeat_time = DateTime.UtcNow;
        }

        // Disconnect
        public void Disconnect()
        {
            CSLogger.LogNotice("Disconnect " + m_server_ip + ":" + m_server_port);
            if (null != m_server_socket)
            {
                m_server_socket.DisConnect(true);
            }
        }

        // Send msg to server
        public void SendMsgToServer(Message msg)
        {
            if (m_server_socket == null)
            {
                return;
            }
            if (!m_server_socket.IsConnected())
            {
                CSLogger.LogNotice("Server Is DisConnected");
                return;
            }
            if (null == msg)
            {
                CSLogger.LogError("SendMsgToServer: null == msg");
                return;
            }
            if (null == m_server_socket)
            {
                CSLogger.LogError("SendMsgToServer: null == m_gs_socket");
                return;
            }
            CSLogger.LogDebug("SendMsgToServer: " + msg.ToString());
            m_server_socket.SendMsg(msg);
        }

        // Called this function in loop
        public void Update()
        {
            if (null == m_server_socket)
            {
                return;
            }
            if (!m_server_socket.IsConnected())
            {
                CSLogger.LogError("Socket is disconnected, please re-connect it");
                return;
            }
            Message msg = m_server_socket.GetMessage();
            base.DispatchMsg(msg);
            // Try to Send heart beat msg
            TimerSendHeartBeatMsg();
        }

        // Send heart beat msg
        private void TimerSendHeartBeatMsg()
        {
            DateTime utcNow = DateTime.UtcNow;
            TimeSpan elapsedTime = utcNow - m_send_heartbeat_time;
            // Send heart beat msg per 5 seconds
            if (elapsedTime.TotalSeconds >= 5)
            {
                CG_HeartBeat_Msg stHBMsg = new CG_HeartBeat_Msg();
                stHBMsg.heartbeat_index = m_heartbeat_index;
                SendMsgToServer(stHBMsg);
                ++m_heartbeat_index;
                m_send_heartbeat_time = utcNow;
            }
        }
    }
}
