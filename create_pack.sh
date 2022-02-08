#!/bin/sh
rm -rf install
mkdir install
cp release/cd2netmd_gui.exe install/QCD2NetMD.exe
windeployqt install/QCD2NetMD.exe
deps=$(ldd install/QCD2NetMD.exe | gawk '/mingw64/ {print $3}'); for d in $deps ; do cp -v $d install/ ; done
cp -rv toolchain install/
cp -v openssl/* install/

