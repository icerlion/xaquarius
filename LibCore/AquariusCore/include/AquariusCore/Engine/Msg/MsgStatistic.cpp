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
 * FileName: MsgStatistic.cpp
 * 
 * Purpose:  Implement statistic msg
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/2/22 16:14
===================================================================+*/
#include "AquariusCore/Engine/Msg/MsgStatistic.h"
#include "AquariusCore/Logger/AquariusLogger.h"
#include "AquariusCore/Util/TinyUtil/StringUtil.h"

FunGetMsgInfo CMsgStatistic::ms_pFunGetMsgInfo = nullptr;

CMsgStatistic::CMsgStatistic(void)
    :m_bEnableFlag(false)
{
}


CMsgStatistic::~CMsgStatistic(void)
{
}

void CMsgStatistic::OnSendMsg( AUINT16 nMsgId )
{
    if (m_bEnableFlag)
    {
        auto itFind = m_mapSendMsg.find(nMsgId);
        if (itFind == m_mapSendMsg.end())
        {
            m_mapSendMsg.insert(std::make_pair(nMsgId, 1));
        }
        else
        {
            ++(itFind->second);
        }
    }
}

void CMsgStatistic::OnRecvMsg( AUINT16 nMsgId )
{
    if (m_bEnableFlag)
    {
        auto itFind = m_mapRecvMsg.find(nMsgId);
        if (itFind == m_mapRecvMsg.end())
        {
            m_mapRecvMsg.insert(std::make_pair(nMsgId, 1));
        }
        else
        {
            ++(itFind->second);
        }
    }
}

void CMsgStatistic::Reset()
{
    m_mapSendMsg.clear();
    m_mapRecvMsg.clear();
}

void CMsgStatistic::ShowStatisticResult(AUINT32 nId)
{
    if (m_bEnableFlag)
    {
        ShowSendStatistic(nId);
        ShowRecvStatistic(nId);
    }
}

void CMsgStatistic::ShowSendStatistic(AUINT32 nId)
{
    AUINT32 nTotalSendBytes = 0;
    for each (auto& kvp in m_mapSendMsg)
    {
        AUINT16 nMsgId = kvp.first;
        AUINT32 nMsgCount = kvp.second;
        std::string strMsgName;
        AUINT16 nMsgLen = 0;
        GetMsgInfo(nMsgId, strMsgName, nMsgLen);
        AUINT32 nTotalMsgSize = nMsgCount * nMsgLen;
        CAquariusLogger::Logger(LL_DEBUG_FUN, "SendStat,Id,%u,MsgName,%s,MsgCount,%u,TotalSize,%u", nId, strMsgName.c_str(), nMsgCount, nTotalMsgSize);
        nTotalSendBytes += nTotalMsgSize;
    }
    CAquariusLogger::Logger(LL_DEBUG_FUN, "SendStat,Id,%u,TotalSendSize,%u", nId, nTotalSendBytes);
}

void CMsgStatistic::ShowRecvStatistic(AUINT32 nId)
{
    AUINT32 nTotalRecvBytes = 0;
    for each (auto& kvp in m_mapRecvMsg)
    {
        AUINT16 nMsgId = kvp.first;
        AUINT32 nMsgCount = kvp.second;
        std::string strMsgName;
        AUINT16 nMsgLen = 0;
        GetMsgInfo(nMsgId, strMsgName, nMsgLen);
        AUINT32 nTotalMsgSize = nMsgCount * nMsgLen;
        CAquariusLogger::Logger(LL_DEBUG_FUN, "RecvStat,Id,%u,MsgName,%s,MsgCount,%u,TotalSize,%u", nId, strMsgName.c_str(), nMsgCount, nTotalMsgSize);
        nTotalRecvBytes += nTotalMsgSize;
    }
    CAquariusLogger::Logger(LL_DEBUG_FUN, "RecvStat,Id,%u,TotalRecvSize,%u", nId, nTotalRecvBytes);
}

void CMsgStatistic::InitMsgStatistic( FunGetMsgInfo pfnGetMsgInfo )
{
    ms_pFunGetMsgInfo = pfnGetMsgInfo;
}

void CMsgStatistic::GetMsgInfo( AUINT16 nMsgId, std::string& strMsgName, AUINT16& nMsgLen )
{
    if (nullptr != ms_pFunGetMsgInfo)
    {
        ms_pFunGetMsgInfo(nMsgId, strMsgName, nMsgLen);
    }
    else
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "nullptr == ms_pFunGetMsgInfo");
        char buff[32] = { 0 };
        a_sprintf(buff, "%04X", nMsgId);
        strMsgName = buff;
        nMsgLen = 0;
    }
}
