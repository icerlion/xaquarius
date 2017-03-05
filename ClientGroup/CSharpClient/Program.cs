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
 * FileName: program.cs
 * 
 * Purpose:  Enter of this application
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/3/5 16:40
===================================================================+*/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using NetIOLayer;

namespace NetIOLayer
{
    class Program
    {
         public static void Main(string[] args)
        {
            // Init logger
            CSLogger.Init();
            // Verify protocol.
            VerifyProtocolParse.RunTest();
            // Default assign server
            string strServerIP = "127.0.0.1";
            int nServerPort = 7001;
            // Create a player and run it
            CSGatePlayer hPlayer = new CSGatePlayer();
            hPlayer.RunGatePlayer(strServerIP, nServerPort);
        }
    }
}
