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
 * FileName: MsgUtilWriter.h
 * 
 * Purpose:  Write MsgUtil function, support msg id, msg len, msg name
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/3/5 14:21
===================================================================+*/
#ifndef _MSGUTILWRITER_H_
#define _MSGUTILWRITER_H_

#include <vector>
#include <string>
#include <fstream>
#include "ProtocolHelper/ProtocolParse/MsgTypeDef.h"

class CMsgUtilWriter
{
public:
    // Begin to write MsgUtil.hpp
    static void BeginMsgUtilHpp(const std::string& strFilePath);

    // Update MsgUtil.hpp by net struct
    static void UpdateMsgUtilHpp(const CMsgTypeStruct* pStruct);

    // End of write MsgUtil.Hpp
    static void EndMsgUtilHpp();

private:
    // MsgUtil ofstream
    static std::ofstream ms_ofsMsgUtilHpp;
};

#endif // _MSGUTILWRITER_H_
