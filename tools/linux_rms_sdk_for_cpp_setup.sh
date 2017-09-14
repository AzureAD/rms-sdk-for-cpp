
cd ../sdk
qmake -recursive
make clean
make
if [ $? -ne 0 ]; then
  echo "sdk make failed."
  exit 1
fi
cd ../samples
qmake -recursive
make clean
make
if [ $? -ne 0 ]; then
  echo "sdk make failed."
  exit 1
fi
cd ../bin
export XKB_DEFAULT_RULES=base
export QT_XKB_CONFIG_ROOT=/usr/share/X11/xkb
export LD_LIBRARY_PATH=`pwd`
cd tests
./rmscryptoUnitTests -xunitxml > rmsUnitTestResults.txt
if [ $? -ne 0 ]; then
  echo "sdk make failed."
  exit 1
fi
./rmsauthUnitTests -xunitxml >> rmsUnitTestResults.txt
if [ $? -ne 0 ]; then
  echo "sdk make failed."
  exit 1
fi
./rmsplatformUnitTests -xunitxml >> rmsUnitTestResults.txt
if [ $? -ne 0 ]; then
  echo "sdk make failed."
  exit 1
fi
./RestClientsUnitTests -xunitxml >> rmsUnitTestResults.txt
if [ $? -ne 0 ]; then
  echo "sdk make failed."
  exit 1
fi
./xmpFileUnitTests -xunitxml >> rmsUnitTestResults.txt
if [ $? -ne 0 ]; then
  echo "sdk make failed."
  exit 1
fi
./commonUnitTests -xunitxml >> rmsUnitTestResults.txt
if [ $? -ne 0 ]; then
  echo "sdk make failed."
  exit 1
fi
cd ../../tools
powershell ./Rms_Sdk_For_Cpp_UTResults_Parser.ps1
if [ $? -ne 0 ]; then
  echo "sdk make failed."
  exit 1
fi
