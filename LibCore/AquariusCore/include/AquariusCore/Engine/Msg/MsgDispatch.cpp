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
 * FileName: MsgDispatch.cpp
 * 
 * Purpose:  Parse msg from receiver buff and dispatch it
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/2/22 16:13
===================================================================+*/
#include "AquariusCore/Engine/Msg/MsgDispatch.h"


bool CMsgDispatch::HandleNetMsg( const char* pData, AUINT32 nDataLen, CTCPSession* pTCPSession, unsigned int& nConsumeDataLen )
{
    // 1. Check session
    nConsumeDataLen = 0;
    if (nullptr == pTCPSession)
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "nullptr == pTCPSession");
        return false;
    }
    // 2. Check data
    if (nullptr == pData)
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "nullptr == pData! sender: [%s], SessionId: [%u]", pTCPSession->GetRemoteEndPointCStr(), pTCPSession->GetSessionId());
        return false;
    }
    // 3. Copy the original request data, loop to parse every msg
    AUINT32 nLeftDataLen = nDataLen;
    const char* pMsgDataOffset = pData;
    bool bResult = true;
    do 
    {
        // 3.1 If the data buff only has msg id and msg len, or less, it can not be parsed to a msg, just return and cache it
        if (sizeof(unsigned short) + sizeof(unsigned short) >= nLeftDataLen)
        {
            CAquariusLogger::Logger(LL_DEBUG_FUN, "Invalid data length! sender: [%s], SessionId: [%u]", pTCPSession->GetRemoteEndPointCStr(), pTCPSession->GetSessionId());
            break;
        }
        // 3.2 Parse the current msg len
        unsigned short nMsgLen = 0;
        memcpy(&nMsgLen, pMsgDataOffset, sizeof(nMsgLen));
        nMsgLen = CCommonUtil::NetToHost16(nMsgLen);
        // 3.3 Parse the current msg id
        unsigned short nMsgId = 0;
        memcpy(&nMsgId, pMsgDataOffset + sizeof(unsigned short), sizeof(nMsgId));
        nMsgId = CCommonUtil::NetToHost16(nMsgId);
        // 3.4 Find msg handler info
        const MsgHandlerData& stMsgHandlerData = CMsgHandlerTable::GetMsgHandlerData(nMsgId);
        if (nullptr == stMsgHandlerData.pfnMsgHandler)
        {
            CAquariusLogger::Logger(LL_ERROR_FUN, "Failed to find msg handler! msg id: [0x%04X]-[%hu], sender: [%s], SessionId: [%u]", nMsgId, nMsgId, pTCPSession->GetRemoteEndPointCStr(), pTCPSession->GetSessionId());
            bResult = false;
            break;
        }
        // 3.5 Check msg len
        if (nMsgLen != stMsgHandlerData.nMsgLen)
        {
            CAquariusLogger::Logger(LL_ERROR_FUN, "The msg was distorted! msg id: [0x%04X]-[%hu], sender: [%s], SessionId: [%u], read len: [%hu], expected len: [%hu]", nMsgId, nMsgId, pTCPSession->GetRemoteEndPointCStr(), pTCPSession->GetSessionId(), nMsgLen, stMsgHandlerData.nMsgLen);
            bResult = false;
            break;
        }
        // 3.6 Check msg data length
        if (nLeftDataLen < stMsgHandlerData.nMsgLen)
        {
            // The complete data has not been received, cache the current data
            CAquariusLogger::Logger(LL_DEBUG_FUN, "The current msg length is not length enough for a msg! msg id: [0x%04X]-[%hu], sender: [%s], SessionId: [%u]", nMsgId, nMsgId, pTCPSession->GetRemoteEndPointCStr(), pTCPSession->GetSessionId());
            break;
        }
        // 3.7 Update consume data len
        nConsumeDataLen += stMsgHandlerData.nMsgLen;
        // 3.8 Call msg handler function
        stMsgHandlerData.pfnMsgHandler(pMsgDataOffset, stMsgHandlerData.nMsgLen, pTCPSession);
        pTCPSession->StatRecvMsg(nMsgId);
        // 3.9 Check and update the left data length
        if (nLeftDataLen == stMsgHandlerData.nMsgLen)
        {
            break;
        }
        else
        {
            // The nLeftDataLen must be greater than refMsgHandlerData.nMsgLen as we had check it at 3.6
            nLeftDataLen -= stMsgHandlerData.nMsgLen;
            pMsgDataOffset += stMsgHandlerData.nMsgLen;
        }
    } while (nLeftDataLen > 0);
    // 4. Return result
    return bResult;
}
