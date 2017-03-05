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
 * FileName: DBTaskDemo.cpp
 * 
 * Purpose:  Purpose:  DBTask demo, show how to use dbtask
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/2/22 15:27
===================================================================+*/
#include "AquariusCore/Logger/AquariusLogger.h"
#include "FightServer/DBTask/DBTaskDemo.h"
#include "AquariusCore/Util/TinyUtil/TimeUtil.h"
#include "AquariusCore/Util/TinyUtil/StringUtil.h"

CDBTaskDemo::CDBTaskDemo()
{
}


CDBTaskDemo::~CDBTaskDemo(void)
{
}

void CDBTaskDemo::ProcDBTaskRequest(CMySQLConnection& hMySQLConnection)
{
    // This function will be called by dbtask thread, you can access mysql, but don't send msg
    std::string strSQL = "INSERT INTO `chat`(`client_session_id`, `chat_content`, `time_point`) VALUES(" + CStringUtil::Int32ToDecString(m_stChatMsg.speaker_id) + ", '" + CStringUtil::BuildSafeString(m_stChatMsg.content) + "', '" + CTimeUtil::UTCTimeToReadableString(m_stChatMsg.time) + "'); ";
    hMySQLConnection.RunSQL(strSQL);
}

void CDBTaskDemo::ProcDBTaskResponse()
{
    // This function will be called by main thread, you can send msg here.
}

void CDBTaskDemo::ReleaseSelf()
{
    CDBTaskDemoPool::RecycleObject(this);
}

void CDBTaskDemo::InitDBTask(const ChatMsg& stChatMsg)
{
    m_stChatMsg = stChatMsg;
}

void CDBTaskDemo::ResetPoolObject()
{
    CCommonUtil::ZeroStruct(m_stChatMsg);
}
