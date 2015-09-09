## How to build libs and sample apps

Straightforward steps for building libraries and samples on supported platforms.

For detailed guidance of the code samples, see [Linux code examples](https://msdn.microsoft.com/en-us/Library/mt283720(v=vs.85).aspx) on MSDN.

---
## Supported targets
- [Ubuntu 14.04](#ubuntu-1404)
- [OpenSUSE 13.2](#opensuse-132)
- [CentOS 7.1](#centos-7)
 
Note: When the libs are compiled you will have created four libs; librms.so, librmsauth.so, librmscrypto.so, and libwebAuthDialog.so. All four are necessary if you use our ADAL for OAuth authentication.

Libs and samples have been successfully compiled on Windows and OSX as well, but these are not fully supported at this time.

### Windows

1. Install Visual Studio 2012/2013
2. Install Qt Creator
  ```
  open https://www.qt.io/download-open-source/
  download the last version of Qt Creator and install it
  ```

3. Install openssl for windows
  ```
  open https://slproweb.com/products/Win32OpenSSL.html
  download the last version of openssl and install it
  ```

4. Install git
  ```
  open https://git-scm.com/download/win. The latest version will be downloaded automatically
  install downloaded version
  ```

5. Clone this repo:
  ```
  open git-bash
  enter the directory you want to clone project in
  execute command: git clone https://github.com/AzureAD/rms-sdk-for-cpp
  ```

6. Copy openssl environment
  ```
  in cloned folder 'rms-sdk-for-cpp' create subfolder 'third_party'
  copy 'include' folder from OpenSSL installed directory to 'third_party'
  copy libraries from OpenSSL\lib\vc\static to third_party\lib\eay
  ```

7. Build projects
  ```
  Open sdk\sdk.pro project in Qt and build it
  Open samples\samples.pro project in Qt and build it
  ```

8. Run sample applications:
  ```
  cd ../bin
  rms_sample.exe	# RMS sample
  rmsauth_sample.exe	# auth sample
  ```

### Ubuntu 14.04

1. Install dev dependencies:
  ```
  sudo apt-get install qt5-default
  sudo apt-get install libqt5webkit5-dev
  sudo apt-get install libqt5xmlpatterns5-dev
  sudo apt-get install libssl-dev
  sudo apt-get install libsecret-1-dev
  sudo apt-get install freeglut3-dev
  ```

2. Clone this repo:
  ```
  sudo apt-get install git
  git clone https://github.com/AzureAD/rms-sdk-for-cpp
  cd rms-sdk-for-cpp
  ```

3. Build libraries:
  ```
  cd sdk
  qmake
  make
  ```

4. Build sample applications:
  ```
  cd ../samples
  qmake
  make
  ```

5. Run sample applications:
  ```
  cd ../bin
  export LD_LIBRARY_PATH=`pwd`
  ./rms_sample	    # RMS sample
  ./rmsauth_sample	# auth sample
  ```

6. Create a tarball (to deploy apps):
  ```
  tar czf sample_apps.tar.gz ./rms_sample ./rmsauth_sample ./librmsauth.so ./librmsauthWebAuthDialog.so ./librms.so ./librmscrypto.so
  ```

### OpenSUSE 13.2

1. Install dev dependencies:
  ```
  sudo zypper install libqt5-qtbase-devel
  sudo zypper install libQt5WebKitWidgets-devel
  sudo zypper install libQt5XmlPatterns-devel
  sudo zypper install libopenssl-devel
  sudo zypper install libsecret-devel
  ```

2. Clone this repo
  ```
  sudo zypper install git
  git clone https://github.com/AzureAD/rms-sdk-for-cpp
  ```

3. Build libraries:
  ```
  cd sdk
  qmake-qt5
  make
  ```

4. Build sample applications:
  ```
  cd ../samples
  qmake-qt5
  make
  ```

5. Run sample applications:
  ```
  cd ../bin
  export LD_LIBRARY_PATH=`pwd`
  ./rms_sample	    # RMS sample
  ./rmsauth_sample	# auth sample
  ```
6. Create a tarball (to deploy apps):
  ```
  tar czf sample_apps.tar.gz ./rms_sample ./rmsauth_sample ./librmsauth.so ./librmsauthWebAuthDialog.so ./librms.so ./librmscrypto.so
  ```

### CentOS 7

1. Install dev dependencies:
  ```
  sudo yum groupinstall "Development Tools"
  sudo yum install wget
  sudo yum install gcc-c++
  wget http://dl.fedoraproject.org/pub/epel/7/x86_64/e/epel-release-7-5.noarch.rpm
  sudo rpm -ivh epel-release-7-5.noarch.rpm
  sudo yum --enablerepo=epel install qt5-qtbase-devel
  sudo yum --enablerepo=epel install qt5-qtwebkit-devel
  sudo yum --enablerepo=epel install qt5-qtxmlpatterns-devel
  sudo yum install openssl-devel
  sudo yum install libsecret-devel
  ```

2. Clone this repo:
  ```
  git clone https://github.com/AzureAD/rms-sdk-for-cpp
  ```

3. Build libraries:
  ```
  cd sdk
  qmake-qt5
  make
  ```

4. Build sample applications:
  ```
  cd ../samples
  qmake-qt5
  make
  ```

5. Run sample applications:
  ```
  cd ../bin
  export LD_LIBRARY_PATH=`pwd`
  ./rms_sample	    # RMS sample
  ./rmsauth_sample	# auth sample
  ```

6. Create a tarball (to deploy apps):
  ```
  tar czf sample_apps.tar.gz ./rms_sample ./rmsauth_sample ./librmsauth.so ./librmsauthWebAuthDialog.so ./librms.so ./librmscrypto.so
  ```
