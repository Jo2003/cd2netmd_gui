# cd2netmd_gui
A tool like Sonys NetMD Simple Burner to transfer audio from CD to NetMD. It is written in C++ using the Qt framework.
Right now it supports Windows 7 64bit and newer. Mac and Linux versions are maybe created later.

![cd2netmd_gui](https://github.com/Jo2003/cd2netmd_gui/releases/download/v0.1.5/CD.to.NetMD.transfer.png)

## Features ##
- reads CD using libcdio
- supports CDDA paranoia
- reads CD-TEXT
- requests track title from CDDB (gnudb.org)
- supports on-the-fly MDLP encoding on SHARP IM-DR4x0, Sony MDS-JB980, and Sony MDS-JE780
- support external MDLP encoding for other NetMD devices
- supports track-, group-, and disc rename / erase on NetMD
- supports simple group management on NetMD
- __supports gapless audio__ in LP2 mode using external encoder and disc-at-once transfer.
- It is a portable app. No installation needed. Extract it where you like and start it.

## Usage ##
- Make sure you have installed WebUSB driver through [Zadig](https://zadig.akeo.ie/) tool.
- Start the program by executing the file CD2NetMDGui\QCD2NetMD.exe .
- Place a CD-Audio in your CD-Drive, place a NetMD in your NetMD drive and press button (Re-)load CD and (Re-)load NetMD.
- When both discs are displayed, chose tracks from CD, set the transfer settings to your needs and press the Transfer button.
- In case you want gapless audio (e.g. for live CD or project records), press button DAO Transfer.
- DAO Transfer will rip the CD into one track, encode the audio through external encoder and transfer the split encode to NetMD.
- Track management on NetMD is done through context menu. 

## Thanks to ##
- [atracdenc](https://github.com/dcherednik/atracdenc) - the external ATRAC encoder
- [JSON for Modern C++](https://github.com/nlohmann/json)
- [Qt](https://qt.io) for a great framework
- [webminidisc](https://github.com/cybercase/webminidisc) for inspiration
- [PLATINUM-MD](https://github.com/gavinbenda/platinum-md) for a good start with linux-minidisc
- [MSYS2](https://www.msys2.org/) for the Windows build environment of choice

## Support me ##
In case you find this tool useful, you may consider to [buy me a beer](https://paypal.me/Jo2003).
