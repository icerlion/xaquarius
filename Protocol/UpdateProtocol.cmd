@rem please build ProtocolHelper first
start "Update Protocol" /w /d.\ "..\PublicTool\ProtocolHelper\publish\ProtocolHelper.exe"
@rem copy for CSharp project
xcopy /y ".\CSharp\Protocol\CSProtocol.cs" "..\ClientGroup\CSharpClient\CSProtocol.cs"
xcopy /y ".\CSharp\Protocol\ServerMsgHandler.cs" "..\ClientGroup\CSharpClient\ServerMsgHandler.cs"
xcopy /y ".\CSharp\Protocol\VerifyProtocol.cs" "..\ClientGroup\CSharpClient\VerifyProtocol.cs"
@rem copy for Unity project
xcopy /y ".\CSharp\Protocol\CSProtocol.cs" "..\ClientGroup\UnityClient\Assets\Script\NetIOLayer\CSProtocol.cs"
xcopy /y ".\CSharp\Protocol\ServerMsgHandler.cs" "..\ClientGroup\UnityClient\Assets\Script\NetIOLayer\ServerMsgHandler.cs"
xcopy /y ".\CSharp\Protocol\VerifyProtocol.cs" "..\ClientGroup\UnityClient\Assets\Script\NetIOLayer\VerifyProtocol.cs"
@pause
