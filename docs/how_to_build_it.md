## How to build libs and sample apps

Straightforward steps for building libraries and samples on supported platforms.

---
## Supported targets
- [Ubuntu 14.04](#ubuntu-1404)
- [OpenSUSE 13.2](#opensuse-132)
- [CentOS 7](#centos-7)

Libs and samples have been successfully compiled on Windows and OSX as well, but these are not fully supported at this time.

### Ubuntu 14.04

1. Install dev dependencies:
  ```
  sudo apt-get install qt5-default
  sudo apt-get install libqt5webkit5-dev
  sudo apt-get install libqt5xmlpatterns5-dev
  sudo apt-get install libssl-dev
  sudo apt-get install libsecret-1-dev
  ```

2. Clone this repo:
  ```
  sudo apt-get install git
  git clone https://github.com/AzureAD/rms-sdk-for-cpp
  cd rms-sdk-for-cpp
  ```

3. Build and install libraries:
  ```
  cd sdk
  qmake
  make
  sudo make install
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

3. Build and install libraries:
  ```
  cd sdk
  qmake-qt5
  make
  sudo make install
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

3. Build and install libraries:
  ```
  cd sdk
  qmake-qt5
  make
  sudo make install
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
  ./rms_sample	    # RMS sample
  ./rmsauth_sample	# auth sample
  ```

6. Create a tarball (to deploy apps):
  ```
  tar czf sample_apps.tar.gz ./rms_sample ./rmsauth_sample ./librmsauth.so ./librmsauthWebAuthDialog.so ./librms.so ./librmscrypto.so
  ```
