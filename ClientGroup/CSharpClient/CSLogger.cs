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
 * FileName: cslogger.cs
 * 
 * Purpose:  Logger util function
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/3/5 16:39
===================================================================+*/
// The macro USE_CSHARP_CLIENT only for NetIOLayer using, 
// DON'T open it in unity project
#define USE_CSHARP_CLIENT

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Globalization;
using System.Threading;
using System.IO;

#if USE_UNITY_ENGINE
using UnityEngine;
#endif

namespace NetIOLayer
{
    // enum: log level list
    public enum LogLevel : int
    {
        LL_DEBUG = 1,
        LL_NOTICE = 2,
        LL_ERROR = 3,
        LL_CRITICAL = 4,
    }
    
    class CSLogger
    {
        private static readonly object lock_console_obj = new object();
        private static readonly object lock_file_obj = new object();
        private static Dictionary<LogLevel, int> m_dicLogLevelConsole = new Dictionary<LogLevel, int>();
        private static Dictionary<LogLevel, int> m_dicLogLevelFile = new Dictionary<LogLevel, int>();

        // Init
        public static void Init()
        {
            if (0 != m_dicLogLevelConsole.Count())
            {
                return;
            }
            //m_dicLogLevelConsole.Add(LogLevel.LL_DEBUG, 1);
            m_dicLogLevelConsole.Add(LogLevel.LL_NOTICE, 1);
            m_dicLogLevelConsole.Add(LogLevel.LL_ERROR, 1);
            m_dicLogLevelConsole.Add(LogLevel.LL_CRITICAL, 1);
            m_dicLogLevelFile.Add(LogLevel.LL_DEBUG, 1);
            m_dicLogLevelFile.Add(LogLevel.LL_NOTICE, 1);
            m_dicLogLevelFile.Add(LogLevel.LL_ERROR, 1);
            m_dicLogLevelFile.Add(LogLevel.LL_CRITICAL, 1);
        }

        // Function of write log with different level
        public static void LogDebug(string msg)
        {
            Logger(LogLevel.LL_DEBUG, msg);
        }
        public static void LogNotice(string msg)
        {
            Logger(LogLevel.LL_NOTICE, msg);
        }
        public static void LogError(string msg)
        {
            Logger(LogLevel.LL_ERROR, msg);
        }
        public static void LogCritical(string msg)
        {
            Logger(LogLevel.LL_CRITICAL, msg);
        }

        // Write logger content
        private static void Logger(LogLevel level, string msg)
        {
            string strTime = "[" + DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss.fff", DateTimeFormatInfo.InvariantInfo) + "]";
            string strLevel = "[" + GetLevelName(level) + "]";
            string strThreadId = "[" + Thread.CurrentThread.ManagedThreadId + "]";
            string strConetent = strTime + strLevel + strThreadId + msg;
            lock (lock_console_obj)
            {
                WriteConsoleLog(level, strConetent);
            }
            lock (lock_file_obj)
            {
                WriteFileLog(level, strConetent);
            }
        }

        // Update console color
        private static void UpdateConsoleColor(LogLevel level)
        {
#if USE_CSHARP_CLIENT
            switch (level)
            {
                case LogLevel.LL_DEBUG:
                    Console.ForegroundColor = ConsoleColor.Gray;
                    break;
                case LogLevel.LL_NOTICE:
                    Console.ForegroundColor = ConsoleColor.Green;
                    break;
                case LogLevel.LL_ERROR:
                    Console.ForegroundColor = ConsoleColor.Red;
                    break;
                case LogLevel.LL_CRITICAL:
                    Console.ForegroundColor = ConsoleColor.DarkRed;
                    break;
                default:
                    Console.ForegroundColor = ConsoleColor.Yellow;
                    break;
            }
#endif
        }

        // Write console log
        private static void WriteConsoleLog(LogLevel level, string log)
        {
            if (!m_dicLogLevelConsole.ContainsKey(level))
            {
                return;
            }
            UpdateConsoleColor(level);
#if USE_CSHARP_CLIENT
            Console.WriteLine(log);
#else
            Debug.Log(log);
#endif

        }

        // Write log to file
        private static void WriteFileLog(LogLevel level, string log)
        {
            if (!m_dicLogLevelFile.ContainsKey(level))
            {
                return;
            }
            FileStream fs = new FileStream("client.log", FileMode.Append);
            StreamWriter sw = new StreamWriter(fs, Encoding.Default);
            sw.WriteLine(log);
            sw.Close();
            fs.Close();
        }

        // Get level name
        private static string GetLevelName(LogLevel level)
        {
            string strLevel = level.ToString("d");
            switch (level)
            {
                case LogLevel.LL_DEBUG:
                    strLevel = "Debug";
                    break;
                case LogLevel.LL_NOTICE:
                    strLevel = "Notice";
                    break;
                case LogLevel.LL_ERROR:
                    strLevel = "Error";
                    break;
                case LogLevel.LL_CRITICAL:
                    strLevel = "Critical";
                    break;
                default:
                    break;
            }
            return strLevel;
        }
    }
}
