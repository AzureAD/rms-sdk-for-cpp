@echo off

::reg Query "HKLM\Hardware\Description\System\CentralProcessor\0" | find /i "x86" > NUL && set OS=32BIT || set OS=64BIT
set buildtarget=%1
set msvcver=%2

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
if exist deploy\ rd /q /s deploy\
call mkdir deploy

call copy bin\rms*_sample.exe deploy\.
call copy bin\rms*.dll deploy\.
call copy bin\rms*.lib deploy\.
call copy bin\rms*.exp deploy\.

call copy third_party\lib\eay\ssleay32.dll deploy\.
call copy third_party\lib\eay\libeay32.dll deploy\.

call copy "THIRD PARTY NOTICES - Azure Info Protection SDK (w QT Libraries).txt" deploy\.

call cd deploy
call C:\Qt\5.7\%windeployqtpath%\bin\windeployqt.exe rms_sample.exe rmsauth_sample.exe rms.dll rmscrypto.dll rmsauth.dll rmsauthWebAuthDialog.dll

call cd ..


