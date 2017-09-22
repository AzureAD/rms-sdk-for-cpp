@echo off

set SOURCE=%1
set DESTINATION=%2
set TARGET=%3

echo %SOURCE%
echo %DESTINATION%\%TARGET%
%SOURCE%\\rmscrypto_sdk\\unittests.exe -xunitxml > %DESTINATION%\%TARGET%
if %ERRORLEVEL% NEQ 0 ( 
	exit /B 1
)
::rmsauthUnitTests.exe -xunitxml >> tests\rmsUnitTestResults.txt
%SOURCE%\\rms_sdk\\platform_ut.exe -xunitxml >> %DESTINATION%\\%TARGET%
if %ERRORLEVEL% NEQ 0 ( 
	exit /B 1
)
%SOURCE%\\rms_sdk\\rest_clients_ut.exe -xunitxml >> %DESTINATION%\\%TARGET%
if %ERRORLEVEL% NEQ 0 ( 
	exit /B 1
)
%SOURCE%\\rms_sdk\\profile_ut.exe -xunitxml >> %DESTINATION%\\%TARGET%
if %ERRORLEVEL% NEQ 0 ( 
	exit /B 1
)
%SOURCE%\\file_sdk\\file_sdk_unittest.exe -xunitxml >> %DESTINATION%\\%TARGET%
if %ERRORLEVEL% NEQ 0 ( 
	exit /B 1
)
REM %SOURCE%\\<TBD>\\pFileUnitTests.exe -xunitxml >> %DESTINATION%\\%TARGET%
REM if %ERRORLEVEL% NEQ 0 ( 
REM 	exit /B 1
REM )
REM %SOURCE%\\<TBD>\\commonUnitTests.exe -xunitxml >> %DESTINATION%\\%TARGET%
REM if %ERRORLEVEL% NEQ 0 ( 
REM 	exit /B 1
REM )

