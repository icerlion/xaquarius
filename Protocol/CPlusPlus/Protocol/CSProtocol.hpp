// Warning: This file is auto-generated by ParseProtocol.exe, you should not change it!
// Time: 2017-03-05 14:31:14
#ifndef _CSPROTOCOL_HPP_
#define _CSPROTOCOL_HPP_

#include "AquariusCore/Util/TypeReDef/TypeReDef.hpp"
#include "AquariusCore/Util/TinyUtil/CommonUtil.h"
#include "AquariusCore/Logger/AquariusLogger.h"
#include "Protocol/ProtocolType.hpp"

#pragma pack(push)
#pragma pack(1)
// heart beat package. [size: 6 ]
static const unsigned short CG_HeartBeat_Msg_Id = 0x3fd4;
typedef struct CG_HeartBeat_Msg
{
    // Get id of this msg
    static inline const unsigned short GetMsgId()
    {
        return CG_HeartBeat_Msg_Id; 
    }
    // Get inline monitor flag this msg
    static inline bool GetMonitorFlag()
    {
        return false; 
    }
    // Get msg name
    static inline const char* GetMsgName()
    {
        return "CG_HeartBeat_Msg"; 
    }
    // Constructor of CG_HeartBeat_Msg
    CG_HeartBeat_Msg()
        :msg_len(sizeof(CG_HeartBeat_Msg)),
        msg_id(CG_HeartBeat_Msg_Id),
        heartbeat_index(0)
    {
    }
    // msg len, you should not change the value
    AUINT16 msg_len;
    // msg id, you should not change the value
    AUINT16 msg_id;
    // index of heart beat
    AUINT16 heartbeat_index;
    // To net byte order for CG_HeartBeat_Msg
    inline void HostToNetStruct()
    {
        msg_len = CCommonUtil::HostToNet16(msg_len);
        msg_id = CCommonUtil::HostToNet16(msg_id);
        heartbeat_index = CCommonUtil::HostToNet16(heartbeat_index);
    }
    // To host byte order for CG_HeartBeat_Msg
    inline void NetToHostStruct()
    {
        msg_len = CCommonUtil::NetToHost16(msg_len);
        msg_id = CCommonUtil::NetToHost16(msg_id);
        heartbeat_index = CCommonUtil::NetToHost16(heartbeat_index);
    }
    // Write function of ValueToCStr for CG_HeartBeat_Msg
    inline const char* ValueToCStr() const
    {
        static std::string s_strMsgValue;
        s_strMsgValue.clear();
        s_strMsgValue.reserve(2048);
        // Append Field msg_len
        s_strMsgValue.append("{ml:");
        s_strMsgValue.append(CStringUtil::UInt32ToDecString(msg_len));
        s_strMsgValue.append("}");
        // Append Field msg_id
        s_strMsgValue.append("{mi:");
        s_strMsgValue.append(CStringUtil::UInt32ToDecString(msg_id));
        s_strMsgValue.append("}");
        // Append Field heartbeat_index
        s_strMsgValue.append("{hi:");
        s_strMsgValue.append(CStringUtil::UInt32ToDecString(heartbeat_index));
        s_strMsgValue.append("}");
        return s_strMsgValue.c_str();
    }
} CG_HeartBeat_Msg;
static_assert( sizeof( CG_HeartBeat_Msg ) == 6, "Check struct size failed for [CG_HeartBeat_Msg]");

// heart beat package. [size: 10 ]
static const unsigned short GC_HeartBeat_Msg_Id = 0x254a;
typedef struct GC_HeartBeat_Msg
{
    // Get id of this msg
    static inline const unsigned short GetMsgId()
    {
        return GC_HeartBeat_Msg_Id; 
    }
    // Get inline monitor flag this msg
    static inline bool GetMonitorFlag()
    {
        return false; 
    }
    // Get msg name
    static inline const char* GetMsgName()
    {
        return "GC_HeartBeat_Msg"; 
    }
    // Constructor of GC_HeartBeat_Msg
    GC_HeartBeat_Msg()
        :msg_len(sizeof(GC_HeartBeat_Msg)),
        msg_id(GC_HeartBeat_Msg_Id),
        heartbeat_index(0),
        server_utc_time(0)
    {
    }
    // msg len, you should not change the value
    AUINT16 msg_len;
    // msg id, you should not change the value
    AUINT16 msg_id;
    // index of heart beat
    AUINT16 heartbeat_index;
    // server utc time
    AUINT32 server_utc_time;
    // To net byte order for GC_HeartBeat_Msg
    inline void HostToNetStruct()
    {
        msg_len = CCommonUtil::HostToNet16(msg_len);
        msg_id = CCommonUtil::HostToNet16(msg_id);
        heartbeat_index = CCommonUtil::HostToNet16(heartbeat_index);
        server_utc_time = CCommonUtil::HostToNet32(server_utc_time);
    }
    // To host byte order for GC_HeartBeat_Msg
    inline void NetToHostStruct()
    {
        msg_len = CCommonUtil::NetToHost16(msg_len);
        msg_id = CCommonUtil::NetToHost16(msg_id);
        heartbeat_index = CCommonUtil::NetToHost16(heartbeat_index);
        server_utc_time = CCommonUtil::NetToHost32(server_utc_time);
    }
    // Write function of ValueToCStr for GC_HeartBeat_Msg
    inline const char* ValueToCStr() const
    {
        static std::string s_strMsgValue;
        s_strMsgValue.clear();
        s_strMsgValue.reserve(2048);
        // Append Field msg_len
        s_strMsgValue.append("{ml:");
        s_strMsgValue.append(CStringUtil::UInt32ToDecString(msg_len));
        s_strMsgValue.append("}");
        // Append Field msg_id
        s_strMsgValue.append("{mi:");
        s_strMsgValue.append(CStringUtil::UInt32ToDecString(msg_id));
        s_strMsgValue.append("}");
        // Append Field heartbeat_index
        s_strMsgValue.append("{hi:");
        s_strMsgValue.append(CStringUtil::UInt32ToDecString(heartbeat_index));
        s_strMsgValue.append("}");
        // Append Field server_utc_time
        s_strMsgValue.append("{sut:");
        s_strMsgValue.append(CStringUtil::UInt32ToDecString(server_utc_time));
        s_strMsgValue.append("}");
        return s_strMsgValue.c_str();
    }
} GC_HeartBeat_Msg;
static_assert( sizeof( GC_HeartBeat_Msg ) == 10, "Check struct size failed for [GC_HeartBeat_Msg]");

// common struct request. [size: 274 ]
static const unsigned short CG_Echo_Request_Id = 0x4362;
typedef struct CG_Echo_Request
{
    // Get id of this msg
    static inline const unsigned short GetMsgId()
    {
        return CG_Echo_Request_Id; 
    }
    // Get inline monitor flag this msg
    static inline bool GetMonitorFlag()
    {
        return false; 
    }
    // Get msg name
    static inline const char* GetMsgName()
    {
        return "CG_Echo_Request"; 
    }
    // Constructor of CG_Echo_Request
    CG_Echo_Request()
        :msg_len(sizeof(CG_Echo_Request)),
        msg_id(CG_Echo_Request_Id)
    {
    }
    // msg len, you should not change the value
    AUINT16 msg_len;
    // msg id, you should not change the value
    AUINT16 msg_id;
    // msg field
    EchoMsg data;
    // To net byte order for CG_Echo_Request
    inline void HostToNetStruct()
    {
        msg_len = CCommonUtil::HostToNet16(msg_len);
        msg_id = CCommonUtil::HostToNet16(msg_id);
        data.HostToNetStruct();
    }
    // To host byte order for CG_Echo_Request
    inline void NetToHostStruct()
    {
        msg_len = CCommonUtil::NetToHost16(msg_len);
        msg_id = CCommonUtil::NetToHost16(msg_id);
        data.NetToHostStruct();
    }
    // Write function of ValueToCStr for CG_Echo_Request
    inline const char* ValueToCStr() const
    {
        static std::string s_strMsgValue;
        s_strMsgValue.clear();
        s_strMsgValue.reserve(2048);
        // Append Field msg_len
        s_strMsgValue.append("{ml:");
        s_strMsgValue.append(CStringUtil::UInt32ToDecString(msg_len));
        s_strMsgValue.append("}");
        // Append Field msg_id
        s_strMsgValue.append("{mi:");
        s_strMsgValue.append(CStringUtil::UInt32ToDecString(msg_id));
        s_strMsgValue.append("}");
        // Append Field data
        s_strMsgValue.append("{d:");
        s_strMsgValue.append(data.ValueToCStr());
        s_strMsgValue.append("}");
        return s_strMsgValue.c_str();
    }
} CG_Echo_Request;
static_assert( sizeof( CG_Echo_Request ) == 274, "Check struct size failed for [CG_Echo_Request]");

// common struct response. [size: 276 ]
static const unsigned short GC_Echo_Response_Id = 0x409e;
typedef struct GC_Echo_Response
{
    // Get id of this msg
    static inline const unsigned short GetMsgId()
    {
        return GC_Echo_Response_Id; 
    }
    // Get inline monitor flag this msg
    static inline bool GetMonitorFlag()
    {
        return false; 
    }
    // Get msg name
    static inline const char* GetMsgName()
    {
        return "GC_Echo_Response"; 
    }
    // Constructor of GC_Echo_Response
    GC_Echo_Response()
        :msg_len(sizeof(GC_Echo_Response)),
        msg_id(GC_Echo_Response_Id),
        response_code(EResponseCode::Invalid)
    {
    }
    // msg len, you should not change the value
    AUINT16 msg_len;
    // msg id, you should not change the value
    AUINT16 msg_id;
    // msg field
    EchoMsg data;
    // ResponseCode
    EResponseCode response_code;
    // To net byte order for GC_Echo_Response
    inline void HostToNetStruct()
    {
        msg_len = CCommonUtil::HostToNet16(msg_len);
        msg_id = CCommonUtil::HostToNet16(msg_id);
        data.HostToNetStruct();
        response_code = (EResponseCode)CCommonUtil::HostToNet16((short)response_code);
    }
    // To host byte order for GC_Echo_Response
    inline void NetToHostStruct()
    {
        msg_len = CCommonUtil::NetToHost16(msg_len);
        msg_id = CCommonUtil::NetToHost16(msg_id);
        data.NetToHostStruct();
        response_code = (EResponseCode)CCommonUtil::NetToHost16((short)response_code);
    }
    // Write function of ValueToCStr for GC_Echo_Response
    inline const char* ValueToCStr() const
    {
        static std::string s_strMsgValue;
        s_strMsgValue.clear();
        s_strMsgValue.reserve(2048);
        // Append Field msg_len
        s_strMsgValue.append("{ml:");
        s_strMsgValue.append(CStringUtil::UInt32ToDecString(msg_len));
        s_strMsgValue.append("}");
        // Append Field msg_id
        s_strMsgValue.append("{mi:");
        s_strMsgValue.append(CStringUtil::UInt32ToDecString(msg_id));
        s_strMsgValue.append("}");
        // Append Field data
        s_strMsgValue.append("{d:");
        s_strMsgValue.append(data.ValueToCStr());
        s_strMsgValue.append("}");
        // Append Field response_code
        s_strMsgValue.append("{rc:");
        s_strMsgValue.append(EResponseCodeToCStr(response_code));
        s_strMsgValue.append("}");
        return s_strMsgValue.c_str();
    }
} GC_Echo_Response;
static_assert( sizeof( GC_Echo_Response ) == 276, "Check struct size failed for [GC_Echo_Response]");

// send chat cmd to server. [size: 268 ]
static const unsigned short CG_Chat_Cmd_Id = 0x24f5;
typedef struct CG_Chat_Cmd
{
    // Get id of this msg
    static inline const unsigned short GetMsgId()
    {
        return CG_Chat_Cmd_Id; 
    }
    // Get inline monitor flag this msg
    static inline bool GetMonitorFlag()
    {
        return true; 
    }
    // Get msg name
    static inline const char* GetMsgName()
    {
        return "CG_Chat_Cmd"; 
    }
    // Constructor of CG_Chat_Cmd
    CG_Chat_Cmd()
        :msg_len(sizeof(CG_Chat_Cmd)),
        msg_id(CG_Chat_Cmd_Id)
    {
    }
    // msg len, you should not change the value
    AUINT16 msg_len;
    // msg id, you should not change the value
    AUINT16 msg_id;
    // msg field
    ChatMsg data;
    // To net byte order for CG_Chat_Cmd
    inline void HostToNetStruct()
    {
        msg_len = CCommonUtil::HostToNet16(msg_len);
        msg_id = CCommonUtil::HostToNet16(msg_id);
        data.HostToNetStruct();
    }
    // To host byte order for CG_Chat_Cmd
    inline void NetToHostStruct()
    {
        msg_len = CCommonUtil::NetToHost16(msg_len);
        msg_id = CCommonUtil::NetToHost16(msg_id);
        data.NetToHostStruct();
    }
    // Write function of ValueToCStr for CG_Chat_Cmd
    inline const char* ValueToCStr() const
    {
        static std::string s_strMsgValue;
        s_strMsgValue.clear();
        s_strMsgValue.reserve(2048);
        // Append Field msg_len
        s_strMsgValue.append("{ml:");
        s_strMsgValue.append(CStringUtil::UInt32ToDecString(msg_len));
        s_strMsgValue.append("}");
        // Append Field msg_id
        s_strMsgValue.append("{mi:");
        s_strMsgValue.append(CStringUtil::UInt32ToDecString(msg_id));
        s_strMsgValue.append("}");
        // Append Field data
        s_strMsgValue.append("{d:");
        s_strMsgValue.append(data.ValueToCStr());
        s_strMsgValue.append("}");
        return s_strMsgValue.c_str();
    }
} CG_Chat_Cmd;
static_assert( sizeof( CG_Chat_Cmd ) == 268, "Check struct size failed for [CG_Chat_Cmd]");

// send chat msg to client. [size: 268 ]
static const unsigned short GC_Chat_Msg_Id = 0x45d9;
typedef struct GC_Chat_Msg
{
    // Get id of this msg
    static inline const unsigned short GetMsgId()
    {
        return GC_Chat_Msg_Id; 
    }
    // Get inline monitor flag this msg
    static inline bool GetMonitorFlag()
    {
        return false; 
    }
    // Get msg name
    static inline const char* GetMsgName()
    {
        return "GC_Chat_Msg"; 
    }
    // Constructor of GC_Chat_Msg
    GC_Chat_Msg()
        :msg_len(sizeof(GC_Chat_Msg)),
        msg_id(GC_Chat_Msg_Id)
    {
    }
    // msg len, you should not change the value
    AUINT16 msg_len;
    // msg id, you should not change the value
    AUINT16 msg_id;
    // chat content
    ChatMsg data;
    // To net byte order for GC_Chat_Msg
    inline void HostToNetStruct()
    {
        msg_len = CCommonUtil::HostToNet16(msg_len);
        msg_id = CCommonUtil::HostToNet16(msg_id);
        data.HostToNetStruct();
    }
    // To host byte order for GC_Chat_Msg
    inline void NetToHostStruct()
    {
        msg_len = CCommonUtil::NetToHost16(msg_len);
        msg_id = CCommonUtil::NetToHost16(msg_id);
        data.NetToHostStruct();
    }
    // Write function of ValueToCStr for GC_Chat_Msg
    inline const char* ValueToCStr() const
    {
        static std::string s_strMsgValue;
        s_strMsgValue.clear();
        s_strMsgValue.reserve(2048);
        // Append Field msg_len
        s_strMsgValue.append("{ml:");
        s_strMsgValue.append(CStringUtil::UInt32ToDecString(msg_len));
        s_strMsgValue.append("}");
        // Append Field msg_id
        s_strMsgValue.append("{mi:");
        s_strMsgValue.append(CStringUtil::UInt32ToDecString(msg_id));
        s_strMsgValue.append("}");
        // Append Field data
        s_strMsgValue.append("{d:");
        s_strMsgValue.append(data.ValueToCStr());
        s_strMsgValue.append("}");
        return s_strMsgValue.c_str();
    }
} GC_Chat_Msg;
static_assert( sizeof( GC_Chat_Msg ) == 268, "Check struct size failed for [GC_Chat_Msg]");

#pragma pack(pop)
#endif // _CSPROTOCOL_HPP_
