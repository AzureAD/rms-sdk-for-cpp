#!/bin/bash

mkdir dist
cp -r ./deb/DEBIAN/ ./dist/
cp -r ./deb/etc ./dist/
cd dist

md5deep -r -l usr > DEBIAN/md5sum

find ./ -type d -exec chmod 755 {} \;
find ./ -type f -exec chmod 644 {} \;
find ./ -iname '*.so*' -type f -exec chmod +x {} \;
chmod +x ./DEBIAN/postinst
chmod +x ./DEBIAN/postrm

cd ..
fakeroot dpkg-deb --build dist
mv -f dist.deb rms-sdk_1.0_x64.deb
