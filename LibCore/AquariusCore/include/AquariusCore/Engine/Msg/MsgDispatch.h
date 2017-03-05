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
 * FileName: MsgDispatch.h
 * 
 * Purpose:  Dispatch msg when receive binary data.
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/2/22 16:12
===================================================================+*/
#ifndef _MSG_DISPATCH_H_
#define _MSG_DISPATCH_H_
#include "AquariusCore/Engine/Msg/MsgHandlerTable.h"
#include "AquariusCore/Logger/AquariusLogger.h"
#include "AquariusCore/Util/TypeReDef/TypeReDef.hpp"
#include "AquariusCore/Engine/Session/TCPSession.h"
#include "AquariusCore/Util/TinyUtil/CommonUtil.h"

//////////////////////////////////////////////////////////////////////////
// Define msg dispatch
class CMsgDispatch
{
public:

    /**
     * Description: Handle net msg
     * Parameter const char * pData                 [in]: data block
     * Parameter AUINT32 nDataLen                   [in]: block length
     * Parameter CTCPSession * pSession             [in]: session pointer
     * Parameter unsigned int& nConsumeDataLen      [out]: consume data length of this time
     * Returns bool: true if success
     */
    static bool HandleNetMsg(const char* pData, AUINT32 nDataLen, CTCPSession* pTCPSession, unsigned int& nConsumeDataLen);

    /**
     * Description: Parse msg from binary data block
     * Parameter const char * pData             [in]: data block pointer
     * Parameter AUINT32 nDataLen               [in]: data len
     * Returns const MsgStruct*: msg pointer
     */
    template<typename MsgStruct>
    static inline const MsgStruct* ParseTypeMsg(const char* pData, AUINT32 nDataLen)
    {
        // Check msg len
        if (sizeof(MsgStruct) != nDataLen)
        {
            CAquariusLogger::Logger(LL_ERROR_FUN, "Invalid msg length, msg: [%s], expect len [%u], real len: [%u]", MsgStruct::GetMsgName(), sizeof(MsgStruct), nDataLen);
            return nullptr;
        }
        const MsgStruct* pMsg = (const MsgStruct*)(pData);
        (const_cast<MsgStruct*>(pMsg))->NetToHostStruct();
        return pMsg;
    }

};

//////////////////////////////////////////////////////////////////////////
#define PARSE_TCP_MSG(MsgStruct) \
    if (nullptr == pTCPSession) { CAquariusLogger::Logger(LL_ERROR_FUN, "MsgName: [%s], nullptr == pTCPSession", MsgStruct::GetMsgName()); return false; } \
    const MsgStruct* pMsg = CMsgDispatch::ParseTypeMsg<MsgStruct>(pData, nDataLen); \
    if (nullptr == pMsg) { return false; } \
    if (pMsg->msg_id != MsgStruct::GetMsgId()) { CAquariusLogger::Logger(LL_ERROR_FUN, "MsgName: [%s], Expect Msg Id: 0x%04X, Real Msg Id: 0x%04X", MsgStruct::GetMsgName(), pMsg->msg_id, MsgStruct::GetMsgId()); return false; } \
    if (pMsg->msg_len != sizeof(MsgStruct)) { int nMsgStructLen = (int)sizeof(MsgStruct); CAquariusLogger::Logger(LL_ERROR_FUN, "MsgName: [%s], Expect Msg Len: 0x%04X, Real Msg Len: 0x%04X", MsgStruct::GetMsgName(), pMsg->msg_len, nMsgStructLen); return false; } \
    if (pMsg->GetMonitorFlag()) { CAquariusLogger::Logger(LL_DEBUG_FUN, "SessionId: [%u], MsgName: [%s], Data: [%s]", pTCPSession->GetSessionId(), MsgStruct::GetMsgName(), pMsg->ValueToCStr()); } \
// End of PARSE_TCP_MSG

#define REGISTER_MSG_HANDLE(HandleClass, NetMsg) if(!CMsgHandlerTable::RegisterMsgHandle<NetMsg>(HandleClass::On##NetMsg)) { return false; }
// End of REGISTER_MSG_HANDLE

#endif // _MSG_DISPATCH_H_
