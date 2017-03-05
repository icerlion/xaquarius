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
 * FileName: ScreenLogVisitor.cpp
 * 
 * Purpose:  Implement Screen log visitor
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/2/22 16:07
===================================================================+*/
#include "AquariusCore/Logger/LogVisitor/ScreenLogVisitor.h"
#include "AquariusCore/Util/TypeReDef/TypeReDef.hpp"

#ifdef WIN32
// The following code only run under win32
#include <Windows.h>

// Set text color in console
int SetConsoleTextColor(unsigned char nColor)
{
    HANDLE hStdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO stSCBI;
    GetConsoleScreenBufferInfo(hStdHandle, &stSCBI);
    return SetConsoleTextAttribute(hStdHandle, nColor | (stSCBI.wAttributes & (~0x0F)));
}

// Update console text color by log level
static void UpdateConsoleTextColor(ELogLevel cur_log_level)
{
    // Define text color type
    enum class EConsoleTextColorType : int
    {
        Invalid = 0,       // First value, invalid
        Green = 10,        // Normal, green
        Blue = 9,          // System, blue
        Yellow = 14,       // Warning, yellow
        Red = 12,          // Error, red
    };
    // Static color type, store last color type
    static EConsoleTextColorType pre_color_type = EConsoleTextColorType::Invalid;
    EConsoleTextColorType cur_color_type = EConsoleTextColorType::Green;
    switch (cur_log_level)
    {
    case ELogLevel::Log_Level_Debug:
    case ELogLevel::Log_Level_Notice:
        cur_color_type = EConsoleTextColorType::Green;
        break;
    case ELogLevel::Log_Level_Warning:
        cur_color_type = EConsoleTextColorType::Yellow;
        break;
    default:
        cur_color_type = EConsoleTextColorType::Red;
        break;
    }
    if (pre_color_type == cur_color_type)
    {
        return;
    }
    SetConsoleTextColor(static_cast<unsigned char>(cur_color_type));
    pre_color_type = cur_color_type;
}
#endif // End of WIN32

bool CScreenLogVisitor::InitializeLogVisitor(const LogVisitorConfigBase* pLogVisitorConfig)
{
    const ScreenLogConfig* pScreenLogConfig = dynamic_cast<const ScreenLogConfig*>(pLogVisitorConfig);
    if (nullptr == pScreenLogConfig)
    {
        JUNIOR_LOG("nullptr == pScreenLogConfig");
        return false;
    }
    m_nLogLevelMask = pScreenLogConfig->nLogLevelMask;
    return true;
}

void CScreenLogVisitor::VisitLogMsg( const LogMsg& stLogMsg )
{
#ifdef WIN32
    UpdateConsoleTextColor(stLogMsg.nLogLevel);
#endif // WIN32
    printf("%s%s\n", stLogMsg.buffLogHeader, stLogMsg.buffLogContent);
    return;
}
