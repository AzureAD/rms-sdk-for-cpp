
cd ../sdk
qmake -recursive
make clean
make
if [ $? -ne 0 ]; then
  echo "sdk make failed."
  exit 1
fi
echo "SDK done."
cd ../samples
qmake -recursive
make clean
make
if [ $? -ne 0 ]; then
  echo "sdk make failed."
  exit 1
fi
echo "Samples done."
export QT_XKB_CONFIG_ROOT=/usr/share/X11/xkb
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
