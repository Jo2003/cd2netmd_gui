#!/bin/sh
rm -rf install
mkdir install
cp release/QCD2NetMD.exe install/
windeployqt install/QCD2NetMD.exe
deps=$(ldd install/QCD2NetMD.exe | gawk '/mingw64/ {print $3}'); for d in $deps ; do cp -v $d install/ ; done
cp -rv toolchain install/
