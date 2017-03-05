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
 * FileName: ServerItemDef.hpp
 * 
 * Purpose:  Define Server Config Item Struct
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/2/22 15:25
===================================================================+*/
#ifndef _SERVERITEMDEF_HPP_
#define _SERVERITEMDEF_HPP_
#include "AquariusCore/Util/TypeReDef/TypeReDef.hpp"
#include "AquariusCore/Logger/AquariusLogger.h"

// Server Name list
const static char CONST_SERVER_NAME_INVALID[] = "Invalid";
const static char CONST_SERVER_NAME_GATESERVER[] = "GateServer";
const static char CONST_SERVER_NAME_FIGHTSERVER[] = "FightServer";
const static char CONST_SERVER_NAME_HUBSERVER[] = "HubServer";

// Define Listener config
typedef struct ListenerConfig 
{
    // Constructor
    ListenerConfig ()
        :strListenerIP(),
        nListenerPort(0)
    {
    }

    // Check available
    inline bool IsAvailable() const
    {
        return (0 != nListenerPort && !strListenerIP.empty());
    }

    /**
     * Description: Parse addr to IP and port
     * Parameter const std::string & strAddr  [in]: addr, such as "192.168.0.1:8000"
     * Returns bool: true if success
     */
    bool ParseFromString(const std::string& strAddr);

    // Listener Name
    std::string strLisetenerName;
    // Listener IP
    std::string strListenerIP;
    // Listen Port
    int nListenerPort;
    // listener string
    std::string strListenerConfigString;
} ListenerConfig ;

// Server Type
enum class EAquariusServerType : int
{
    Invalid = 0,
    GateServer = 1,
    FightServer = 2,
    HubServer = 3,
};

// Server item define
typedef struct ServerItem 
{
    // Constructor
    ServerItem()
        :nId(0),
        nServerType(EAquariusServerType::Invalid),
        strName(CONST_SERVER_NAME_INVALID),
        nShutdownDelayTime(60)
    {
    }
    // server type or group id
    AUINT32 nId;
    // server type
    EAquariusServerType nServerType;
    // Public TCP listener, only for GateServer
    // client can access this port
    ListenerConfig stPublicListener;
    // Private TCP listener, only for connector.
    ListenerConfig stPrivateListener;
    // connector server type list
    std::set<EAquariusServerType> setConnector;
    // server type name
    std::string strName;
    // delay time when shutdown
    unsigned int nShutdownDelayTime;
} ServerItem;

// Define EServerTypeFunction
class ServerConfigHelper
{
public:
    static inline const char* ServerTypeToName(EAquariusServerType nType)
    {
        if (EAquariusServerType::GateServer == nType ) { return CONST_SERVER_NAME_GATESERVER; }
        if (EAquariusServerType::FightServer == nType ) { return CONST_SERVER_NAME_FIGHTSERVER; }
        if (EAquariusServerType::HubServer == nType ) { return CONST_SERVER_NAME_HUBSERVER; }
        CAquariusLogger::Logger(LL_ERROR_FUN, "Invalid server type: [%d]", nType);
        return CONST_SERVER_NAME_INVALID;
    }

    static inline EAquariusServerType ServerNameToType( const char* pszInTypeName )
    {
        if (0 == strcmp(CONST_SERVER_NAME_GATESERVER, pszInTypeName)) { return EAquariusServerType::GateServer; }
        if (0 == strcmp(CONST_SERVER_NAME_FIGHTSERVER, pszInTypeName)) { return EAquariusServerType::FightServer; }
        if (0 == strcmp(CONST_SERVER_NAME_HUBSERVER, pszInTypeName)) { return EAquariusServerType::HubServer; }
        CAquariusLogger::Logger(LL_ERROR_FUN, "Invalid server name: [%s]", pszInTypeName);
        return EAquariusServerType::Invalid;
    }
};

// Key: server id or type
// Value: server item
typedef std::map<AUINT32, ServerItem> MapServerItem;

// DBConfig
typedef struct DBConfig
{
    // Constructor
    DBConfig()
        :strHost(),
        nPort(0),
        strUser(),
        strPassword(),
        strDBName()
    {
    }
    // host name
    std::string strHost;
    // host port
    int nPort;
    // user name 
    std::string strUser;
    // password
    std::string strPassword;
    // db name
    std::string strDBName;
} DBConfig;

#endif // _SERVERITEMDEF_HPP_
