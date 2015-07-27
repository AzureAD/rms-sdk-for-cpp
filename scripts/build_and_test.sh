#!/bin/bash

log() {
    local file_name=${0##*/}
    local timestamp=$(date +"%Y-%m-%d %H:%M:%S")
    echo "$timestamp $file_name: $1"
}

QMAKE=qmake
LIB_SUFFIX=.so
UT_SUFFIX=
STRIP_OPTIONS=--strip-unneeded
VARS="CONFIG+=release"
TARGET_DIR=./dist/release
DEFINES=

BUILD=true
DEBUG=false
TEST=false
SAMPLE=true
TRAVIS=false
while [ $# -gt 0 ]; do
  case $1 in
    debug) DEBUG=true
      ;;
    test)  TEST=true
      ;;
    sample) SAMPLE=true
      ;;
    travis) TRAVIS=true
      ;;
  esac
  shift
done

if [ $TRAVIS == 'true' ]; then
    echo "TRAVIS Build"
    DEFINES="DEFINES+=TRAVIS_BUILD"
    if [ $TRAVIS_OS_NAME == osx ]; then 
        LIB_SUFFIX=.dylib
        STRIP_OPTIONS= 
    fi
else
    UNAME=`uname`
    echo "UNAME: $UNAME"
    if [ $UNAME == "Darwin" ]; then
       LIB_SUFFIX=.dylib
       STRIP_OPTIONS=
    elif [ $UNAME == "Linux" ]; then
      DISTRO=$(awk -F'=' '{if($1=="ID")print $2; }' /etc/*-release)
      echo "DISTRO: $DISTRO"
      if [ $DISTRO == "" ]; then
          log "Error reading DISTRO"
          exit 1
      elif [ ${DISTRO^^} == "UBUNTU" ]; then
          type qmake >/dev/null 2>&1 || { echo >&2 log "qmake is required but not installed."; exit 1; }
      else # CentOS & OpenSUSE
          QMAKE=qmake-qt5
          type qmake-qt5 >/dev/null 2>&1 || { echo >&2 log "qmake-qt5 is required but not installed."; exit 1; }
      fi
    fi
fi

#get the script location
pushd `dirname $0` > /dev/null
SCRIPT_DIR=`pwd`
REPO_ROOT=$SCRIPT_DIR/..
popd > /dev/null
echo "SCRIPT_DIR: $SCRIPT_DIR"
echo "REPO_ROOT : $REPO_ROOT"

if [ $DEBUG == 'true' ]; then
   echo "DEBUG Build"
   VARS="CONFIG+=debug"
   TARGET_DIR=./dist/debug
   LIB_SUFFIX=d${LIB_SUFFIX}
   UT_SUFFIX=d
fi

if [ $BUILD == 'true' ]; then
  echo "=== Building sdk libraries ==="
  echo "--> Entering sdk directory..."
  cd $REPO_ROOT/sdk
  echo "--> Running $QMAKE $DEFINES $VARS -recursive"
  $QMAKE $DEFINES $VARS -recursive
  echo "--> Running make, please see sdk_build.log for details..."
  make | tee sdk_build.log 2>&1
  cd $REPO_ROOT
fi

echo "=== Prepping reusable libs and includes in ${TARGET_DIR}"
mkdir -p ${TARGET_DIR}/usr/include/rms
mkdir -p ${TARGET_DIR}/usr/include/rmsauth
mkdir -p ${TARGET_DIR}/usr/include/rmscrypto
mkdir -p ${TARGET_DIR}/usr/lib

cp bin/librms${LIB_SUFFIX} ${TARGET_DIR}/usr/lib
cp bin/librmsauth${LIB_SUFFIX} ${TARGET_DIR}/usr/lib
cp bin/librmscrypto${LIB_SUFFIX} ${TARGET_DIR}/usr/lib
cp bin/librmsauthWebAuthDialog${LIB_SUFFIX} ${TARGET_DIR}/usr/lib

strip ${STRIP_OPTIONS} ${TARGET_DIR}/usr/lib/librms${LIB_SUFFIX} > /dev/null 2>&1
strip ${STRIP_OPTIONS} ${TARGET_DIR}/usr/lib/librmsauth${LIB_SUFFIX} > /dev/null 2>&1
strip ${STRIP_OPTIONS} ${TARGET_DIR}/usr/lib/librmsauthWebAuthDialog${LIB_SUFFIX} > /dev/null 2>&1
strip ${STRIP_OPTIONS} ${TARGET_DIR}/usr/lib/librmscrypto${LIB_SUFFIX} > /dev/null 2>&1

cp sdk/rms_sdk/ModernAPI/*.h ${TARGET_DIR}/usr/include/rms
cp sdk/rmsauth_sdk/rmsauth/rmsauth/*.h ${TARGET_DIR}/usr/include/rmsauth
cp sdk/rmscrypto_sdk/CryptoAPI/*.h ${TARGET_DIR}/usr/include/rmscrypto

if [ $TEST == 'true' ]; then
  echo "--> Starting unit tests..."
  cd $REPO_ROOT/bin
  export LD_LIBRARY_PATH=`pwd`
  if [ -e "./tests/rmsauthUnitTests$UT_SUFFIX" ]; then
      # Please use --interactive option to start interactive test cases as well
      # Need to add encrypted credentials to test non-interactive cases
      # ./tests/rmsauthUnitTests$UT_SUFFIX
      echo "skipping rmsauth tests"
  else
    echo "!!! unit tests for rmsauth not found"
  fi
  if [ -e "./tests/rmscryptoUnitTests$UT_SUFFIX" ]; then
      ./tests/rmscryptoUnitTests$UT_SUFFIX
  else
    echo "!!! unit tests for rmscrypto not found"
  fi
  if [ -e "./tests/rmsplatformUnitTests$UT_SUFFIX" ]; then
      ./tests/rmsplatformUnitTests$UT_SUFFIX
  else
    echo "!!! unit tests for rmsplatform not found"
  fi
fi

if [ $SAMPLE == 'true' ]; then
  echo "=== Building sample applications ==="
  echo "--> Entering samples directory..."
  cd $REPO_ROOT/samples
  $QMAKE $VARS -recursive
  echo "--> Running make, please see sample_apps_build.log for details..."
  make | tee sample_apps_build.log 2>&1
  cd $REPO_ROOT
fi

echo "=== This was built:"
find $REPO_ROOT/bin
