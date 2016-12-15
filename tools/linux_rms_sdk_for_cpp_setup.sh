#Eventually I want this script to run inside AzureAD/rms-sdk-for-cpp
#so the path of some files may change in the future
#!/bin/sh
apt-get update
echo $1 | sudo -S sudo apt-get install qt5-default
echo $1 | sudo -S sudo apt-get install libqt5webkit5-dev
echo $1 | sudo -S sudo apt-get install libqt5xmlpatterns5-dev
echo $1 | sudo -S sudo apt-get install libssl-dev
echo $1 | sudo -S sudo apt-get install libsecret-1-dev
echo $1 | sudo -S sudo apt-get install freeglut3-dev
echo $1 | sudo -S sudo apt-get install git
cd ~
git clone https://github.com/AzureAD/rms-sdk-for-cpp
cd rms-sdk-for-cpp
git pull
cd sdk
qmake
make
cd ../samples
qmake
make
cd ../bin
export LD_LIBRARY_PATH=`pwd`
cd tests
#mv tests/*UnitTests .
./rmscryptoUnitTests -xunitxml > rmsUnitTestResults.txt
./rmsauthUnitTests -xunitxml >> rmsUnitTestResults.txt
./rmsplatformUnitTests -xunitxml >> rmsUnitTestResults.txt
#The script rmsUnitTestParser.ps1 should also be in AzureAD/rms-sdk-for-cpp
#but for now it is hard coded into /myagent on the linux VM
cd ../../../myagent
powershell ./Rms_Sdk_For_Cpp_UTResults_Parser.ps1
