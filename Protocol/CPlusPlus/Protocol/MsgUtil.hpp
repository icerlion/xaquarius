// Warning: This file is auto-generated by ParseProtocol.exe, you should not change it!
// Time: 2017-03-05 14:31:14
#ifndef _MSG_UTIL_HPP_
#define _MSG_UTIL_HPP_

#include "Protocol/ProtocolType.hpp"
#include "Protocol/CSProtocol.hpp"
#include "Protocol/SSProtocol.hpp"
#include "AquariusCore/Util/TypeReDef/TypeReDef.hpp"
#include "AquariusCore/Util/TinyUtil/StringUtil.h"

class CMsgUtil
{
public:
    // Get msg name and msg len by id
    static void GetMsgInfo(AUINT16 nMsgId, std::string& strMsgName, AUINT16& nMsgLen )
    {
        const MsgInfo& stMsgInfo = FindMsgInfo(nMsgId);
        strMsgName = stMsgInfo.strMsgName;
        nMsgLen = stMsgInfo.nMsgLen;
    }

private:
    // Define Msg Info struct
    typedef struct MsgInfo
    {
        // Constructor
        MsgInfo(const char* name = "NA", AUINT16 id = 0, AUINT16 len = 0)
          :strMsgName(name),
          nMsgId(id),
          nMsgLen(len)
        {
        }
        // Destructor
        ~MsgInfo()
        {
        }
        std::string strMsgName; // MsgName
        AUINT16 nMsgId;         // MsgId
        AUINT16 nMsgLen;        // MsgLen
    } MsgInfo;

    // Key: msg id
    // Value: msg info
    typedef std::map<AUINT32, MsgInfo> MapMsgInfo;

    // Get MapMsgInfo;
    inline static MapMsgInfo& GetMapMsgInfo()
    {
        // Hold all msg info;
        static MapMsgInfo s_mapMsgInfo;
        return s_mapMsgInfo;
    }

    // Find msg info by id, if the id is not found, the msg name will be the msg id
    static const MsgInfo& FindMsgInfo(AUINT16 nMsgId)
    {
        // Init mapMsgInfo before find it
        MapMsgInfo& mapMsgInfo = GetMapMsgInfo();
        if (mapMsgInfo.empty())
        {
            InitializeMapMsgInfo();
        }
        MapMsgInfo::const_iterator itFind = mapMsgInfo.find(nMsgId);
        if (itFind == mapMsgInfo.end())
        {
            static MsgInfo s_InvalidMsgInfo;
            s_InvalidMsgInfo.strMsgName = CStringUtil::UInt16ToHexString(nMsgId);
            s_InvalidMsgInfo.nMsgId = nMsgId;
            return s_InvalidMsgInfo;
        }
        return itFind->second;
    }

    // Insert all msg info
    static void InitializeMapMsgInfo()
    {
        MapMsgInfo& mapMsgInfo = GetMapMsgInfo();
        mapMsgInfo.insert(std::make_pair( CG_HeartBeat_Msg_Id, MsgInfo("CG_HeartBeat_Msg", CG_HeartBeat_Msg_Id, sizeof(CG_HeartBeat_Msg))));
        mapMsgInfo.insert(std::make_pair( GC_HeartBeat_Msg_Id, MsgInfo("GC_HeartBeat_Msg", GC_HeartBeat_Msg_Id, sizeof(GC_HeartBeat_Msg))));
        mapMsgInfo.insert(std::make_pair( CG_Echo_Request_Id, MsgInfo("CG_Echo_Request", CG_Echo_Request_Id, sizeof(CG_Echo_Request))));
        mapMsgInfo.insert(std::make_pair( GC_Echo_Response_Id, MsgInfo("GC_Echo_Response", GC_Echo_Response_Id, sizeof(GC_Echo_Response))));
        mapMsgInfo.insert(std::make_pair( CG_Chat_Cmd_Id, MsgInfo("CG_Chat_Cmd", CG_Chat_Cmd_Id, sizeof(CG_Chat_Cmd))));
        mapMsgInfo.insert(std::make_pair( GC_Chat_Msg_Id, MsgInfo("GC_Chat_Msg", GC_Chat_Msg_Id, sizeof(GC_Chat_Msg))));
        mapMsgInfo.insert(std::make_pair( SS_HeartBeat_Msg_Id, MsgInfo("SS_HeartBeat_Msg", SS_HeartBeat_Msg_Id, sizeof(SS_HeartBeat_Msg))));
        mapMsgInfo.insert(std::make_pair( GF_Chat_Cmd_Id, MsgInfo("GF_Chat_Cmd", GF_Chat_Cmd_Id, sizeof(GF_Chat_Cmd))));
        mapMsgInfo.insert(std::make_pair( GF_Echo_Request_Id, MsgInfo("GF_Echo_Request", GF_Echo_Request_Id, sizeof(GF_Echo_Request))));
        mapMsgInfo.insert(std::make_pair( FG_Echo_Response_Id, MsgInfo("FG_Echo_Response", FG_Echo_Response_Id, sizeof(FG_Echo_Response))));
        mapMsgInfo.insert(std::make_pair( GH_Echo_Request_Id, MsgInfo("GH_Echo_Request", GH_Echo_Request_Id, sizeof(GH_Echo_Request))));
        mapMsgInfo.insert(std::make_pair( HG_Echo_Response_Id, MsgInfo("HG_Echo_Response", HG_Echo_Response_Id, sizeof(HG_Echo_Response))));
    } // End of InitializeMapMsgInfo
};

#endif // _MSG_UTIL_HPP_

