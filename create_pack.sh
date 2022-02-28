#!/bin/sh
rm -rf install
mkdir install
cp release/netmd_wizard.exe "install/NetMD Wizard.exe"
windeployqt install/QCD2NetMD.exe
deps=$(ldd install/QCD2NetMD.exe | gawk '/mingw64/ {print $3}'); for d in $deps ; do cp -v $d install/ ; done
cp -rv toolchain install/
cp -v openssl/* install/
cp -v ffmpeg/windows/ffmpeg.exe install/toolchain/

