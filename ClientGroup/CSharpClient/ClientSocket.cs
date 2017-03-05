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
 * FileName: ClientSocket.cs
 * 
 * Purpose:  Client socket, connect to server
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/3/5 16:38
===================================================================+*/
using System;
using System.Collections.Generic;
using System.IO;
using System.Net;
using System.Net.Sockets;
using System.Threading;

namespace NetIOLayer
{
    class ClientSocket 
    {
        // server ip
        private string m_host;
        // server port
        private int m_port;
        // socket object
        private Socket m_socket;
        
        // send thread
        private Thread m_send_thread;
        // recv thread
        private Thread m_recv_thread;
        // conn thread
        private Thread m_conn_thread;

        // send queue
        private Queue<Message> m_send_queue;
        // recv queue
        private Queue<Message> m_recv_queue;

        // recv stream
        private MemoryStream m_recv_stream;
        // recv buff
        private byte[] m_recv_buff = new byte[65536];
        // recv stream pos
        private long m_recv_stream_pos = 0;
        // event object
        private System.Threading.ManualResetEvent m_event_wait_send = new ManualResetEvent(true);

        // Awake the send queue
        public void Awake()
        {
            m_send_queue = new Queue<Message>();
        }

        // Get message from queue
        public Message GetMessage()
        {
            Message msg = null;
            if (m_recv_queue != null && m_recv_queue.Count > 0)
            {
                lock (m_recv_queue)
                {
                    msg = m_recv_queue.Dequeue();
                    CSLogger.LogDebug("Receive msg:" + msg.ToString());
                }
            }
            return msg;
        }

        // Send msg
        public void SendMsg(Message msg)
        {
            if (!IsConnected())
            {
                CSLogger.LogError("GateServer is disconnected when send " + msg.ToString());
                return;
            }
            CSLogger.LogNotice("Send Msg: " + msg.ToString());
            lock (m_send_queue)
            {
                m_send_queue.Enqueue(msg);
            }
            m_event_wait_send.Set();
        }

        // disconnect
        public void DisConnect(bool dispose)
        {
            if (m_send_thread != null)
            {
                m_send_thread.Abort();
                m_send_thread = null;
            }

            if (m_recv_thread != null)
            {
                m_recv_thread.Abort();
                m_recv_thread = null;
            }

            if (m_conn_thread != null)
            {
                m_conn_thread.Abort();
                m_conn_thread = null;
            }

            if (m_recv_queue != null)
            {
                lock (m_recv_queue)
                {
                    m_recv_queue.Clear();
                }
                m_recv_queue = null;
            }

            if (dispose)
            {
                if (m_send_queue != null)
                {
                    lock (m_send_queue)
                    {
                        m_send_queue.Clear();
                    }
                }
            }

            if (m_socket != null)
            {
                m_socket.Close();
                m_socket = null;
            }
        }

        // get connect status
        public bool IsConnected()
        {
            return (null != m_socket && m_socket.Connected);
        }

        // connect to a server
        public void Connect(string host, int port)
        {
            DisConnect(false);
              
            m_recv_queue = new Queue<Message>();

            m_host = host;
            m_port = port;

            try
            {
                m_conn_thread = new Thread(new ThreadStart(ConnThread));
                m_conn_thread.Start();
            }
            catch (Exception /*e*/)
            {
                CSLogger.LogError("Connect: Exception");
            }
        }

        // conn thread function
        private void ConnThread()
        {
            CSLogger.LogDebug("Enter ConnThread");
            try
            {
                IPAddress ip_address = null;
                IPAddress[] ip_array = Dns.GetHostAddresses(m_host);
                foreach (IPAddress ip in ip_array)
                {
                    if (ip.AddressFamily == AddressFamily.InterNetwork)
                    {
                        ip_address = ip;
                        break;
                    }
                }
                IPEndPoint ip_end_point = new IPEndPoint(ip_address, m_port);
                m_socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
                m_socket.Blocking = true;
                m_socket.Connect(ip_end_point);
                m_recv_stream = new MemoryStream();
                m_recv_thread = new Thread(new ThreadStart(RecvThread));
                m_send_thread = new Thread(new ThreadStart(SendThread));
                m_recv_thread.Start();
                m_send_thread.Start();
                m_conn_thread = null;
            }
            catch (System.Exception ex)
            {
                CSLogger.LogError(ex.ToString());
            }
            
            CSLogger.LogDebug("Exit ConnThread");
        }

        // recv thread function
        private void RecvThread()
        {
            CSLogger.LogDebug("Enter RecvThread");
            try
            {
                while (true)
                {
                    if (m_socket == null || m_socket.Connected == false)
                    {
                        m_recv_thread = null;
                        break;
                    }
                    ParseProcotol();
                    System.Threading.Thread.Sleep(10);
                }
            }
            catch (SocketException /*e*/)
            {
                CSLogger.LogDebug("RecvThread: SocketException ");
            }
            catch (Exception /*e*/)
            {
                CSLogger.LogDebug("RecvThread: Exception ");
            }
            CSLogger.LogDebug("Exit RecvThread");
        }

        // send thread function
        private void SendThread()
        {
            CSLogger.LogDebug("Enter SendThread");
            try
            {
                Message msg = null;
                while (true)
                {
                    if (m_socket == null || m_socket.Connected == false)
                    {
                        m_send_thread = null;
                        break;
                    }
                    m_event_wait_send.WaitOne();
                    m_event_wait_send.Reset();

                    lock (m_send_queue)
                    {
                        while (m_send_queue.Count > 0)
                        {
                            msg = m_send_queue.Peek();
                            byte[] bytes = msg.ToBytes();
                            int result = 0;
                            int byte_size = bytes.Length;
                            while (result != byte_size)
                            {
                                try
                                {
                                    result += m_socket.Send(bytes, result, byte_size - result, SocketFlags.None);
                                }
                                catch (System.Exception ex)
                                {
                                    CSLogger.LogError(ex.ToString());
                                }
                                
                            }
                            m_send_queue.Dequeue();
                        }
                    }
                    System.Threading.Thread.Sleep(10);
                }
            }
            catch (SocketException /*e*/)
            {
                CSLogger.LogDebug("SendThread: SocketException ");
            }
            catch (Exception /*e*/)
            {
                CSLogger.LogDebug("SendThread: Exception ");
            }
            CSLogger.LogDebug("Exit SendThread");
        }

        // Parse a protocol.
        private void ParseProcotol()
        {
            // Check recv len
            int socket_recv_len = m_socket.Receive(m_recv_buff, SocketFlags.None);
            if (socket_recv_len <= 0)
            {
                return;
            }
            // Check pos
            m_recv_stream.Position = m_recv_stream.Length;
            m_recv_stream.Write(m_recv_buff, 0, socket_recv_len);
            m_recv_stream.Position = m_recv_stream_pos;
            while (m_recv_stream.Length - m_recv_stream.Position >= Message.MSG_BASIC_LEN)
            {
                m_recv_stream_pos = m_recv_stream.Position;
                m_recv_stream.Read(m_recv_buff, 0, Message.MSG_BASIC_LEN);
                MBinaryReader mbr = new MBinaryReader(m_recv_buff);

                int msg_len = mbr.ReadUInt16();
                if (m_recv_stream.Length - m_recv_stream.Position >= msg_len - Message.MSG_BASIC_LEN)
                {
                    m_recv_stream_pos = 0;
                    m_recv_stream.Read(m_recv_buff, 0, msg_len - Message.MSG_BASIC_LEN);
                    mbr = new MBinaryReader(m_recv_buff);
                    ushort msg_id = mbr.ReadUInt16();
                    Message msg = MessageFactory.CreateMessageById(msg_id);
                    if (null == msg)
                    {
                        CSLogger.LogNotice("Invalid msg id: " + msg_id);
                    }
                    if (!msg.FromBytes(mbr))
                    {
                        CSLogger.LogNotice("ParseProcotol: Fail to parse msg: " + msg.ToString());
                    }
                    if (m_recv_queue != null)
                    {
                        lock (m_recv_queue)
                        {
                            m_recv_queue.Enqueue(msg);
                        }
                    }
                }
                else
                {
                    m_recv_stream.Position = m_recv_stream_pos;
                    break;
                }
                mbr.Close();
                mbr = null;
            }
            if (m_recv_stream.Position == m_recv_stream.Length)
            {
                m_recv_stream.SetLength(0);
            }
        }
    }
}
