To compile file_sdk:

1. Download Adobe xmp sdk from: http://download.macromedia.com/pub/developer/xmp/sdk/XMP-Toolkit-SDK-CC201607.zip
2. Extract the zip file
3. Follow the instructions in <xmp-sdk>/third-party folder and then follow the instruction in the <xmp-sdk>/build folder to generate static library.

For Windows:
4. Copy XMP-Toolkit-SDK-CC201607\public\include folder to rms-sdk-for-cpp\third_party\include\xmp
5. Copy XMPCoreStatic.lib and XMPFilesStatic.lib to rms-sdk-for-cpp\third_party\lib\xmp (for debug build, append "D" to the name of the files: XMPFilesStaticD.lib, XMPFilesStaticD.lib)

For linux:
4. Copy XMP-Toolkit-SDK-CC201607\public\include folder to /usr/include/xmp/
5. Rename staticXMPFiles.ar to libXMPFiles.a and staticXMPCore.ar to libXMPCore.a
6. Copy libXMPFiles.aand libXMPCore.a to /usr/lib/xmp/

To compile rms_sdk with file_api

For Linux:
1. Make sure the system has pkg-config installed (which will almost always be installed)
2. Install Libgsf and its dependencies by using the following command:
    sudo apt-get install libgsf-1-dev

For Windows:

1. Follow instructions here: https://github.com/msys2/msys2/wiki/MSYS2-installation to download, install and update core packages.
2. Install LibGsf packages and its dependencies (both 64 and 32 bit) using the following command:
        64 bit: pacman -S mingw64/mingw-w64-x86_64-libgsf
        32 bit: pacman -S mingw32/mingw-w64-i686-libgsf
   The above instructions are for 64 bit installation of MSYS2.
3. Copy the following header files/folders from <MSYS inst dir>\mingw64\include to rms-sdk-for-cpp\third_party\include\Libgsf
        gmodule.h
        iconv.h
        libintl.h
        lzma.h
        pcre.h
        zconf.h
        zlib.h
        bzlib.h
        ffi.h
        ffitarget.h
        glib.h
        glibconfig.h
        glib-object.h
        glib-unix.h
        gio
        glib
        gobject
        gsf
        libxml
        lzma
   Also copy 'glibconfig.h' from <MSYS inst dir>\mingw64\lib\glib-2.0\include to rms-sdk-for-cpp\third_party\include\Libgsf
4. Copy the following DLLs from <MSYS inst dir>\mingw64\bin to rms-sdk-for-cpp\third_party\lib\Libgsf
        libglib-2.0-0.dll
        libgmodule-2.0-0.dll
        libgobject-2.0-0.dll
        libgsf-1-114.dll
        libgthread-2.0-0.dll
        libiconv-2.dll
        libintl-8.dll
        liblzma-5.dll
        libpcre-1.dll
        libwinpthread-1.dll
        libxml2-2.dll
        zlib1.dll
        libbz2-1.dll
        libffi-6.dll
        libgio-2.0-0.dll
5. Create import libs for all the above DLLs in the same folder rms-sdk-for-cpp\third_party\lib\Libgsf and add this to the 
   PATH environment variable. You can use the methods listed in
   https://support.microsoft.com/en-us/help/131313/how-to-create-32-bit-import-libraries-without--objs-or-source.
6. The steps 3-5 are for 64 bit binaries. For 32 bit binaries, do the same steps but copy the header files and DLLs from
   <MSYS inst dir>\mingw32\include and <MSYS inst dir>\mingw32\bin respectively.

To compile file_sdk_sample:

1. Downoad cxxopts: https://github.com/jarro2783/cxxopts/archive/master.zip .
2. Copy cxxopts.hpp to rms-sdk-for-cpp/third_party/include/cxxopts
>>>>>>> Product Backlog Item 1412906: Create a sample app with the ability to stub label a file
