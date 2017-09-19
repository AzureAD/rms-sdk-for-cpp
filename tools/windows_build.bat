call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\Common7\Tools\VsDevCmd.bat"
set third_party_path=%1
set qmake_path=%2
set platform=%3
if %platform%==amd64 (
	call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" amd64
)
rmdir /Q /S ..\bin
rmdir /Q /S ..\third_party
mkdir ..\third_party
xcopy %third_party_path% ..\third_party /e /y
cd ..\sdk
%qmake_path%\qmake.exe sdk.pro -spec win32-msvc2015 "CONFIG+=release"
C:\Qt\Tools\QtCreator\bin\jom.exe qmake_all
C:\Qt\Tools\QtCreator\bin\jom.exe clean
C:\Qt\Tools\QtCreator\bin\jom.exe
cd ..\samples
%qmake_path%\qmake.exe samples.pro -spec win32-msvc2015 "CONFIG+=release"
C:\Qt\Tools\QtCreator\bin\jom.exe qmake_all
C:\Qt\Tools\QtCreator\bin\jom.exe clean
C:\Qt\Tools\QtCreator\bin\jom.exe
cd ..\bin
xcopy tests\*UnitTest* . /e /y
set PATH=%qmake_path%;%PATH%
set PATH=..\third_party\lib\eay;%PATH%
rmscryptoUnitTests.exe -xunitxml > tests\rmsUnitTestResults.txt
if %ERRORLEVEL% NEQ 0 ( 
	exit /B 1
)
::rmsauthUnitTests.exe -xunitxml >> tests\rmsUnitTestResults.txt
rmsplatformUnitTests.exe -xunitxml >> tests\rmsUnitTestResults.txt
if %ERRORLEVEL% NEQ 0 ( 
	exit /B 1
)
RestClientsUnitTests.exe -xunitxml >> tests\rmsUnitTestResults.txt
if %ERRORLEVEL% NEQ 0 ( 
	exit /B 1
)
xmpFileUnitTests.exe -xunitxml >> tests\rmsUnitTestResults.txt
if %ERRORLEVEL% NEQ 0 ( 
	exit /B 1
)
pFileUnitTests.exe -xunitxml >> tests\rmsUnitTestResults.txt
if %ERRORLEVEL% NEQ 0 ( 
	exit /B 1
)
commonUnitTests.exe -xunitxml >> tests\rmsUnitTestResults.txt
if %ERRORLEVEL% NEQ 0 ( 
	exit /B 1
)

cd ..