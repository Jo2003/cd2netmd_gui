#!/bin/bash

#-----------------------------------------------------------------------
# Finally I found the time (and the need) to create a more or less
# good loking deb package!
# (c)2022 By Jo2003 All rights reserved!
#-----------------------------------------------------------------------
BIN_FILE=${1}
BIN_NAME=netmd_wizard
PACKAGE=$(echo -n ${BIN_NAME} | sed 's/_/-/')
OFF_NAME="NetMD Wizard"
VER="$(sed -n 's/.*PROGRAM_VERSION[^0-9]\+\([^\"]\+\).*/\1/p' defines.h)-$(date +%Y%m%d)"
ARCH="amd64"
QTVER="5"
BUILD_FOLDER="$(pwd)/packages/${BIN_NAME}_${VER}_${ARCH}"

create_folders() {
    rm -rf ${BUILD_FOLDER}
    mkdir -p "${BUILD_FOLDER}/etc/udev/rules.d"
    mkdir -p "${BUILD_FOLDER}/usr/share/hal/fdi/information/20thirdparty"
    mkdir -p "${BUILD_FOLDER}/usr/bin"
    mkdir -p "${BUILD_FOLDER}/usr/share/${BIN_NAME}/language"
    # mkdir -p "${BUILD_FOLDER}/usr/share/${BIN_NAME}/modules"
    # mkdir -p "${BUILD_FOLDER}/usr/share/${BIN_NAME}/doc"
    mkdir -p "${BUILD_FOLDER}/usr/share/${BIN_NAME}/resources/help"
    mkdir -p "${BUILD_FOLDER}/usr/share/doc/${PACKAGE}"
    mkdir -p "${BUILD_FOLDER}/usr/share/man/man7"
    mkdir -p "${BUILD_FOLDER}/usr/share/applications"
    mkdir -p "${BUILD_FOLDER}/DEBIAN/software"
}

copy_content() {
    strip -s ${BIN_FILE}
    cp -f ${BIN_FILE} "${BUILD_FOLDER}/usr/bin/"
    cp -f prebuilt/linux/bin/atracdenc "${BUILD_FOLDER}/usr/bin/"
    cp -f ffmpeg/linux/c2nffmpeg "${BUILD_FOLDER}/usr/bin/"
    cp -f help/*.png help/*.html "${BUILD_FOLDER}/usr/share/${BIN_NAME}/resources/help/"
    cp -f prebuilt/linux/config/netmd.rules "${BUILD_FOLDER}/etc/udev/rules.d/"
    cp -f prebuilt/linux/config/20-netmd.fdi "${BUILD_FOLDER}/usr/share/hal/fdi/information/20thirdparty/"
    # cp -f *.qm "${BUILD_FOLDER}/usr/share/${BIN_NAME}/language/"
    cp -f res/netmd_wizard.png "${BUILD_FOLDER}/usr/share/${BIN_NAME}/${BIN_NAME}.png"
}

create_desktop_file() {
    cat << EOF > "${BUILD_FOLDER}/usr/share/applications/${OFF_NAME}.desktop"
[Desktop Entry]
Name=${OFF_NAME}
Comment=A tool to transfer CD Audio to NetMD.
Exec=/usr/bin/${BIN_NAME}
Terminal=false
Type=Application
Icon=/usr/share/${BIN_NAME}/${BIN_NAME}.png
Categories=Audio;Recorder

EOF
}

create_control_file() {
    cat << EOF > "${BUILD_FOLDER}/DEBIAN/control"
Package: ${PACKAGE}
Version: ${VER}
Section: audio
Priority: extra
Architecture: ${ARCH}
Installed-Size: $(($(du -b --max-depth=0 ${BUILD_FOLDER}/usr|gawk '{print $1}') / 1024))
EOF

    cat << EOF >> "${BUILD_FOLDER}/DEBIAN/control"
Depends: libusb-1.0-0 (>= 2:1.0.21), libsndfile1 (>= 1.0.28), libflac8 (>= 1.3.2), libogg0 (>= 1.3.2), libvorbis0a (>= 1.3.5), libvorbisenc2 (>= 1.3.5), libgcrypt20 (>= 1.8.1), libqt5widgets5 (>= 5.9.5), libqt5gui5 (>= 5.9.5), libqt5network5 (>= 5.9.5), libqt5core5a (>= 5.9.5), libgpg-error0 (>= 1.27)
EOF


cat << EOF >> "${BUILD_FOLDER}/DEBIAN/control"
Maintainer: Jo2003 <olenka.joerg@gmail.com>
Description: A tool like NetMD Simple Burner.
 It extracts CD Audio from CD and encodes / transfers
 the audio data to your NetMD device.
Homepage: https://github.com/Jo2003/cd2netmd_gui

EOF
}

create_changelog() {
    cat << EOF | gzip -9 -c >"${BUILD_FOLDER}/usr/share/doc/${PACKAGE}/changelog.Debian.gz"
${PACKAGE} (${VER}) bionic; urgency=minor

  * New release, makes us really happy!

 -- Jo2003 <olenka.joerg@gmail.com>  $(date -R)
EOF
    git log --decorate=full > "${BUILD_FOLDER}/usr/share/doc/${PACKAGE}/changelog"
    gzip -9 "${BUILD_FOLDER}/usr/share/doc/${PACKAGE}/changelog"
}

create_postinst() {
    cat << EOF > "${BUILD_FOLDER}/DEBIAN/postinst"
#!/bin/sh
udevadm control --reload-rules
udevadm trigger

EOF
}

create_deb() {
    dpkg-deb --build "${BUILD_FOLDER}"
}

create_man_page() {
    cat << EOF | gzip -9 -c > "${BUILD_FOLDER}/usr/share/man/man7/${BIN_NAME}.7.gz"
.\" Manpage for ${BIN_NAME}.
.\" Contact olenka.joerg@gmail.com to correct errors or typos.
.TH man 7 "$(date -R)" "1.0" "${BIN_NAME} man page"
.SH NAME
${BIN_NAME} \- starts ${OFF_NAME} in GUI mode
.SH SYNOPSIS
${BIN_NAME}
.SH DESCRIPTION
For me it doesn't look like this program needs a man page. Nevertheless lintian wants one - so here it is. If you need help have a look at the github page https://github.com/Jo2003/cd2netmd_gui.
.SH OPTIONS
No options so far.
.SH BUGS
There are for sure bugs. If you find one please contact the author!
.SH AUTHOR
Jo2003 (olenka.joerg@gmail.com)
EOF
}

create_copyright_file() {
    cat << EOF > "${BUILD_FOLDER}/usr/share/doc/${PACKAGE}/copyright"
${OFF_NAME}
Copyright 2021-$(date +%Y) Jo2003 (olenka.joerg@gmail.com)
All rights reserved!
This program uses atracdenc (https://github.com/dcherednik/atracdenc) for encoding 
and the Qt framework (c) by Trolltech, Nokia, Digia, Qt-Project, qt.io, who knows ...
${OFF_NAME} is released under the GPL 3.
A copy of this license can be found here: /usr/share/common-licenses/GPL-3 .

For Olenka!

EOF
}

create_folders
copy_content
create_desktop_file
create_changelog
create_man_page
create_copyright_file
create_control_file
create_postinst
create_deb
