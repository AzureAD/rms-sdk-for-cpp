::@echo off

set buildtarget=%1
set msvcver=%2
set build_type=%3

IF /I "%msvcver%"=="msvc2015" (
	set VCINSTALLDIR="C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\"
)
IF /I "%msvcver%"=="msvc2013" (
	set VCINSTALLDIR="C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\"
)

IF /I "%msvcver%"=="msvc2015" (
	IF /I "%buildtarget%"=="amd64" (
		set windeployqtpath=msvc2015_64
	)
	IF /I "%buildtarget%"=="x86" (
		set windeployqtpath=msvc2015
	)
)
IF /I "%msvcver%"=="msvc2013" (
	IF /I "%buildtarget%"=="amd64" (
		set windeployqtpath=msvc2013_64
	)
	IF /I "%buildtarget%"=="x86" (
		set windeployqtpath=msvc2013
	)
)

::Inside rms-sdk-for-cpp directory
if exist rmsSdkDeploy\ rd /q /s rmsSdkDeploy\
call mkdir rmsSdkDeploy

IF /I "%build_type%"=="debug" (
	call copy bin\rmsd.dll rmsSdkDeploy\.
	call copy bin\rmsd.lib rmsSdkDeploy\.
	call copy bin\rmsd.exp rmsSdkDeploy\.
	call copy bin\rmsd.pdb rmsSdkDeploy\.
	call copy bin\rmscryptod.dll rmsSdkDeploy\.
	call copy bin\rmscryptod.lib rmsSdkDeploy\.
	call copy bin\rmscryptod.exp rmsSdkDeploy\.
	call copy bin\rmscryptod.pdb rmsSdkDeploy\.

	call cd rmsSdkDeploy
	call C:\Qt\5.7\%windeployqtpath%\bin\windeployqt.exe rmsd.dll rmscryptod.dll
)

IF /I "%build_type%"=="release" (
	call copy bin\rms.dll rmsSdkDeploy\.
	call copy bin\rms.lib rmsSdkDeploy\.
	call copy bin\rms.exp rmsSdkDeploy\.
	call copy bin\rmscrypto.dll rmsSdkDeploy\.
	call copy bin\rmscrypto.lib rmsSdkDeploy\.
	call copy bin\rmscrypto.exp rmsSdkDeploy\.
	
	call cd rmsSdkDeploy
	call C:\Qt\5.7\%windeployqtpath%\bin\windeployqt.exe rms.dll rmscrypto.dll
)

call cd ..

call copy third_party\lib\eay\ssleay32.dll rmsSdkDeploy\.
call copy third_party\lib\eay\libeay32.dll rmsSdkDeploy\.

call copy "THIRD PARTY NOTICES - Azure Info Protection SDK (w QT Libraries).txt" rmsSdkDeploy\.
