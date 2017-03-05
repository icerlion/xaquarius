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
 * FileName: MsgHandlerTable.h
 * 
 * Purpose:  Hold all msg handler data
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/2/22 16:13
===================================================================+*/
#ifndef _MSG_HANDLER_TABLE_H_
#define _MSG_HANDLER_TABLE_H_
#include <string>
#include "AquariusCore/Engine/Session/TCPSession.h"
#include "AquariusCore/Util/ObjPool/ObjectPool.hpp"
#include "AquariusCore/Util/Singleton/ISingleton.hpp"

// Define msg handler
typedef bool (*MsgHandlerFun)(const char* pData, AUINT32 nDataLen, CTCPSession* pTCPSession);

//////////////////////////////////////////////////////////////////////////
// Define MsgHandlerData
typedef struct MsgHandlerData
{
public:
    // Constructor
    MsgHandlerData()
        :nMsgId(0),
        nMsgLen(0),
        pfnMsgHandler(nullptr),
        strMsgName()
    {
    }

    // Destructor
    ~MsgHandlerData()
    {
    }

    // Reset the data
    inline void Reset()
    {
        nMsgId = 0;
        nMsgLen = 0;
        pfnMsgHandler = nullptr;
        strMsgName.clear();
    }
    // Msg id
    AUINT16 nMsgId;
    // Msg len
    AUINT16 nMsgLen;
    // Msg handler function
    MsgHandlerFun pfnMsgHandler;
    // Msg Name
    std::string strMsgName;
} MsgHandlerData;

//////////////////////////////////////////////////////////////////////////
class CMsgHandlerWrapper : public IObjInPool
{
public:
    // Constructor
    CMsgHandlerWrapper()
    {
    }

    // Destructor
    virtual ~CMsgHandlerWrapper() override final
    {
    }

    // Implement in IObjInPool::ResetPoolObject
    virtual void ResetPoolObject() override final
    {
        m_stMsgHandlerData.Reset();
    }

    // Get the MsgHandlerData
    inline const MsgHandlerData& GetMsgHandlerData() const
    {
        return m_stMsgHandlerData;
    }

    // Set MsgHandlerData
    inline void SetMsgHandlerData(const MsgHandlerData& stMsgHandlerData)
    {
        m_stMsgHandlerData = stMsgHandlerData;
    }

    // Get msg id
    inline AUINT32 GetId() const
    {
        return m_stMsgHandlerData.nMsgId;
    }

protected:
    // Hold msg handler data
    MsgHandlerData m_stMsgHandlerData;
};

const static int MAX_MSG_COUNT = 8196;
typedef CObjectPool<CMsgHandlerWrapper, MAX_MSG_COUNT> CMsgHandlerWrapperPool;

//////////////////////////////////////////////////////////////////////////

class CMsgHandlerTable
{
public:
    // Register msg handle
    template<typename MsgStruct>
    static bool RegisterMsgHandle(MsgHandlerFun pfnHandler)
    {
        // 1. Build MsgHandlerData
        MsgHandlerData stMsgHandlerData;
        stMsgHandlerData.nMsgId = MsgStruct::GetMsgId();
        stMsgHandlerData.nMsgLen = sizeof(MsgStruct);
        stMsgHandlerData.pfnMsgHandler = pfnHandler;
        stMsgHandlerData.strMsgName = MsgStruct::GetMsgName();
        // 2. Check msg id
        if (0 == stMsgHandlerData.nMsgId)
        {
            CAquariusLogger::Logger(LL_ERROR_FUN, "Invalid msg id = 0, name: [%s]", stMsgHandlerData.strMsgName.c_str());
            return false;
        }
        // 3. Check msg size
        if (stMsgHandlerData.nMsgLen > CONST_MAX_MSG_LENGTH)
        {
            CAquariusLogger::Logger(LL_ERROR_FUN, "Msg size is too big, name: [%s]", stMsgHandlerData.strMsgName.c_str());
            return false;
        }
        // 4. Check msg handler
        if (nullptr == stMsgHandlerData.pfnMsgHandler)
        {
            CAquariusLogger::Logger(LL_ERROR_FUN, "Invalid handler function, name: [%s]", stMsgHandlerData.strMsgName.c_str());
            return false;
        }
        // 5. Check existed msg id
        if (ms_mapMsgHandler.end() != ms_mapMsgHandler.find(stMsgHandlerData.nMsgId))
        {
            CAquariusLogger::Logger(LL_ERROR_FUN, "MsgId existed:[0x%04X], name: [%s]", stMsgHandlerData.nMsgId, stMsgHandlerData.strMsgName.c_str());
            return false;
        }
        // 6. Get a handler wrapper pointer
        CMsgHandlerWrapper* pMsgHandlerWrapper = CMsgHandlerWrapperPool::AcquireObject();
        if (nullptr == pMsgHandlerWrapper)
        {
            CAquariusLogger::Logger(LL_ERROR_FUN, "nullptr == pMsgHandlerWrapper");
            return false;
        }
        pMsgHandlerWrapper->SetMsgHandlerData(stMsgHandlerData);
        // 7. Insert msg handler table
        if (!ms_mapMsgHandler.insert(std::make_pair(pMsgHandlerWrapper->GetId(), pMsgHandlerWrapper)).second)
        {
            CAquariusLogger::Logger(LL_ERROR_FUN, "The msg id [0x%04X] was reduplicate, type name: [%s]", stMsgHandlerData.nMsgId, stMsgHandlerData.strMsgName.c_str());
            CMsgHandlerWrapperPool::RecycleObject(pMsgHandlerWrapper);
            return false;
        }
        return true;
    }

    // Find msg handler data by msg id
    static const MsgHandlerData& GetMsgHandlerData(AUINT16 nMsgId);

    // Destroy msg handler
    static void Destroy();

private:
    // Key: msg id
    // Value: msg handler wrapper
    static std::map<AUINT16, const CMsgHandlerWrapper*> ms_mapMsgHandler;
};

#endif // _MSG_HANDLER_TABLE_H_
