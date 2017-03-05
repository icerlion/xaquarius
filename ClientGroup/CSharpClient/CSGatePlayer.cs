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
 * FileName: csgateplayer.cs
 * 
 * Purpose:  Define Player
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/3/5 16:38
===================================================================+*/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using NetIOLayer;
using System.Globalization;

namespace NetIOLayer
{
    class CSGatePlayer : NetBeanBase
    {
        // Constructor by phone no
        public CSGatePlayer( )
        {
        }

        // run this player
        public void RunGatePlayer(string strSrvIP, int nSrvPort)
        {
            if (!Init(strSrvIP, nSrvPort))
            {
                return;
            }
            AsyncConnect();
            // Max try count: 1000
            DateTime loop_begin_time = DateTime.Now;
            for (int i = 0; i < 1000; ++i)
            {
                if (IsServerConnected())
                {
                    OnAsyncConnectSuccess();
                    break;
                }
                System.Threading.Thread.Sleep(2);
                TimeSpan time_span = DateTime.Now - loop_begin_time;
                if (time_span.TotalSeconds >= 3.0f)
                {
                    break;
                }
            }
            if (!IsServerConnected())
            {
                CSLogger.LogNotice("Connected failed");
                OnAsyncConnectFailed();
                return;
            }
            // Main loop
            while (true)
            {
                Update();
            }
        }

        // Default handle of msg: GC_HeartBeat_Msg
        protected override bool OnGC_HeartBeat_Msg(GC_HeartBeat_Msg msg)
        {
            CSLogger.LogNotice("Receive HB: " + msg.heartbeat_index);
            return false;
        }
    }
}
