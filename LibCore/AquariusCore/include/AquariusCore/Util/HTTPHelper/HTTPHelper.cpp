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
 * FileName: HTTPHelper.cpp
 * 
 * Purpose:  Implement HTTPHelper
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/2/22 15:35
===================================================================+*/
#include "AquariusCore/Util/HTTPHelper/HTTPHelper.h"
#include "AquariusCore/Logger/AquariusLogger.h"
#include <fstream>

void CHTTPHelper::InitCURLLib()
{
    curl_global_init(CURL_GLOBAL_ALL);
}

void CHTTPHelper::CleanupCURLLib()
{
    curl_global_cleanup();
}

std::string CHTTPHelper::CURLEscape( const char* pParam )
{
    char* pResult = curl_easy_escape(nullptr, pParam, 0);
    std::string strResult = pResult;
    curl_free(pResult);
    return strResult;
}


int CHTTPHelper::OnDebugCurl( CURL* /*pCurl*/, curl_infotype nType, char* pData, AUINT32 /*nSize*/, void* /*pParam*/ )
{
    char buff[32] = { 0 };
    a_sprintf(buff, "%d", nType);
    std::string strMsg = buff;
    switch (nType)
    {
    case CURLINFO_TEXT:
        strMsg = "CURLINFO_TEXT";
        break;
    case CURLINFO_HEADER_IN:
        strMsg = "CURLINFO_HEADER_IN";
        break;
    case CURLINFO_HEADER_OUT:
        strMsg = "CURLINFO_HEADER_OUT";
        break;
    case CURLINFO_DATA_IN:
        strMsg = "CURLINFO_DATA_IN";
        break;
    case CURLINFO_DATA_OUT:
        strMsg = "CURLINFO_DATA_OUT";
        break;
    case CURLINFO_SSL_DATA_IN:
        strMsg = "CURLINFO_SSL_DATA_IN";
        break;
    case CURLINFO_SSL_DATA_OUT:
        strMsg = "CURLINFO_SSL_DATA_OUT";
        break;
    default:
        break;
    }
    // Control the debug data
    std::string strData = pData;
    if (strData.size() > 256)
    {
        strData.resize(256);
    }
    CAquariusLogger::Logger(LL_DEBUG_FUN, "[%s][%s]", strMsg.c_str(), strData.c_str());
    return 0;
}

AUINT32 CHTTPHelper::WriteDataCallBack(void* pContents, AUINT32 nSize, AUINT32 nMemblock, void* pReceiver)
{
    // 1. Check user buff
    AUINT32 nRealSize = (AUINT32)(nSize * nMemblock);
    ICURLDataReceiver* pDataReceiver = (ICURLDataReceiver*)pReceiver;
    if (nullptr == pDataReceiver)
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "nullptr == pMemoryCache!");
        return 0;
    }
    // 2. Try to write data to memory pool, if success, just return
    if (!pDataReceiver->WriteReceivedData((const char*)pContents, nRealSize))
    {
        CAquariusLogger::Logger(LL_WARNING_FUN, "Failed on pDataReceiver->WriteReceivedData");
        return 0;
    }
    return nRealSize;
}

bool CHTTPHelper::OpenDataReceiver( const char* pszTargetFile, ICURLDataReceiver*& pReceiver )
{
    // Reset all data receiver
    m_hFileReceiver.Reset();
    m_hMemoryReceiver.Reset();
    // Check target file content
    pReceiver = nullptr;
    if (nullptr != pszTargetFile && pszTargetFile[0] != '\0' && m_hFileReceiver.OpenReceiver(pszTargetFile))
    {
        pReceiver = &m_hFileReceiver;
        return true;
    }
    // Open memory receiver
    if (m_hMemoryReceiver.OpenReceiver(nullptr))
    {
        pReceiver = &m_hMemoryReceiver;
        return true;
    }
    CAquariusLogger::Logger(LL_ERROR_FUN, "Failed to open data receiver");
    return false;
}

bool CHTTPHelper::CloseDataReceiver(ICURLDataReceiver* pReceiver)
{
    if (nullptr == pReceiver)
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "nullptr == pReceiver");
        return false;
    }
    if (!pReceiver->CloseReceiver())
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "pReceiver->CloseReceiver failed");
        return false;
    }
    return true;
}


bool CHTTPHelper::IsSecurityURL(const std::string& strURL) const
{
    const static std::string CONST_HTTPS_HEAD = "https";
    return (0 == strncmp(strURL.c_str(), CONST_HTTPS_HEAD.c_str(), CONST_HTTPS_HEAD.length()));
}

bool CHTTPHelper::ProcessHTTPRequest(const CHTTPHelper::HTTPRequest& stRequest)
{
    // 1. Reset memory cache
    ICURLDataReceiver* pReceiver;
    if (!OpenDataReceiver(stRequest.strTargetFile.c_str(), pReceiver))
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "OpenDataReceiver failed");
        return false;
    }
    // 2. Init curl
    CURL* pCurl = curl_easy_init();
    if (nullptr == pCurl)
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "nullptr == pCurl");
        CloseDataReceiver(pReceiver);
        return false;
    }
    // 3. Check debug flag
    if (m_bDebugFlag)
    {
        curl_easy_setopt(pCurl, CURLOPT_VERBOSE, 1);
        curl_easy_setopt(pCurl, CURLOPT_DEBUGFUNCTION, CHTTPHelper::OnDebugCurl);
    }
    else
    {
        curl_easy_setopt(pCurl, CURLOPT_CONNECTTIMEOUT, 5);
        curl_easy_setopt(pCurl, CURLOPT_TIMEOUT, 5);
    }

    // 4. send all data to this function 
    curl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION, CHTTPHelper::WriteDataCallBack);
    // 5. we pass our 'chunk' struct to the callback function 
    curl_easy_setopt(pCurl, CURLOPT_WRITEDATA, (void*)pReceiver);
    // 6. Set cookie
    if (stRequest.strCookie.empty())
    {
        curl_easy_setopt(pCurl, CURLOPT_COOKIE, stRequest.strCookie.c_str());
    }
    // 7. Set curl parameter
    curl_easy_setopt(pCurl, CURLOPT_URL, stRequest.strURL.c_str());
    curl_easy_setopt(pCurl, CURLOPT_READFUNCTION, nullptr);
    // While multi-thread use time-out mechanism, and main thread called sleep() or wait(),
    // libcurl will receive a single to quit if you did not set this option.
    curl_easy_setopt(pCurl, CURLOPT_NOSIGNAL, 1);
    // 8. security flag
    if (IsSecurityURL(stRequest.strURL))
    {
        curl_easy_setopt(pCurl, CURLOPT_SSL_VERIFYPEER, false);
        curl_easy_setopt(pCurl, CURLOPT_SSL_VERIFYHOST, false);
    }
    // 9. Post flag
    if (stRequest.bIsPost)
    {
        curl_easy_setopt(pCurl, CURLOPT_POST, 1);
        curl_easy_setopt(pCurl, CURLOPT_POSTFIELDS, stRequest.strPostParam.c_str());
        curl_easy_setopt(pCurl, CURLOPT_POSTFIELDSIZE, stRequest.strPostParam.length());
    }
    // 10. Check res code
    CURLcode nResCode = curl_easy_perform(pCurl);
    curl_easy_cleanup(pCurl);
    CloseDataReceiver(pReceiver);
    if (CURLE_OK != nResCode)
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "ResCode: [%d], url: [%s]", nResCode, stRequest.strURL.c_str());
        return false;
    }
    return true;
}

bool CHTTPHelper::Get(const std::string& strURL, const char* pszCookie/* = nullptr*/)
{
    HTTPRequest stRequest;
    stRequest.strURL = strURL;
    if (nullptr != pszCookie)
    {
        stRequest.strCookie = pszCookie;
    }
    return ProcessHTTPRequest(stRequest);
}

bool CHTTPHelper::Post(const std::string& strURL, const char* pszPostParam, const char* pszCookie /*= nullptr*/)
{
    HTTPRequest stRequest;
    stRequest.strURL = strURL;
    stRequest.bIsPost = true;
    if (nullptr != pszPostParam)
    {
        stRequest.strPostParam = pszPostParam;
    }
    if (nullptr != pszCookie)
    {
        stRequest.strCookie = pszCookie;
    }
    return ProcessHTTPRequest(stRequest);
}
