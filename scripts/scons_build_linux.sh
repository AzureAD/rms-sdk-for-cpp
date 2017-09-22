#!/bin/sh
DESTINATION=$1

declare -a ARCHS=(x86_64)
declare -a CONFIGS=(debug release)

rm -rf unittest_results
mkdir unittest_results

for arch in ${ARCHS[@]};do
  for config in ${CONFIGS[@]};do
    mkdir -p $DESTINATION/"$config"/"$arch"
    echo scons -j 4 --configuration="$config" --package
    scons -j 4 --configuration="$config" --package -c
    scons -j 4 --configuration="$config" --package
    if [ $? -ne 0 ]; then
      echo "scons sdk make failed."
      exit 1
    fi
    export PATH=${HOME}/Qt/5.7/gcc_64/bin:$PATH$
    export LD_LIBRARY_PATH=bin/"$config"/"$arch"

    bin/"$config"/"$arch"/sdk/rmscrypto_sdk/unittests -xunitxml > unittest_results/"$config"_"$arch"_utResults.txt
    bin/"$config"/"$arch"/sdk/rms_sdk/platform_ut -xunitxml >> unittest_results/"$config"_"$arch"\_utResults.txt
    bin/"$config"/"$arch"/sdk/rms_sdk/profile_ut -xunitxml >> unittest_results/"$config"_"$arch"_utResults.txt
    bin/"$config"/"$arch"/sdk/rms_sdk/rest_clients_ut -xunitxml >> unittest_results/"$config"_"$arch"_utResults.txt
    bin/"$config"/"$arch"/sdk/file_sdk/file_sdk_unittest -xunitxml >> unittest_results/"$config"_"$arch"_utResults.txt

    powershell scripts/ut_parser.ps1 unittest_results/"$config"_"$arch"_utResults

    cp -r bin/"$config"/"$arch"/sdk/*.so $DESTINATION/"$config"/"$arch"/.
    done
done