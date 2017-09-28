REM
REM Note, this should point to the MSBuild.exe on your machine
REM

SET MSBUILD="C:\Program Files (x86)\MSBuild\14.0\Bin\MSBuild.exe"

REM Check for missing parameters, this batch file needs 6 of them
IF %1.==. GOTO No1
IF %2.==. GOTO No2
If %3.==. GOTO No3
If %4.==. GOTO No4
If %5.==. GOTO No5
If %6.==. GOTO No6

REM Save parms
SET PROJECT=%1
SET PROJECTEXT=%2
SET CONFIGURATION=%3
SET PLATFORM=%4
SET TARGET=%5
SET FOLDER=%6

SET PROJECTFILEPATH=..\%PROJECT%\%PROJECT%.%PROJECTEXT%

REM Do the build
%MSBUILD% %PROJECTFILEPATH% /maxcpucount:24 /p:Configuration=%CONFIGURATION% /p:Platform=%PLATFORM% /fileLogger /flp:LogFile=log.%FOLDER%.txt /target:%TARGET%

REM Determine filename of generated binary
SET LASTCONFIGLETTER=%CONFIGURATION:~-1%
if %LASTCONFIGLETTER%==A (
	if %PROJECT%==OpenEBTS (
		SET OUTPUTFILENAME=OpenEBTSa.dll
	) else (
		SET OUTPUTFILENAME=OpenEBTSViewera.exe
	)
) else (
	if %PROJECT%==OpenEBTS (
		SET OUTPUTFILENAME=OpenEBTS.dll
	) else if %PROJECT%==OpenEBTSViewer (
		SET OUTPUTFILENAME=OpenEBTSViewer.exe
	) else (
		SET OUTPUTFILENAME=OpenEBTSNET.dll
	)
)

if %PLATFORM%==Win32 (
SET OUTPUTPATH="..\%PROJECT%\%CONFIGURATION%\%OUTPUTFILENAME%"
) else if %PLATFORM%==x86 (
SET OUTPUTPATH="..\%PROJECT%\bin\x86\%CONFIGURATION%\%OUTPUTFILENAME%"
) else (
SET OUTPUTPATH="..\%PROJECT%\x64\%CONFIGURATION%\%OUTPUTFILENAME%"
)

REM Make folder and copy output
mkdir %FOLDER%
copy %OUTPUTPATH% %FOLDER%

Goto End

:No1
    ECHO Missing parameter #1 (projectname, e.g., OpenEBTS, OpenEBTSViewer)
    GOTO End
:No2
    ECHO Missing parameter #2 (project file extension, e.g., vcxproj, csproj)
    GOTO End
:No3
    ECHO Missing parameter #3 (configuration, e.g., Debug, Release..)
    GOTO End
:No4
    ECHO Missing parameter #4 (platform, e.g., Win32, x64)
    GOTO End
:No5
    ECHO Missing parameter #5 (target, e.g., Build, Rebuild, Clean..)
    GOTO End
:No6
    ECHO Missing parameter #6 (folder, e.g., OpenEBTS.UNICODE.x86.Release)
    GOTO End

:End
