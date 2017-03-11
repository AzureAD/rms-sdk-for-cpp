@echo off

::Inside rms-sdk-for-cpp directory
if exist rmsAuthDeploy\ rd /q /s rmsAuthDeploy\
call mkdir rmsAuthDeploy

call copy bin\rmsauth.dll rmsAuthDeploy\.
call copy bin\rmsauth.lib rmsAuthDeploy\.
call copy bin\rmsauth.exp rmsAuthDeploy\.
call copy bin\rmsauthWEbAuthDialog.dll rmsAuthDeploy\.
call copy bin\rmsauthWEbAuthDialog.lib rmsAuthDeploy\.
call copy bin\rmsauthWEbAuthDialog.exp rmsAuthDeploy\.

call copy third_party\lib\eay\ssleay32.dll rmsAuthDeploy\.
call copy third_party\lib\eay\libeay32.dll rmsAuthDeploy\.

call cd rmsAuthDeploy
call windeployqt rmsauth.dll rmsauthWEbAuthDialog.dll

call cd ..