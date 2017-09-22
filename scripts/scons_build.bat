@echo off

REM set third_party_path=%1
REM set qmake_path=%2
REM set platform=%3
set DIRECTORY_NAME=%1

rmdir /Q /S %DIRECTORY_NAME%
mkdir %DIRECTORY_NAME%

set PATH=C:\Qt\5.7\msvc2015\bin;C:\Qt\5.7\msvc2015_64\bin;%PATH%

rmdir /Q /S bin
rmdir /Q /S third_party
mkdir third_party
xcopy "C:\third_party_msvc15win64" third_party /e /y

call rmdir /Q /S unittest_results
call mkdir unittest_results

call scons --arch=x64 --configuration=release --package -c
call scons --arch=x64 --configuration=release --package
if %ERRORLEVEL% NEQ 0 ( 
	exit /B 1
)
call scripts\\scons_unittest_helper.bat bin\\release\\amd64\\sdk unittest_results release_amd64_results.txt
call mkdir %DIRECTORY_NAME%\\release
call mkdir %DIRECTORY_NAME%\\release\\amd64
call copy bin\\release\\amd64\\sdk\\rms*.dll %DIRECTORY_NAME%\\release\\amd64

call scons --arch=x64 --configuration=debug --package -c
call scons --arch=x64 --configuration=debug --package
if %ERRORLEVEL% NEQ 0 ( 
	exit /B 1
)
call scripts\\scons_unittest_helper.bat bin\\debug\\amd64\\sdk unittest_results debug_amd64_results.txt
call mkdir %DIRECTORY_NAME%\\debug
call mkdir %DIRECTORY_NAME%\\debug\\amd64
call copy bin\\debug\\amd64\\sdk\\rms*.dll %DIRECTORY_NAME%\\debug\\amd64

rmdir /Q /S third_party
mkdir third_party
xcopy "C:\third_party_msvc15win32" third_party /e /y

call scons --arch=x86 --configuration=release --package -c
call scons --arch=x86 --configuration=release --package
if %ERRORLEVEL% NEQ 0 ( 
	exit /B 1
)
call scripts\\scons_unittest_helper.bat bin\\release\\x86\\sdk unittest_results release_x86_results.txt
call mkdir %DIRECTORY_NAME%\\release
call mkdir %DIRECTORY_NAME%\\release\\x86
call copy bin\\release\\x86\\sdk\\rms*.dll %DIRECTORY_NAME%\\release\\x86

call scons --arch=x86 --configuration=debug --package -c
call scons --arch=x86 --configuration=debug --package
if %ERRORLEVEL% NEQ 0 ( 
	exit /B 1
)
call scripts\\scons_unittest_helper.bat bin\\debug\\x86\\sdk unittest_results debug_x86_results.txt
call mkdir %DIRECTORY_NAME%\\debug
call mkdir %DIRECTORY_NAME%\\debug\\x86
call copy bin\\debug\\x86\\sdk\\rms*.dll %DIRECTORY_NAME%\\debug\\x86
