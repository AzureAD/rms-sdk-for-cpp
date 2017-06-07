## How to build libs and sample apps

Straightforward steps for building libraries and samples on supported platforms.

For detailed guidance of the code samples, see [Linux code examples](https://msdn.microsoft.com/en-us/Library/mt283720(v=vs.85).aspx) on MSDN.

---
## Supported targets
- [Windows](#windows)
- [Ubuntu 14.04](#ubuntu-1404)

### Windows

1. Install Visual Studio 2015

2. Install Qt Creator. Select the msvc2015 component (32-bit) in the installer prompt under Qt 5.7. Also select the 64-bit kit if you want to build 64-bit binaries. Select the QtWebEngine component as well.
  ```
  open https://www.qt.io/download-open-source/
  download the last version of Qt Creator and install it
  ```

3. Install git
  ```
  open https://git-scm.com/download/win. The latest version will be downloaded automatically
  install downloaded version
  ```

4. Clone this repo:
  ```
  open git-bash
  enter the directory you want to clone project in
  execute command: git clone https://github.com/AzureAD/rms-sdk-for-cpp.git
  ```

5. Install Openssl for Windows, built for Visual C++ 2015.

6. Copy openssl environment
  ```
  in cloned folder 'rms-sdk-for-cpp' create subfolder 'third_party'
  copy 'include' folder from OpenSSL installed directory to 'third_party'
  copy libraries from OpenSSL\lib\vc\static to third_party\lib\eay
  ```
  
7. Build projects using Qt Creator. The project for the SDK is sdk\sdk.pro. The project for the samples is in samples\samples.pro. Build sdk.pro before samples.pro. Use the following steps to build each project.
  ```
  Select the Desktop Qt5.7.0 MSVC2015 32-bit or 64-bit kit while opening the .pro file.
  Right click the project in the Projects explorer and click Rebuild.
  Run qmake every time you change the .pro file before building (qmake generates the makefiles which nmake builds).
  ```

8. To run sample apps from Qt Creator, add the path to the OpenSSL libs in the Qt environment.
 * Press Ctrl+5
 * Go to PATH in Build Environment
 * Add ..third_party\lib\eay to the PATH environment variable.
 * Right click on rms_sample or rmsauth_sample to run it.

### Ubuntu 14.04

1. Install Qt Creator. In the installer prompt under Qt 5.7 select Desktop gcc 64-bit, Qt WebEngine, and Sources.
  ```
  Open https://www.qt.io/download-open-source/
  Download version 5.7 of Qt Creator and install it
  ```

  make sure that ```qmake --version``` returns version 5.7. 

  In our case  ```qmake --version``` returns this:
  ```
  QMake version 3.0
  using Qt version 5.7.1 in /home/<user>/Qt/5.7/gcc_64/lib
  ```
  
  if not, change PATH to include the location of qmake ```PATH=/home/<user>/Qt/5.7/gcc_64/bin```

2. Install dev dependencies:
  ```
  sudo apt-get install libssl-dev
  sudo apt-get install libsecret-1-dev
  sudo apt-get install freeglut3-dev
  ```

3. Install git:
  ```
  sudo apt-get install git
  ```

4. Install g++
  ```
  sudo apt-get install build-essential g++
  ```

5. Clone this repo:
  ```
  enter the directory you want to clone the project in
  execute command: git clone https://github.com/AzureAD/rms-sdk-for-cpp
  ```

6. Build libraries:
  ```
  cd rms-sdk-for-cpp/sdk
  qmake
  make
  ```

7. Build sample applications:
  ```
  cd ../samples
  qmake
  make
  ```

  Alternatively, you can open Qt Creator and build rms-sdk-for-cpp/sdk/sdk.pro and rms-sdk-for-cpp/samples/samples.pro.  

8. Run sample applications:
  ```
  cd ../bin
  export LD_LIBRARY_PATH=`pwd`
  ./rms_sample	    # RMS sample
  ./rmsauth_sample	# auth sample
  ```

  Alternatively, you can run the sample application from Qt Creator.

9. Create a tarball (to deploy apps):
  ```
  tar czf sample_apps.tar.gz ./rms_sample ./rmsauth_sample ./librmsauth.so ./librmsauthWebAuthDialog.so ./librms.so ./librmscrypto.so
  ```