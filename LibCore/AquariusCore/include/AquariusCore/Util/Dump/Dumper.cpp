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
 * FileName: Dumper.cpp
 * 
 * Purpose:  Write dump file.
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/2/22 15:33
===================================================================+*/
#ifdef WIN32
#include "AquariusCore/Util/TinyUtil/CommonUtil.h"
#include <Windows.h>
#include "AquariusCore/Util/Dump/Dumper.h"
#include "boost/date_time.hpp"

std::string MiniDumper::ms_strAppName = "NA";
int MiniDumper::ms_nAppId = 0;
std::ofstream MiniDumper::ms_ofsLog;

void MiniDumper::SelfCriticalLogger(const char* pszLogMsg)
{
    // 1. Check ofs
    if (!ms_ofsLog.is_open())
    {
        ms_ofsLog.open(ms_strAppName + "-Critical.log");
    }
    std::string strTimePoint = boost::posix_time::to_iso_extended_string(boost::posix_time::microsec_clock::local_time());
    // 2. Format log msg time point
    if (strTimePoint.length() <= 10)
    {
        printf("strTimePoint.length() <= 10");
        return;
    }
    // 3. Output dump log
    strTimePoint[10] = ' ';
    AUINT32 nThreadId = CCommonUtil::GetThisThreadId();
    char buff[512] = { 0 };
    a_sprintf(buff, "[%s][0x%08X][DUMP] - %s\n", strTimePoint.c_str(), nThreadId, pszLogMsg);
    ms_ofsLog<<buff;
    ms_ofsLog.flush();
    printf("%s", buff);
}

void MiniDumper::Initialize(const std::string& strServerName, int nServerId)
{
    // Should be inited only once
    static bool s_bInited = false;
    if (s_bInited)
    {
        SelfCriticalLogger("ERROR: MiniDumper::Initialize, re-inited!");
    }
    else
    {
        ::SetUnhandledExceptionFilter( TopLevelFilter );
        ms_strAppName = strServerName;
        ms_nAppId = nServerId;
        s_bInited = true;
    }
}

// based on dbghelp.h
typedef BOOL (WINAPI *MINIDUMPWRITEDUMP)(HANDLE hProcess, DWORD dwPid, HANDLE hFile, MINIDUMP_TYPE DumpType,
    CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
    CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
    CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam
    );

LONG MiniDumper::TopLevelFilter( struct _EXCEPTION_POINTERS *pExceptionInfo )
{
    LONG nRetVal = EXCEPTION_CONTINUE_SEARCH;
    HMODULE hDll = nullptr;
    HANDLE hFile = INVALID_HANDLE_VALUE;
    do 
    {
        // 1. Find dll module
        char szDbgHelpPath[_MAX_PATH] = { 0 };
        std::string strModulePath;
        if (GetModuleFileName( nullptr, szDbgHelpPath, _MAX_PATH ))
        {
            strModulePath = szDbgHelpPath;
            size_t nPos = strModulePath.rfind('\\');
            if (nPos != std::string::npos)
            {
                strModulePath = strModulePath.substr(0, nPos + 1);
                strModulePath.append("\\DBGHELP.DLL");
                hDll = ::LoadLibrary( strModulePath.c_str() );
            }
        }
        if (hDll == nullptr)
        {
            // load any version we can
            hDll = ::LoadLibrary( "DBGHELP.DLL" );
        }
        if (nullptr == hDll)
        {
            SelfCriticalLogger("DBGHELP.DLL not found");
            break;
        }
        // 2. Find MiniDumpWriteDump
        MINIDUMPWRITEDUMP pDump = (MINIDUMPWRITEDUMP)::GetProcAddress( hDll, "MiniDumpWriteDump" );
        if (nullptr == pDump)
        {
            SelfCriticalLogger("DBGHELP.DLL too old");
            break;
        }
        // 3. build dump file name
        std::string strDumpFilePath = BuildDumpFileName();
        if (strDumpFilePath.empty())
        {
            SelfCriticalLogger("Invalid dump file name!");
            break;
        }
        // 4. Create dump file
        hFile = ::CreateFile( strDumpFilePath.c_str(), GENERIC_WRITE, FILE_SHARE_WRITE, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr );
        if (INVALID_HANDLE_VALUE == hFile)
        {
            std::string strLogMsg = "Failed to create dump file: ";
            strLogMsg.append(strDumpFilePath);
            SelfCriticalLogger(strDumpFilePath.c_str());
            break;
        }
        // 5. Write dump
        _MINIDUMP_EXCEPTION_INFORMATION stExInfo;
        stExInfo.ThreadId = ::GetCurrentThreadId();
        stExInfo.ExceptionPointers = pExceptionInfo;
        stExInfo.ClientPointers = FALSE;
        if (!pDump( GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpValidTypeFlags, &stExInfo, nullptr, nullptr ))
        {
            char buff[256] = { 0 };
            a_sprintf(buff, "Failed to write dump file: [%s], error code: [%u]", strDumpFilePath.c_str(), GetLastError());
            SelfCriticalLogger(buff);
            break;
        }
        // 6. Update return value
        SelfCriticalLogger("Success create dump file!");
        if (nullptr != pExceptionInfo && nullptr != pExceptionInfo->ExceptionRecord)
        {
            std::string strException = "ExceptionCode: ";
            char buffCode[32] = { 0 };
            sprintf_s(buffCode, "0x%08X", pExceptionInfo->ExceptionRecord->ExceptionCode);
            char buffFlag[32] = { 0 };
            sprintf_s(buffFlag, "%u", pExceptionInfo->ExceptionRecord->ExceptionFlags);
            strException.append(buffCode).append(",");
            strException.append("ExceptionFlags: ").append(buffFlag);
            SelfCriticalLogger(strException.c_str());
        }
        nRetVal = EXCEPTION_EXECUTE_HANDLER;
#pragma warning(push)
#pragma warning(disable: 4127)
    } while (false);
#pragma warning(pop)
    if (INVALID_HANDLE_VALUE != hFile)
    {
        CloseHandle(hFile);
    }
    if (nullptr != hDll)
    {
        FreeLibrary(hDll);
    }
    return nRetVal;
}

std::string MiniDumper::BuildDumpFileName()
{
    SYSTEMTIME stTimeInfo;
    ZeroMemory(&stTimeInfo, sizeof (stTimeInfo));
    GetLocalTime(&stTimeInfo);
    char pszBuff[MAX_PATH] = { 0 };
    // Format: ServerName-ServerId-Date-Time.dmp
    a_sprintf(pszBuff, sizeof(pszBuff),
        "%s-%d-%4d%02d%02d-%02d%02d%02d.dmp",
        ms_strAppName.c_str(),
        ms_nAppId,
        stTimeInfo.wYear, 
        stTimeInfo.wMonth, 
        stTimeInfo.wDay, 
        stTimeInfo.wHour,
        stTimeInfo.wMinute, 
        stTimeInfo.wSecond);
    return std::string(pszBuff);
}

#endif // WIN32
