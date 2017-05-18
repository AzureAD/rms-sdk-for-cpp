@echo off

set buildtarget=%1

IF /I "%buildtarget%"=="amd64" (
	set windeployqtpath=msvc2015_64
)
IF /I "%buildtarget%"=="x86" (
	set windeployqtpath=msvc2015
)

::Inside rms-sdk-for-cpp directory
if exist rmsSdkDeploy\ rd /q /s rmsSdkDeploy\
call mkdir rmsSdkDeploy

call copy bin\rms.dll rmsSdkDeploy\.
call copy bin\rms.lib rmsSdkDeploy\.
call copy bin\rms.exp rmsSdkDeploy\.
call copy bin\rmscrypto.dll rmsSdkDeploy\.
call copy bin\rmscrypto.lib rmsSdkDeploy\.
call copy bin\rmscrypto.exp rmsSdkDeploy\.

call copy third_party\lib\eay\ssleay32.dll rmsSdkDeploy\.
call copy third_party\lib\eay\libeay32.dll rmsSdkDeploy\.

call cd rmsSdkDeploy
call C:\Qt\5.7\%windeployqtpath%\bin\windeployqt.exe rms.dll rmscrypto.dll

call cd ..