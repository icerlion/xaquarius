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
 * FileName: ReceivedDataHandler.cpp
 * 
 * Purpose:  Handle receive data
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/2/22 16:14
===================================================================+*/
#include "AquariusCore/Engine/Msg/ReceivedDataHandler.h"
#include <string>
#include "AquariusCore/Logger/AquariusLogger.h"
#include "AquariusCore/Engine/Msg/MsgDispatch.h"

CReceivedDataHandler::CReceivedDataHandler() 
    :m_nLeftDataLen(0)
{
    memset(m_chLeftDataBuff, 0, sizeof(m_chLeftDataBuff));
    memset(m_chThisReadBuff, 0, sizeof(m_chThisReadBuff));
}

CReceivedDataHandler::~CReceivedDataHandler()
{
}

void CReceivedDataHandler::Reset()
{
    m_nLeftDataLen = 0;
    memset(m_chLeftDataBuff, 0, sizeof(m_chLeftDataBuff));
    memset(m_chThisReadBuff, 0, sizeof(m_chThisReadBuff));
}

bool CReceivedDataHandler::OnReceiveData( AUINT32 nBytesTransferred, CTCPSession* pTCPSession )
{
    // 1. Process receive data
    bool bParseResult = false;
    // 2. If m_left_data_len is 0, there is no left data in data cache
    if (0 == m_nLeftDataLen)
    {
        bParseResult = ParseNetDataBuff(m_chThisReadBuff, nBytesTransferred, pTCPSession, true);
    }
    else
    {
        // 3. If the total data buff was longer than m_left_data_buff, just break.
        if (m_nLeftDataLen + nBytesTransferred > sizeof(m_chLeftDataBuff))
        {
            CAquariusLogger::Logger(LL_ERROR_FUN, "Impossible, m_left_data_len + nBytesTransferred > sizeof(m_left_data_buff)");
            bParseResult = false;
        }
        else
        {
            // 4. Copy the data buff read this time into the m_left_data_buff, parse it as an entirety
            memcpy(m_chLeftDataBuff + m_nLeftDataLen, m_chThisReadBuff, nBytesTransferred);
            m_nLeftDataLen += nBytesTransferred;
            bParseResult = ParseNetDataBuff(m_chLeftDataBuff, m_nLeftDataLen, pTCPSession, false);
        }
    }
    // 5. If parse failed, just reset the data member
    if (!bParseResult)
    {
        m_nLeftDataLen = 0;
        memset(m_chLeftDataBuff, 0, sizeof(m_chLeftDataBuff));
    }
    return bParseResult;
}

bool CReceivedDataHandler::ParseNetDataBuff( const char* pData, AUINT32 nDataLen, CTCPSession* pTCPSession, bool isRawThisReadBuff )
{
    // 1. Parse the msg data buff
    unsigned int nConsumeDataLen = 0;
    if (!CMsgDispatch::HandleNetMsg(pData, nDataLen, pTCPSession, nConsumeDataLen))
    {
        return false;
    }
    // 2. Check consume data length at this time
    if (nConsumeDataLen == nDataLen)
    {
        if (0 != m_nLeftDataLen)
        {
            m_nLeftDataLen = 0;
            memset(m_chLeftDataBuff, 0, sizeof(m_chLeftDataBuff));
        }
        return true;
    }
    // 3. If consume data len is invalid, just return false.
    if (nConsumeDataLen > nDataLen)
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "Impossible: nConsumeDataLen > nDataLen: [%d]-[%d]", nConsumeDataLen, nDataLen);
        return false;
    }
    // 4. The msg data buff was not consume totally, just cache the left data.
    if (nConsumeDataLen >= nDataLen)
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "Impossible: nConsumeDataLen: [%u] >= nDataLen: [%u]", nConsumeDataLen, nDataLen);
    }
    AUINT32 this_left_data_len = nDataLen - nConsumeDataLen;
    if (this_left_data_len > sizeof(m_chLeftDataBuff))
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "Impossible: this_left_data_len > sizeof(m_left_data_buff)");
        return false;
    }
    // 5. Check safe parameter
    if (isRawThisReadBuff)
    {
        // 5.1 Check left data len
        if (0 != m_nLeftDataLen)
        {
            CAquariusLogger::Logger(LL_ERROR_FUN, "Impossible: 0 != m_left_data_len: [%d]", m_nLeftDataLen);
            return false;
        }
    }
    else
    {
        // 5.2 Check pointer
        if (pData != m_chLeftDataBuff)
        {
            CAquariusLogger::Logger(LL_ERROR_FUN, "Impossible: pData != m_left_data_buff");
            return false;
        }
        // 5.3 Check left data len
        if (0 == m_nLeftDataLen)
        {
            CAquariusLogger::Logger(LL_ERROR_FUN, "Impossible: 0 == m_left_data_len");
            return false;
        }
    }
    // 6. Update left data buff
    memcpy(m_chLeftDataBuff, pData + nConsumeDataLen, this_left_data_len);
    m_nLeftDataLen = this_left_data_len;
    return true;
}
