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
 * FileName: MsgStatistic.h
 * 
 * Purpose:  Statistic msg count, both receive and send.
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/2/22 16:13
===================================================================+*/
#ifndef _MSG_STATISTIC_H_
#define _MSG_STATISTIC_H_
#include "AquariusCore/Util/TypeReDef/TypeReDef.hpp"

// Assistant function, get msg info
typedef void (*FunGetMsgInfo)(AUINT16 nMsgId, std::string& strMsgName, AUINT16& nMsgLen );

class CMsgStatistic
{
public:
    // Constructor
    CMsgStatistic(void);

    // Destructor
    ~CMsgStatistic(void);

    // Init msg statistic assistant function
    static void InitMsgStatistic(FunGetMsgInfo pfnGetMsgInfo);

    // Reset msg stat data
    void Reset();

    // Called after send msg
    void OnSendMsg(AUINT16 nMsgId);

    // Called after recv data
    void OnRecvMsg(AUINT16 nMsgId);

    // Show statistic result
    void ShowStatisticResult(AUINT32 nSessionId);

    // Get msg info, call FunGetMsgInfo
    static void GetMsgInfo(AUINT16 nMsgId, std::string& strMsgName, AUINT16& nMsgLen);

    // Set enable flag
    inline void SetEnableFlag(bool bFlag)
    {
        m_bEnableFlag = bFlag;
    }

private:
    // Show send statistic data
    void ShowSendStatistic(AUINT32 nSessionId);

    // Show recv statistic data
    void ShowRecvStatistic(AUINT32 nSessionId);

private:
    // Flag of enable stat msg
    bool m_bEnableFlag;
    // Key: Msg id, 
    // Value: Msg count
    typedef std::map<AUINT16, AUINT32> MapMsgStat;
    // Statistic of send msg
    MapMsgStat m_mapSendMsg;
    // Statistic of send msg
    MapMsgStat m_mapRecvMsg;
    // Assistant function of get msg info
    static FunGetMsgInfo ms_pFunGetMsgInfo;
};

#endif // _MSG_STATISTIC_H_
