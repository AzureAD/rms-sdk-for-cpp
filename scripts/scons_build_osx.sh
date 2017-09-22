#!/bin/sh

TARGET_NAME=$1
DIRECTORY_NAME=$2

rm -rf $DIRECTORY_NAME

mkdir -p $DIRECTORY_NAME/api
cp -r src/api/*.h $DIRECTORY_NAME/api

scons --release --package -c
scons --release --package
if [ "$?" != 0 ]; then
    exit 1
fi

# scons --x86 --release --package -c
# scons --x86 --release --package
# if [ "$?" != 0 ]; then
#     exit 1
# fi

scons --package -c
scons --package
if [ "$?" != 0 ]; then
    exit 1
fi

# scons --x86 --package -c
# scons --x86 --package
# if [ "$?" != 0 ]; then
#     exit 1
# fi

rm -rf unittest_results
mkdir unittest_results

# export GTEST_OUTPUT=xml:unittest_results/debug_i386_results.xml
# ./bins/debug/i386/unittest/mip_upe_sdk_unittest

export GTEST_OUTPUT=xml:unittest_results/debug_x86_64_results.xml
export LD_LIBRARY_PATH=./bins/debug/x86_64/
./bins/debug/x86_64/unittest/mip_upe_sdk_unittest

# export GTEST_OUTPUT=xml:unittest_results/release_i386_results.xml
# ./bins/release/i386/unittest/mip_upe_sdk_unittest

export GTEST_OUTPUT=xml:unittest_results/release_x86_64_results.xml
export LD_LIBRARY_PATH=./bins/release/x86_64/
./bins/release/x86_64/unittest/mip_upe_sdk_unittest