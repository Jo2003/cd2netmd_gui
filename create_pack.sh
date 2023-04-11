#!/bin/sh
rm -rf install
mkdir -p install/help
cp -v release/netmd_wizard.exe "install/NetMD Wizard.exe"
cp -v help/*.png help/*.html install/help/
windeployqt "install/NetMD Wizard.exe"
cp -v prebuilt/windows/lib/libnetmd++.dll install/
deps=$(ldd "install/NetMD Wizard.exe" | gawk '/mingw64/ {print $3}'); for d in $deps ; do cp -v $d install/ ; done
cp -rv toolchain install/
cp -v openssl/* install/
cp -v ffmpeg/windows/ffmpeg.exe install/toolchain/
