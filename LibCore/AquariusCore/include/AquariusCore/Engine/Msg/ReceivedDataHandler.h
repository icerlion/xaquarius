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
 * FileName: ReceivedDataHandler.h
 * 
 * Purpose:  Handle received data
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/2/22 16:14
===================================================================+*/
#ifndef _RECEIVE_DATA_HANDLER_H_
#define _RECEIVE_DATA_HANDLER_H_
#include "AquariusCore/Util/TypeReDef/TypeReDef.hpp"

class CTCPSession;
class CReceivedDataHandler
{
public:
    // Constructor
    CReceivedDataHandler();

    // Destructor
    ~CReceivedDataHandler();

    // Reset
    void Reset();

    // Get the data buffer for read new data
    inline char* GetThisTimeReadDataBuffPointer()
    {
        return m_chThisReadBuff;
    }

    // Get buff length
    inline AUINT32 GetThisTimeReadDataBuffLength() const
    {
        return sizeof(m_chThisReadBuff);
    }

    // Get length of left data
    inline AUINT32 GetLeftDataLen() const
    {
        return m_nLeftDataLen;
    }

    /**
     * Description: Process received data
     * Parameter AUINT32 nBytesTransferred          [in]: read data this time
     * Parameter CTCPSession * pTCPSession          [in]: session pointer
     * Returns void:
     */
    bool OnReceiveData(AUINT32 nBytesTransferred, CTCPSession* pTCPSession);

protected:
    /**
     * Description: Parse data buff 
     * Parameter const char * pData         [in]: data pointer
     * Parameter AUINT32 nDataLen           [in]: data length
     * Parameter CTCPSession* pTCPSession   [in]: session pointer
     * Parameter bool isRawThisReadBuff     [in]: is buff read by raw session this time
     * Returns bool: true if success
     */
    bool ParseNetDataBuff(const char* pData, AUINT32 nDataLen, CTCPSession* pTCPSession, bool isRawThisReadBuff);

private:
    // buff of last time left
    char m_chLeftDataBuff[CONST_MAX_MSG_LENGTH * 4];
    // available length of m_left_data_buff
    unsigned int m_nLeftDataLen;
    // read data buff of this time
    char m_chThisReadBuff[CONST_MAX_MSG_LENGTH];
};

#endif // _RECEIVE_DATA_HANDLER_H_
