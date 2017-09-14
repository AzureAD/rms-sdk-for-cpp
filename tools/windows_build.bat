call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\Common7\Tools\VsDevCmd.bat"
set third_party_path=%1
set qmake_path=%2
set platform=%3
set build_type=%4
if %platform%==amd64 (
	echo Running vcvarsall for amd64
  call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" amd64
)
if %platform%==x86 (
  echo Running vcvarsall for x86
  call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" x86
)
mkdir ..\third_party
xcopy %third_party_path% ..\third_party /e
cd ..\sdk
call %qmake_path%\qmake.exe sdk.pro -spec win32-msvc2013 "CONFIG+="%build_type% 
call C:\Qt\Tools\QtCreator\bin\jom.exe clean
call C:\Qt\Tools\QtCreator\bin\jom.exe
cd ..\samples
call %qmake_path%\qmake.exe samples.pro -spec win32-msvc2013 "CONFIG+="%build_type% 
call C:\Qt\Tools\QtCreator\bin\jom.exe clean
call C:\Qt\Tools\QtCreator\bin\jom.exe
cd ..\bin
copy tests\*UnitTest* .
set PATH=%qmake_path%;%PATH%
set PATH=..\third_party\lib\eay;%PATH%

echo *********************************************************************************
echo Running Tests for %build_type% build!

if %build_type%==debug (
  call rmscryptoUnitTestsd.exe -xunitxml > tests\rmsUnitTestResults.txt
  ::rmsauthUnitTestsd.exe -xunitxml >> tests\rmsUnitTestResults.txt
  call rmsplatformUnitTestsd.exe -xunitxml >> tests\rmsUnitTestResults.txt
)
if %build_type%==release (
  call rmscryptoUnitTests.exe -xunitxml > tests\rmsUnitTestResults.txt
  ::rmsauthUnitTests.exe -xunitxml >> tests\rmsUnitTestResults.txt
  call rmsplatformUnitTests.exe -xunitxml >> tests\rmsUnitTestResults.txt
)

cd ..