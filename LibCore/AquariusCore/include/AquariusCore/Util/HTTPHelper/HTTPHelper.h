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
 * FileName: HTTPHelper.h
 * 
 * Purpose:  Define HTTPHelper, suport get/post method to access web
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/2/22 15:34
===================================================================+*/
#ifndef _HTTP_HELPER_H_
#define _HTTP_HELPER_H_
#include "curl/curl.h"
#include <string>
#include <fstream>
#include "AquariusCore/Util/MemoryCache/MemoryCache.hpp"

// Link lib for win32
#ifdef WIN32
#pragma comment(lib, "wldap32.lib")
#endif // WIN32

// Define base data receiver
class ICURLDataReceiver
{
public:
    // Constructor
    ICURLDataReceiver() 
    {
    }

    // Destructor
    virtual ~ICURLDataReceiver()
    {
    }

    // CallBack function, write received data
    virtual bool WriteReceivedData(const char* pszDataBuff, AUINT32 nDataLen) = 0;

    // Open this receiver
    virtual bool OpenReceiver(const char* pParam) = 0;

    // Close receiver
    virtual bool CloseReceiver() = 0;

    // Reset the data receiver
    virtual void Reset() = 0;
};

//////////////////////////////////////////////////////////////////////////
// Data receiver for curl, write data to memory
class CCURLMemoryReceiver : public ICURLDataReceiver
{
public:
    // Constructor
    CCURLMemoryReceiver()
    {
    }

    // Destructor
    virtual ~CCURLMemoryReceiver() override final
    {
    }

    // Implement WriteReceivedData
    virtual bool WriteReceivedData(const char* pszDataBuff, AUINT32 nDataLen) override final
    {
        // If failed write data to memory, just return false
        if (!m_hMemoryCache.WriteToPool(pszDataBuff, nDataLen))
        {
            CAquariusLogger::Logger(LL_WARNING_FUN, "MemoryPool is exhausted!");
            return false;
        }
        return true;
    }

    // Open receiver, just reset the memory
    virtual bool OpenReceiver(const char* /*pParam*/) override final
    {
        m_hMemoryCache.Reset();
        m_strReceiveResult.clear();
        return true;
    }

    // Close the receiver
    virtual bool CloseReceiver() override final
    {
        size_t nResultLen = strlen(GetDataPointer());
        if (0 == nResultLen)
        {
            CAquariusLogger::Logger(LL_ERROR_FUN, "0 == nResultLen");
            return false;
        }
        if (nResultLen > GetReceivedDataLen())
        {
            nResultLen = GetReceivedDataLen();
        }
        m_strReceiveResult.assign(GetDataPointer(), nResultLen);
        return true;
    }

    // Get memory data pointer
    inline const char* GetDataPointer() const
    {
        return m_hMemoryCache.memory;
    }

    // Get memory data len
    AUINT32 GetReceivedDataLen() const
    {
        return m_hMemoryCache.offset;
    }

    // Get result string
    inline const std::string& GetResultString() const
    {
        return m_strReceiveResult;
    }

    // Reset the data receiver
    virtual void Reset() override final
    {
        m_hMemoryCache.Reset();
        m_strReceiveResult.clear();
    }

private:
    const static AUINT32 CONST_CURL_MEMORY_CACHE_SIZE = 128 * 1024;
    typedef MemoryCache<CONST_CURL_MEMORY_CACHE_SIZE> CURLMemoryCache;
    // Memory cache, the receiver data will be write to this buff
    CURLMemoryCache m_hMemoryCache;
    // Write the receiver to string object
    std::string m_strReceiveResult;
};

//////////////////////////////////////////////////////////////////////////
// Define data receiver, the data will be write to file
class CCURLFileReceiver : public ICURLDataReceiver
{
public:
    // Constructor
    CCURLFileReceiver()
    {
    }

    // Destructor
    virtual ~CCURLFileReceiver() override final
    {
    }

    // Open this receiver, the parameter is the file name
    virtual bool OpenReceiver(const char* pParam) override final
    {
        // Check file name
        if (nullptr == pParam || '\0' == pParam[0])
        {
            CAquariusLogger::Logger(LL_ERROR_FUN, "Invalid file path");
            return false;
        }
        // Check open file
        m_ofstream.open(pParam, std::fstream::binary | std::fstream::trunc);
        if (!m_ofstream.is_open())
        {
            CAquariusLogger::Logger(LL_ERROR_FUN, "Failed to open file: [%s]", pParam);
            return false;
        }
        return true;
    }

    // Close receiver
    virtual bool CloseReceiver() override final
    {
        if (!m_ofstream.is_open())
        {
            CAquariusLogger::Logger(LL_ERROR_FUN, "The file stream is not opened");
            return false;
        }
        m_ofstream.close();
        return true;
    }

    // Write data to file stream
    virtual bool WriteReceivedData(const char* pszDataBuff, AUINT32 nDataLen) override final
    {
        if (!m_ofstream.is_open())
        {
            CAquariusLogger::Logger(LL_ERROR_FUN, "The file stream is not opened");
            return false;
        }
        m_ofstream.write(pszDataBuff, nDataLen);
        return true;
    }

    // Reset the data receiver
    virtual void Reset() override final
    {
        m_ofstream.close();
    }

private:
    // Target file stream
    std::ofstream m_ofstream;
};

//////////////////////////////////////////////////////////////////////////
// Base class of url helper
class CHTTPHelper
{
public:
    // Constructor
    CHTTPHelper()
        :m_bDebugFlag(false)
    {
    }

    // Destructor
    virtual ~CHTTPHelper()
    {
    }

    // Set debug flag
    inline void SetDebugFlag(bool bFlag)
    {
        m_bDebugFlag = bFlag;
    }

    // Get HTTP response pointer
    inline const char* GetResultPointer() const
    {
        return m_hMemoryReceiver.GetDataPointer();
    }

    // Get HTTP result string
    inline const std::string& GetResultString() const
    {
        return m_hMemoryReceiver.GetResultString();
    }

    // Init curl lib
    static void InitCURLLib();

    // Cleanup curl lib
    static void CleanupCURLLib();

    // Escape URL
    static std::string CURLEscape(const char* pParam);

    /**
     * Description: Debug CURL event
     * Parameter CURL * pCurl           [in]: curl pointer
     * Parameter curl_infotype nType    [in]: curl info type
     * Parameter char * pData           [in]: data pointer
     * Parameter AUINT32 nSize          [in]: size of data
     * Parameter void * pParam          [in]: parameter
     * Returns int: 0 if success
     */
    static int OnDebugCurl(CURL* pCurl, curl_infotype nType, char* pData, AUINT32 nSize, void* pParam);

    // Call back function when write data
    static AUINT32 WriteDataCallBack(void* pContents, AUINT32 nSize, AUINT32 nMemblock, void* pReceiver);

    // Get URL by security
    bool Get(const std::string& strURL, const char* pszCookie = nullptr);

    // Post URL
    bool Post(const std::string& strURL, const char* pszPostParam, const char* pszCookie = nullptr);

protected:
    /**
     * Description: Open data receiver
     * Parameter const char * pszTargetFile         [in]: target file, can be nullptr if you want to write data to memory
     * Parameter ICURLDataReceiver * & pReceiver    [out]: target receiver pointer
     * Returns bool: true if success
     */
    bool OpenDataReceiver(const char* pszTargetFile, ICURLDataReceiver*& pReceiver);

    // Close the data receiver
    bool CloseDataReceiver(ICURLDataReceiver* pReceiver);

    // Define HTTP request
    typedef struct HTTPRequest
    {
        HTTPRequest()
            :strURL(""),
            bIsPost(false),
            strPostParam(""),
            strTargetFile("")
        {
        }
        std::string strURL;
        bool bIsPost;
        std::string strPostParam;
        std::string strTargetFile;
        std::string strCookie;
    } HTTPRequest;

    // Check security url
    bool IsSecurityURL(const std::string& strURL) const;

    // Process HTTP request
    bool ProcessHTTPRequest(const HTTPRequest& stRequest);

protected:
    // Debug flag
    bool m_bDebugFlag;
    // Memory receiver
    CCURLMemoryReceiver m_hMemoryReceiver;
    // File receiver
    CCURLFileReceiver m_hFileReceiver;
};

#endif // _HTTP_HELPER_H_
