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
 * FileName: Dumper.h
 * 
 * Purpose:  Write dump file, only work on windows.
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/2/22 15:33
===================================================================+*/
#ifdef WIN32
// This class base on win32
#ifndef _DUMPER_H_
#define _DUMPER_H_

#pragma warning(push)
#pragma warning(disable:4091)
#include <imagehlp.h>
#pragma warning(pop)
#include <string>
#include <fstream>

class MiniDumper 
{
public:
    /**
     * Description: Init dumper
     * Parameter const std::string & strServerName  [in]: server name
     * Parameter int nServerId                      [in]: server id
     * Returns void:
     */
    static void Initialize(const std::string& strServerName, int nServerId);

private:
    // Filter exception
    static LONG WINAPI TopLevelFilter( struct _EXCEPTION_POINTERS *pExceptionInfo );

    // Build dump file name
    static std::string BuildDumpFileName();

    // Write log
    static void SelfCriticalLogger(const char* pszLogMsg);

private:
    // App name
    static std::string ms_strAppName;
    // App Id
    static int ms_nAppId;
    // log stream
    static std::ofstream ms_ofsLog;

};
#endif // _DUMPER_H_

#endif // WIN32
