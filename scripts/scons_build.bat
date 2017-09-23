@echo off
SETLOCAL enabledelayedexpansion 
set DIRECTORY_NAME=%1

rmdir /Q /S %DIRECTORY_NAME%

set archs=x64 x86
set configs=release debug

call rmdir /Q /S unittest_results
call mkdir unittest_results

set PATH=C:\Qt\5.7\msvc2015\bin;C:\Qt\5.7\msvc2015_64\bin;%PATH%
set PATH=third_party\lib\eay;third_party\lib\Libgsf;third_party\lib\xmp;%PATH%

for %%a in (%archs%) do (
	rmdir /Q /S third_party\include
	rmdir /Q /S third_party\lib
	if %%a == x64 (
		xcopy "C:\third_party_msvc15win64" third_party /e /y
	)
	if %%a == x86 (
		xcopy "C:\third_party_msvc15win32" third_party /e /y
	)
	for %%c in  (%configs%) do (
		echo scons -j 4 --arch=%%a --configuration=%%c --package
		call scons -j 4 --arch=%%a --configuration=%%c --package -c
		call scons -j 4 --arch=%%a --configuration=%%c --package
		if %ERRORLEVEL% NEQ 0 (
			exit /B 1
		)
		if %%a == x64 (
			set arch_path=amd64
		) else (
			set arch_path=x86
		)
		mkdir %DIRECTORY_NAME%\%%c
		mkdir %DIRECTORY_NAME%\%%c\!arch_path!
		echo scripts\scons_unittest_helper.bat bin\%%c\!arch_path!\sdk unittest_results %%c_!arch_path!_results
		echo copy bin\%%c\!arch_path!\sdk\rms*.dll %DIRECTORY_NAME%\%%c\!arch_path!
		call scripts\scons_unittest_helper.bat bin\%%c\!arch_path!\sdk unittest_results %%c_!arch_path!_results
		if %ERRORLEVEL% NEQ 0 (
			exit /B 1
		)
		call copy bin\%%c\!arch_path!\sdk\rms*.dll %DIRECTORY_NAME%\%%c\!arch_path!
	)
)
