call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\Common7\Tools\VsDevCmd.bat"
mkdir ..\third_party
xcopy C:\third_party ..\third_party /e
cd ..\sdk
call C:\Qt\5.7\msvc2013\bin\qmake.exe sdk.pro -spec win32-msvc2013 "CONFIG+=debug_and_release" 
call C:\Qt\Tools\QtCreator\bin\jom.exe clean
call C:\Qt\Tools\QtCreator\bin\jom.exe
cd ..\samples
call C:\Qt\5.7\msvc2013\bin\qmake.exe samples.pro -spec win32-msvc2013 "CONFIG+=debug_and_release" 
call C:\Qt\Tools\QtCreator\bin\jom.exe clean
call C:\Qt\Tools\QtCreator\bin\jom.exe
cd ..\bin
copy tests\*UnitTest* .
xcopy C:\Qt\5.7\msvc2013\bin\*.dll .
rmscryptoUnitTests.exe -xunitxml > tests\rmsUnitTestResults.txt
::rmsauthUnitTests.exe -xunitxml >> tests\rmsUnitTestResults.txt
rmsplatformUnitTests.exe -xunitxml >> tests\rmsUnitTestResults.txt
cd ..