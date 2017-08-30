
cd ../sdk
qmake
make clean
make
cd ../samples
qmake
make clean
make
cd ../bin
export LD_LIBRARY_PATH=`pwd`
cd tests
./rmscryptoUnitTests -xunitxml > rmsUnitTestResults.txt
./rmsauthUnitTests -xunitxml >> rmsUnitTestResults.txt
./rmsplatformUnitTests -xunitxml >> rmsUnitTestResults.txt
./RestClientsUnitTests -xunitxml >> rmsUnitTestResults.txt
./xmpFileUnitTests -xunitxml >> rmsUnitTestResults.txt
./commonUnitTests -xunitxml >> rmsUnitTestResults.txt
cd ../../tools
powershell ./Rms_Sdk_For_Cpp_UTResults_Parser.ps1
