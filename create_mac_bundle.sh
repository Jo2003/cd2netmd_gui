#!/bin/bash
APPNAME=netmd_wizard
BUILDFOLDER=release

[[ ! -z "${1}" ]] && BUILDFOLDER=${1}

TMPFILE=/tmp/plist.tmp
CONTENTS=${APPNAME}.app/Contents
OFFNAME="NetMD Wizard"
QTTRANS=/Users/joergn/Qt5.12.12/5.12.12/clang_64/translations
echo "Creating APP bundle for ${APPNAME} ..."

echo "Official name is ${OFFNAME} ..."
VER="$(sed -n 's/.*GIT_VERSION[^0-9]*\([0-9.a-z-]*\).*/\1/p' git_version.h)"

# create destination folder name ...
DSTFOLDER="${APPNAME}_mac_${VER}"

echo "Destination folder is ${DSTFOLDER} ..."

cd ${BUILDFOLDER}
rm -rf ${APPNAME}_mac*
mkdir -p "${CONTENTS}/Resources/language"
mkdir -p "${CONTENTS}/Resources/help"
mkdir -p "${CONTENTS}/PlugIns"
mkdir -p "${CONTENTS}/Frameworks"
mkdir -p "${CONTENTS}/translations"
cp "../res/minidisc.icns" "${CONTENTS}/Resources/${APPNAME}.icns"
cp ../prebuilt/mac/bin/atracdenc "${CONTENTS}/MacOS/"
cp ../ffmpeg/mac/ffmpeg "${CONTENTS}/MacOS/"
cp ../prebuilt/mac/lib/*.dylib "${CONTENTS}/Frameworks/"
cp ../help/*.png ../help/*.html "${CONTENTS}/Resources/help/"

# copy Qt translations
cp "${QTTRANS}/qt_de.qm" "${CONTENTS}/translations"
cp "${QTTRANS}/qt_fr.qm" "${CONTENTS}/translations"
cp "${QTTRANS}/qt_pl.qm" "${CONTENTS}/translations"
cp "${QTTRANS}/qt_ru.qm" "${CONTENTS}/translations"

# create Info.plist file ...
cat << EOF > ${TMPFILE}
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist SYSTEM "file://localhost/System/Library/DTDs/PropertyList.dtd">
<plist version="0.9">
	<dict>
		<key>CFBundleIconFile</key>
		<string>$APPNAME.icns</string>
		<key>CFBundlePackageType</key>
		<string>APPL</string>
		<key>CFBundleGetInfoString</key>
		<string>Created by Qt/QMake</string>
		<key>CFBundleSignature</key>
		<string>????</string>
		<key>CFBundleExecutable</key>
		<string>$APPNAME</string>
		<key>CFBundleIdentifier</key>
		<string>com.Jo2003.$APPNAME</string>
		<key>CFBundleName</key>
		<string>$OFFNAME</string>
		<key>NSRequiresAquaSystemAppearance</key>
		<string>true</string>
		<key>NSPrincipalClass</key>
		<string>NSApplication</string>
		<key>NSHighResolutionCapable</key>
		<string>True</string>
		<key>LSMinimumSystemVersion</key>
		<string>10.13</string>
	</dict>
</plist>
EOF

iconv -f ASCII -t UTF-8 ${TMPFILE} >"${CONTENTS}/Info.plist"
macdeployqt "${APPNAME}.app" -verbose=0
# install_name_tool -add_rpath @executable_path/../Framework "${APPNAME}.app/Contents/MacOS/atracdenc"
# install_name_tool -add_rpath @executable_path/plugins "${APPNAME}.app/Contents/MacOS/${APPNAME}"
# install_name_tool -change libjson-c.5.dylib @executable_path/../Frameworks/libjson-c.5.dylib "${APPNAME}.app/Contents/MacOS/${APPNAME}"
# install_name_tool -id @executable_path/../Frameworks/libjson-c.5.dylib "${APPNAME}.app/Contents/Frameworks/libjson-c.5.dylib"
# install_name_tool -id @executable_path/../Frameworks/libcue.2.dylib "${APPNAME}.app/Contents/Frameworks/libcue.2.dylib"
install_name_tool -change /usr/local/lib/libsndfile.1.dylib @executable_path/../Frameworks/libsndfile.1.dylib "${APPNAME}.app/Contents/MacOS/atracdenc"
# install_name_tool -change /usr/local/opt/libogg/lib/libogg.0.dylib @executable_path/../Frameworks/libogg.0.dylib "${APPNAME}.app/Contents/MacOS/flac"
# install_name_tool -change /usr/local/Cellar/flac/1.3.3/lib/libFLAC.8.dylib @executable_path/../Frameworks/libFLAC.8.dylib "${APPNAME}.app/Contents/MacOS/flac"

POS=`pwd`
cd "${CONTENTS}/Frameworks"
rm -rf QtDeclarative.framework
rm -rf QtScript.framework
rm -rf QtSvg.framework
rm -rf QtXmlPatterns.framework
cd "${POS}"

if [ "${BUILDFOLDER}" = "release" ] ; then
	mkdir -p "${DSTFOLDER}"
    mv "${APPNAME}.app" "${DSTFOLDER}/${OFFNAME}.app"
    ln -s /Applications "${DSTFOLDER}/"
fi

cd ..
