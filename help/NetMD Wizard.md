# NetMD Wizard #

NetMD Wizard is a program to write audio data to your NetMD device. 

![View](complete_view.png)

## Features ##

- reads CD Audio through libcdio with optional CD Paranoia support
- reads CD-Text and requests CD information from CDDB (gnudb.org)
- ships with *atracdenc*, supports the other, *well known*, alternate ATRAC3 encoder
- supports on-the-fly LP encoding on supporting devices
- supports gap-less audio transfer in LP mode (using external encoder)
- supports gap-less audio transfer in SP mode on supporting devices
- loads Cue Sheets and handles them as CD Audio
- supports drag and drop of audio files
- supports sorting and naming in the source widget
- supports naming and grouping in the MD tree widget
- binaries are available for Windows, Mac, and Linux

## Usage ##

> Note: Before using this tool under Windows you have to install the WebUSB library. This can be done using the [zadig tool](https://zadig.akeo.ie/).

Hopefully usage doesn't need much instruction. In a nutshell:

1. Import Audio Data

2. Load the Minidisc

3. Choose the transfer mode

   1. The audio track symbol stands for Track at once (normal mode)
   2. The disc symbol stands for Disc at once (DAO / gapless mode)

    ![Transfer Modes](transfer_modi.png)

4. Press the button "Transfer".

### Import Audio Data ###

You have various possibilities to import audio data into the program:

- CD & Cue Sheet (through button 'Reload CD', 'Load CUE Sheet'):
![Buttons](load_buttons.png)
- Drag and Drop Audio Files: 
![Drag 'n' Drop](dnd.png)

In Cue Sheet as well through drag and drop following audio file types are supported:

| Audio File                                     | File Extension |
| ---------------------------------------------- | -------------- |
| WAVE (Raw PCM data with wave header)           | wav            |
| OGG Vorbis                                     | ogg            |
| Monkey Audio compressed WAVE files.            | ape            |
| Mpeg Audio Layer 3                             | mp3            |
| Advanced Audio Codec and Apple lossless (alac) | m4a, mp4       |
| Free Lossless Audio Codec                      | flac           |
| Atrac 1 (SP) audio                             | aea            |

You can use the audio import windows to remove unwanted tracks (delete key), sort tracks via drag and drop, or rename the tracks:

 ![Sorting](sorting.png)

In Cue Sheet mode and when using a CD Audio, selected tracks will be transferred. If no track is selected, all tracks will be transferred. 

> Note: If you have inserted audio files through drag and drop all files will be transferred to MD - I assume that you only drop wanted audio files to the program. 

### (D)isc (A)t (O)nce or Gapless Mode

DAO / gapless is supported in 4 modes. All have there pros and cons.

#### DAO LP2 / LP4 Mode 

The audio content will be extracted and compressed at once. After that, the audio will be split into tracks and transferred to your NetMD device. You have to expect quality loss due to LP2/LP4 mode and the usage of an external encoder. Playback is only supported on MDLP capable devices.

#### DAO SP Mode 

The audio content will be extracted and transferred to the NetMD device at once. After that the audio data will be split directly on the NetMD device through TOC edit. This gives you the best possible quality. Playback is supported on all MD devices. Unfortunately, this is only supported on Sony / Aiwa portable type R, and type S devices.

#### DAO SP Pre-Enc Mode 

The whole audio content will be extracted and converted into Atrac 1, and transferred to the NetMD device at once. After that the audio data will be split directly on the NetMD device through TOC edit. Playback is supported on all MD devices. Unfortunately, this is only supported on Sony portable type S devices. This mode is much faster than the other SP modes, but quality depends on atracdenc and might be slightly worse.

#### DAO SP Mono Mode 

The whole audio content will be extracted and transferred to the NetMD device at once. The NetMd device converts the audio data into SP mono (needing half the data size). After that the audio data will be split directly on the NetMD device through TOC edit. Playback is supported on all MD devices. Unfortunately, this is only supported on Sony portable type R - and S devices.

<div style='color:red; background-color: #fff6d1; border: red solid 2px; padding: 5px; margin: 5px;'>For all DAO SP modi I'd recommend the usage of a blank MD. While we take care for existing content, you might end up with issues on very fragmented discs. Furthermore, take care that there is no pending TOC edit on your NetMD device before starting the DAO upload. Simply press 'stop' on your device <b>before going on!</b></div>

> Note: In DAO mode when reading from CD all sorting and deleting from tracks done in the source window will be **reverted** before the transfer starts.

## Settings ##

 ![Settings](settings.png)

- __CD Read Config:__ You can enable CDDA paranoia mode here. In some cases this might give you better audio quality when reading from CD. This drastically slows down the CD read speed.
- __Transfer Config:__ In case your NetMD device support on-the-fly LP encoding this will be enabled by default. This gives better audio quality in LP mode then using the external encoder. Currently there are only 3 NetMD devices known which support on-the-fly encoding: Sony MDS-JB980, Sony MDS-JE780, Sharp IM-DR420H.
- __CDDB:__ Enable / disable the CDDB request for CD Audio information.
- __MD Track Grouping:__ If enabled, a title group for a complete MD transfer is created (after LP transfer).
- __MD Title:__ If enabled, the MD will be named after the disc from last SP audio transfer.
- **Size Check:** Enabling this will *disable* the audio length check. So, you will not be warned if the audio you want to copy doesn't fit on your MD. This setting might be useful if the available disc space is only a bit to small. Try it at your own risk. Your transfer might fail. This setting isn't stored. 
- **Device Reset:** If enabled, your NetMD device will be reset after completing the TOC edit (SP DAO transfer). If you don't enable this, you have to re-insert the MD to your device after the TOC edit is done. 
- __Theme:__ Use the theme you like most.
- __Log Level:__ Changes the detail grade of logging. 
- __Del. temp. files:__ Delete temporary files created be the program. Normally these files will be deleted before the program closes. In case of a program crash (sorry!) these files might persist in your temp folder. Pressing this button will delete these files.
- __Log File:__ Opens the log file. Useful for debugging or when I ask you for some more information. 

## MD Context Menu

 ![Context Menu](context_menu.png)

From the MDs context menu you can:

* add audio tracks to a group
* delete a group
* delete a track
* erase the MD
* export the title list as text file



## Related Projects ##

- [atracdenc](https://github.com/dcherednik/atracdenc) - the external ATRAC encoder
- [JSON for Modern C++](https://github.com/nlohmann/json)
- [Qt](https://qt.io) - One framework to rule them all.
- [webminidisc](https://github.com/cybercase/webminidisc) - the inspiration
- [PLATINUM-MD](https://github.com/gavinbenda/platinum-md) - a good start with linux-minidisc
- [MSYS2](https://www.msys2.org/) - the Windows build environment of choice
- [libcdio](https://www.gnu.org/software/libcdio/) - reading CDs on multi platform.
- [taglib](https://taglib.org/) - makes reading tags much easier.
- [ffmpeg](https://www.ffmpeg.org/) - encodes _xxx_ to compatible wav files. 
- [NetMdTool](https://github.com/cadavrezzz/NetMDTool) - the source of some nice ideas

## Thanks to... 

* [**asivery**](https://www.reddit.com/user/asivery/) for his great support with the Sony firmware patching.
* **donlazlo** - my first tester which even paid me one (or more) beer.
* [**Sir68k**](https://www.reddit.com/u/Sir68k/) for discovering the Sony firmware exploit.
* **Ozzey** for his code reviews.
* [**DaveFlash**](https://www.reddit.com/user/DaveFlash) for the nice program icon and some good ideas.

## Support ##

In case you find a bug (yes, there are some), please file a bug report on the project page: [GitHub](https://github.com/Jo2003/cd2netmd_gui).

## Support me ##
In case you find this tool useful, you may consider to [buy me a beer](https://paypal.me/Jo2003).

