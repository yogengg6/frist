CALL builder OpenEBTS vcxproj ReleaseA Win32 Rebuild "OpenEBTS.ASCII.x86.Release (EN)"
CALL builder OpenEBTS vcxproj Release Win32 Rebuild "OpenEBTS.UNICODE.x86.Release (EN)"
CALL builder OpenEBTS vcxproj ReleaseSpanish Win32 Rebuild "OpenEBTS.UNICODE.x86.Release (ES)"

CALL builder OpenEBTS vcxproj ReleaseA x64 Rebuild "OpenEBTS.ASCII.x64.Release (EN)"
CALL builder OpenEBTS vcxproj Release x64 Rebuild "OpenEBTS.UNICODE.x64.Release (EN)"
CALL builder OpenEBTS vcxproj ReleaseSpanish x64 Rebuild "OpenEBTS.UNICODE.x64.Release (ES)"

CALL builder OpenEBTSNET csproj Release x86 Rebuild "OpenEBTSNET.UNICODE.x86.Release (EN)"
CALL builder OpenEBTSNET csproj ReleaseSigCheck x86 Rebuild "OpenEBTSNET.UNICODE.x86.Release.SigCheck (EN)"

CALL builder OpenEBTSViewer vcxproj Release Win32 Rebuild "OpenEBTSViewer.UNICODE.x86.Release (EN)"
CALL builder OpenEBTSViewer vcxproj Release x64 Rebuild "OpenEBTSViewer.UNICODE.x64.Release (EN)"
