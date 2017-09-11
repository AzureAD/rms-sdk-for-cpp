To compile file_sdk:

1. Download Adobe xmp sdk from: http://download.macromedia.com/pub/developer/xmp/sdk/XMP-Toolkit-SDK-CC201607.zip
2. Extract the zip file
3. Follow the instructions in third-party folder and then follow the instruction in the build folder to generate static library.

For Windows:
4. Copy XMP-Toolkit-SDK-CC201607\public\include folder to rms-sdk-for-cpp\third_party\include\xmp
5. Copy XMPCoreStatic.lib and XMPFilesStatic.lib to rms-sdk-for-cpp\third_party\lib\xmp

For linux:
4. Copy XMP-Toolkit-SDK-CC201607\public\include folder to /usr/include/xmp/
5. Copy staticXMPFiles and staticXMPCore to /usr/include/xmp/
