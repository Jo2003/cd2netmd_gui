# NetMD Wizard
A tool like Sonys NetMD Simple Burner to transfer audio from CD to NetMD. It is written in C++ using the Qt framework.
It supports Windows 7 64bit and newer, MacOS 10.13 and newer, and Ubuntu (and derivates) 18.04 and newer.

![cd2netmd_gui](https://github.com/Jo2003/cd2netmd_gui/releases/download/v.0.1.7.5/cd2netmd_gui_dark.png)

## Features ##
- reads CD using libcdio
- supports CDDA paranoia
- reads __Cue Sheets__
- reads CD-TEXT
- requests track title from CDDB (gnudb.org)
- supports on-the-fly MDLP encoding on SHARP IM-DR4x0, Sony MDS-JB980, and Sony MDS-JE780
- support external MDLP encoding for other NetMD devices
- supports track-, group-, and disc rename / erase on NetMD (context menu)
- supports simple group management on NetMD (context menu)
- __supports gapless audio__ in LP2 mode using external encoder and disc-at-once transfer.


## Usage ##
- For Windows only make sure you have installed WebUSB driver through [Zadig](https://zadig.akeo.ie/) tool.
- Start the program.
- Place a CD-Audio in your CD-Drive, place a NetMD in your NetMD drive and press button (Re-)load CD and (Re-)load NetMD.
- Instead loading a CD you can also browse for a Cue Sheet file.
- When both discs are displayed, chose tracks from CD, set the transfer settings to your needs and press the Transfer button.
- In case you want gapless audio (e.g. for live CD or project records), press button DAO Transfer.
- DAO Transfer will rip the CD into one track, encode the audio through external encoder and transfer the split encode to NetMD.
- DAO also works for Cue Sheets.
- Track management on NetMD is done through context menu. 

## Projects used / Thanks to ##
- [atracdenc](https://github.com/dcherednik/atracdenc) - the external ATRAC encoder
- [JSON for Modern C++](https://github.com/nlohmann/json)
- [Qt](https://qt.io) - One framework to rule them all.
- [webminidisc](https://github.com/cybercase/webminidisc) for inspiration
- [PLATINUM-MD](https://github.com/gavinbenda/platinum-md) for a good start with linux-minidisc
- [MSYS2](https://www.msys2.org/) for the Windows build environment of choice
- [libcue](https://github.com/Jo2003/libcue) for making Cue Sheet parsing a bit easier.
- [libcdio](https://www.gnu.org/software/libcdio/) for reading CDs on multi platform.
- [taglib](https://taglib.org/) for make reading tags much easier.
- [ffmpeg](https://www.ffmpeg.org/) to encode _xxx_ to compatible wav files. 

## Support me ##
In case you find this tool useful, you may consider to [buy me a beer](https://paypal.me/Jo2003).
