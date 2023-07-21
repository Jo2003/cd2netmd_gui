/*
 * netmd++.h
 *
 * This file is part of netmd++, a library for accessing NetMD devices.
 *
 * It makes use of knowledge / code collected by Marc Britten and
 * Alexander Sulfrian for the Linux Minidisc project.
 *
 * Asivery helped to make this possible!
 * Sir68k discovered the Sony FW exploit!
 *
 * Copyright (C) 2023 Jo2003 (olenka.joerg@gmail.com)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

/**
@file netmd++.h
@mainpage
# netmd++
This C++ API was written to ease the handling of NetMD devices. It is a synchronous API.
So, function calls might block your program flow. If you want to use this API in an GUI app,
better put the API calls into a background thread.

## Supported Devices
|Manufacturer | Manufacturer ID | Device ID | Name                      | Type  |
| ----------- | --------------- | --------- | ------------------------- | ----- |
|**Sony**     | 0x054c          | 0x0034    | Sony PCLK-XX              | NetMD |
|             | 0x054c          | 0x0036    | Sony NetMD Walkman        | NetMD |
|             | 0x054c          | 0x006F    | Sony NW-E7                | NetMD |
|             | 0x054c          | 0x0075    | Sony MZ-N1                | NetMD |
|             | 0x054c          | 0x007c    | Sony NetMD Walkman        | NetMD |
|             | 0x054c          | 0x0080    | Sony LAM-1                | NetMD |
|             | 0x054c          | 0x0081    | Sony MDS-JE780/JB980      | NetMD |
|             | 0x054c          | 0x0084    | Sony MZ-N505              | NetMD |
|             | 0x054c          | 0x0085    | Sony MZ-S1                | NetMD |
|             | 0x054c          | 0x0086    | Sony MZ-N707              | NetMD |
|             | 0x054c          | 0x008e    | Sony CMT-C7NT             | NetMD |
|             | 0x054c          | 0x0097    | Sony PCGA-MDN1            | NetMD |
|             | 0x054c          | 0x00ad    | Sony CMT-L7HD             | NetMD |
|             | 0x054c          | 0x00c6    | Sony MZ-N10               | NetMD |
|             | 0x054c          | 0x00c7    | Sony MZ-N910              | NetMD |
|             | 0x054c          | 0x00c8    | Sony MZ-N710/NE810/NF810  | NetMD |
|             | 0x054c          | 0x00c9    | Sony MZ-N510/NF610        | NetMD |
|             | 0x054c          | 0x00ca    | Sony MZ-NE410/DN430/NF520 | NetMD |
|             | 0x054c          | 0x00e7    | Sony CMT-M333NT/M373NT    | NetMD |
|             | 0x054c          | 0x00eb    | Sony MZ-NE810/NE910       | NetMD |
|             | 0x054c          | 0x0101    | Sony LAM                  | NetMD |
|**Aiwa**     | 0x054c          | 0x0113    | Aiwa AM-NX1               | NetMD |
|**Sony**     | 0x054c          | 0x011a    | Sony CMT-SE7              | NetMD |
|             | 0x054c          | 0x0119    | Sony CMT-SE9              | NetMD |
|             | 0x054c          | 0x013f    | Sony MDS-S500             | NetMD |
|             | 0x054c          | 0x0148    | Sony MDS-A1               | NetMD |
|**Aiwa**     | 0x054c          | 0x014c    | Aiwa AM-NX9               | NetMD |
|**Sony**     | 0x054c          | 0x017e    | Sony MZ-NH1               | HiMD  |
|             | 0x054c          | 0x0180    | Sony MZ-NH3D              | HiMD  |
|             | 0x054c          | 0x0182    | Sony MZ-NH900             | HiMD  |
|             | 0x054c          | 0x0184    | Sony MZ-NH700/800         | HiMD  |
|             | 0x054c          | 0x0186    | Sony MZ-NH600             | HiMD  |
|             | 0x054c          | 0x0187    | Sony MZ-NH600D            | HiMD  |
|             | 0x054c          | 0x0188    | Sony MZ-N920              | NetMD |
|             | 0x054c          | 0x018a    | Sony LAM-3                | NetMD |
|             | 0x054c          | 0x01e9    | Sony MZ-DH10P             | HiMD  |
|             | 0x054c          | 0x0219    | Sony MZ-RH10              | HiMD  |
|             | 0x054c          | 0x021b    | Sony MZ-RH910             | HiMD  |
|             | 0x054c          | 0x021d    | Sony CMT-AH10             | HiMD  |
|             | 0x054c          | 0x022c    | Sony CMT-AH10             | HiMD  |
|             | 0x054c          | 0x023c    | Sony DS-HMD1              | HiMD  |
|             | 0x054c          | 0x0286    | Sony MZ-RH1               | HiMD  |
|**Sharp**    | 0x04dd          | 0x7202    | Sharp IM-MT880H/MT899H    | NetMD |
|             | 0x04dd          | 0x9013    | Sharp IM-DR400/DR410      | NetMD |
|             | 0x04dd          | 0x9014    | Sharp IM-DR80/DR420/DR580 | NetMD |
|**Panasonic**| 0x04da          | 0x23b3    | Panasonic SJ-MR250        | NetMD |
|             | 0x04da          | 0x23b6    | Panasonic SJ-MR270        | NetMD |
|**Kenwood**  | 0x0b28          | 0x1004    | Kenwood MDX-J9            | NetMD |


## Namespace
This API uses the namespace *netmd*.

## Usage

 - include the header file into your project:
~~~
#include "path/to/netmd++.h"
~~~

 - create an instance of the API:
~~~
netmd::netmd_pp* pNetMd = new netmd::netmd_pp();
~~~

 - initialize the first found NetMD device:
~~~
if (pNetMd != nullptr)
{
    pNetMd->initDevice();
}
~~~

 - If you change or re-plug the device, simply run above code (initDevice()) again!

## Examples
### Track transfer
Check for on-the-fly support and transfer a WAVE file to NetMD with on-the-fly encoding (LP2) or w/o encoding (SP).

~~~
#include <netmd++.h>

int main()
{
    netmd::netmd_pp* pNetMd = new netmd::netmd_pp();

    if ((pNetMd != nullptr) && (pNetMd->initDevice() == netmd::NETMDERR_NO_ERROR))
    {
        if (pNetMd->otfEncodeSupported())
        {
            pNetMd->sendAudioFile("/path/to/nice/audio.wav", "Very nice Audio file (LP2)", netmd::NETMD_DISKFORMAT_LP2);
        }
        else
        {
            pNetMd->sendAudioFile("/path/to/nice/audio.wav", "Very nice Audio file (SP)", netmd::NO_ONTHEFLY_CONVERSION);
        }
    }
    return 0;
}
~~~

### Erase disc and set new title
~~~
#include <netmd++.h>

int main()
{
    netmd::netmd_pp* pNetMd = new netmd::netmd_pp();

    if ((pNetMd != nullptr) && (pNetMd->initDevice() == netmd::NETMDERR_NO_ERROR))
    {
        pNetMd->eraseDisc();
        pNetMd->setDiscTitle("Amazing MD");

    }
    return 0;
}
~~~

# MDs UTOC
For the UTOC structure please have a look at this great site on [minidisc.org](https://www.minidisc.org/md_toc.html)

## Addressing in UTOC
The disc start and end addresses each consist of a cluster, sector, and sound group, all packed into 3 bytes.
The smallest unit is a sound frame, representing 11.6ms of mono audio (212 bytes), while the smallest
<b>addressable</b> unit is the sound group, containing 2 sound frames. A sector contains 11
sound frames / 5.5 sound groups. Addressing must be done through sound group. Sound groups are numbered
0 ... 10. Sound groups 0 ... 5 are part of the even sector, while sound groups 5 ... 10 are part of the odd sector.
Group 5 overlaps both even and odd sectors and can therefore be addressed on both sectors.
<pre>
+-------------------------------------------+
|                sector pair                |
+---------------------+---------------------+
|   even sector (2n)  |  odd sector (2n+1)  |
+---+---+---+---+---+-+-+---+---+---+---+---+
| 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10| <- sound groups
+---+---+---+---+---+---+---+---+---+---+---+
</pre>
A cluster is an aggregate of 32 audio sectors (176 sound groups) representing 2.04 seconds of stereo audio; 
it is the smallest unit of data that can be written to a MiniDisc. In the 3 byte packing, there are 14 bits 
allocated to the cluster number, 6 bits to the sector, and 4 bits to the soundgroup; this arrangement allows 
addressing of up to 9.2 hours of stereo audio.

## Modifying the UTOC
 1. download the UTOC sectors 0 ... 2 from NetMD Device:
~~~
pNetMd->prepareTOCManip();
NetMDByteVector tocData;

for (int i = 0; i < 3; i++)
{
    tocData += pNetMd->readUTOCSector(static_cast<UTOCSector>(i));
}
~~~
 2. create toc class instance and add some track data
~~~
uint8_t *pData = new uint8_t[tocData.size()];
for(size_t i = 0; i < tocData.size(); i++)
{
    pData[i] = toc.at(i);
}
netmd::CNetMdTOC utoc(8, 459'000, pData);
utoc.addTrack(1, 60'000, "Funky Track One Minute Part #1");
utoc.addTrack(2, 60'000, "Funky Track One Minute Part #2");
~~~
 3. upload changed TOC data to NetMD
~~~
bool doit = true;
for (int x = 0; x < 3; x++)
{
    tocData.clear();
    addArrayData(tocData, &pData[2352 * x], 2352);
    if (pNetMD->writeUTOCSector(static_cast<UTOCSector>(x), tocData) == NETMDERR_NO_ERROR)
    {
        std::cout << "TOC sector " << x << " written!" << std::endl;
    }
    else
    {
        doit = false;
    }
}

if (doit)
{
    pNetMD->finalizeTOC();
}
delete [] pData;
~~~
*/
#pragma once
#include <cstdint>
#include <sstream>
#include <vector>

namespace netmd {

/// disk format
enum DiskFormat : uint8_t
{
    NETMD_DISKFORMAT_LP4       =   0,   ///< LP4
    NETMD_DISKFORMAT_LP2       =   2,   ///< LP2
    NETMD_DISKFORMAT_SP_MONO   =   4,   ///< SP mono
    NETMD_DISKFORMAT_SP_STEREO =   6,   ///< SP stereo
    NO_ONTHEFLY_CONVERSION     = 0xf    ///< dont do on-the-fly encoding
};

/// NetMD errors
enum NetMdErr : int
{
    NETMDERR_NO_ERROR      =  0,  ///< success
    NETMDERR_USB           = -1,  ///< general USB error
    NETMDERR_NOTREADY      = -2,  ///< player not ready for command
    NETMDERR_TIMEOUT       = -3,  ///< timeout while waiting for response
    NETMDERR_CMD_FAILED    = -4,  ///< minidisc responded with 08 response
    NETMDERR_CMD_INVALID   = -5,  ///< minidisc responded with 0A response
    NETMDERR_PARAM         = -6,  ///< parameter error
    NETMDERR_OTHER         = -7,  ///< any other error
    NETMDERR_NOT_SUPPORTED = -8,  ///< not supported
    NETMDERR_INTERIM       = -9,  ///< interim
    NETMDERR_AGAIN         = -10, ///< try again
};

/// track times
struct TrackTime
{
    int mMinutes;   ///< time in minutes
    int mSeconds;   ///< time in seconds
    int mTenthSecs; ///< time in 10ms
};

/// type safe protection flags
enum class TrackProtection : uint8_t
{
    UNPROTECTED = 0x00, //!< track is unprotected
    PROTECTED   = 0x03, //!< track  is protected
    UNKNOWN     = 0xFF  //!< unknown track state
};

/// type safe encoding flags
enum class AudioEncoding : uint8_t
{
    SP      = 0x90, //!< SP encoding
    LP2     = 0x92, //!< LP2 encoding
    LP4     = 0x93, //!< LP4 encoding
    UNKNOWN = 0xff  //!< unknown encoding
};

/// log severity
enum typelog
{
    DEBUG,      //!< debug information
    INFO,       //!< information
    WARN,       //!< more serious
    CRITICAL,   //!< critical information
    CAPTURE     //!< needed for log parcing!
};

/// TOC sector names
enum UTOCSector : uint16_t
{
    POS_ADDR,   //!< position and addresses of audio data
    HW_TITLES,  //!< half width titles
    TSTAMPS,    //!< time stamps
    FW_TITLES,  //!< full width titles
    UNKNWN_1,   //!< some unidentified TOC sector #1
    UNKNON_2,   //!< some unidentified TOC sector #2
};

/// NetMD time
struct NetMdTime
{
    uint16_t hour;      //!< hour
    uint8_t  minute;    //!< minute
    uint8_t  second;    //!< second
    uint8_t  frame;     //!< frame
};

/// Structure to hold the capacity information of a disc.
struct DiscCapacity
{
    /// Time allready recorded on the disc.
    NetMdTime recorded;

    /// Total time, that could be recorded on the disc. This depends on the
    /// current recording settings.
    NetMdTime total;

    /// Time that is available on the disc. This depends on the current
    /// recording settings.
    NetMdTime available;
};

//-----------------------------------------------------------------------------
//! @brief      track group
//-----------------------------------------------------------------------------
struct Group
{
    int      mGid;      //!< group id
    int16_t  mFirst;    //!< first track
    int16_t  mLast;     //!< last track
    std::string mName;  //!< group name
};

/// netmd groups
using Groups = std::vector<Group>;

/// byte vector
using NetMDByteVector = std::vector<uint8_t>;

//--------------------------------------------------------------------------
//! @brief      format helper for TrackTime
//!
//! @param      o     ref. to ostream
//! @param[in]  tt    TrackTime
//!
//! @return     formatted TrackTime stored in ostream
//--------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& o, const TrackTime& tt);

//--------------------------------------------------------------------------
//! @brief      format helper for AudioEncoding
//!
//! @param      o     ref. to ostream
//! @param[in]  ae    AudioEncoding
//!
//! @return     formatted AudioEncoding stored in ostream
//--------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& o, const AudioEncoding& ae);

//--------------------------------------------------------------------------
//! @brief      format helper for TrackProtection
//!
//! @param      o     ref. to ostream
//! @param[in]  tp    TrackProtection
//!
//! @return     formatted TrackProtection stored in ostream
//--------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& o, const TrackProtection& tp);

//------------------------------------------------------------------------------
//! @brief      Addition assignment operator for NetMDByteVector.
//!
//! @param      a     byte vector 1
//! @param[in]  b     byte vector 2
//!
//! @return     The result of the addition assignment
//------------------------------------------------------------------------------
NetMDByteVector& operator+=(NetMDByteVector& a, const NetMDByteVector& b);

/// disc header
class CMDiscHeader;

/// access device class
class CNetMdDev;

/// secure implementation
class CNetMdSecure;

/// the API class
class CNetMdApi;

/// use netmd_pp instead of CNetMdApi
using  netmd_pp = CNetMdApi;

//------------------------------------------------------------------------------
//! @brief      This class describes a C++ NetMD access library
//------------------------------------------------------------------------------
class CNetMdApi
{
public:
    //--------------------------------------------------------------------------
    //! @brief      Constructs a new instance.
    //--------------------------------------------------------------------------
    CNetMdApi();

    //--------------------------------------------------------------------------
    //! @brief      Destroys the object.
    //--------------------------------------------------------------------------
    ~CNetMdApi();

    //--------------------------------------------------------------------------
    //! @brief      Initializes the device.
    //!
    //! @return     NetMdErr
    //--------------------------------------------------------------------------
    int initDevice();

    //--------------------------------------------------------------------------
    //! @brief      Gets the device name.
    //!
    //! @return     The device name.
    //--------------------------------------------------------------------------
    std::string getDeviceName() const;

    //--------------------------------------------------------------------------
    //! @brief      Sets the log level.
    //!
    //! @param[in]  severity  The severity
    //--------------------------------------------------------------------------
    static void setLogLevel(int severity);

    //--------------------------------------------------------------------------
    //! @brief      Sets the log stream.
    //!
    //! @param[in]  os The stream instance to log to
    //--------------------------------------------------------------------------
    static void setLogStream(std::ostream& os);

    //--------------------------------------------------------------------------
    //! @brief      request track count
    //!
    //! @return     < 0 -> @ref NetMdErr; else -> track count
    //--------------------------------------------------------------------------
    int trackCount();

    //--------------------------------------------------------------------------
    //! @brief      request disc flags
    //!
    //! @return     < 0 -> @ref NetMdErr; else -> flags
    //--------------------------------------------------------------------------
    int discFlags();

    //--------------------------------------------------------------------------
    //! @brief      erase MD
    //!
    //! @return     @ref NetMdErr
    //--------------------------------------------------------------------------
    int eraseDisc();

    //--------------------------------------------------------------------------
    //! @brief      get track time
    //!
    //! @param[in]  trackNo    The track no
    //! @param[out] trackTime  The track time
    //!
    //! @return     @ref NetMdErr
    //--------------------------------------------------------------------------
    int trackTime(int trackNo, TrackTime& trackTime);

    //--------------------------------------------------------------------------
    //! @brief      get disc title
    //!
    //! @param[out] title  The title
    //!
    //! @return     @ref NetMdErr
    //--------------------------------------------------------------------------
    int discTitle(std::string& title);

    //--------------------------------------------------------------------------
    //! @brief      Sets the disc title.
    //!
    //! @param[in]  title  The title
    //!
    //! @return     @ref NetMdErr
    //--------------------------------------------------------------------------
    int setDiscTitle(const std::string& title);

    //--------------------------------------------------------------------------
    //! @brief      move a track (number)
    //!
    //! @param[in]  from  from position
    //! @param[in]  to    to position
    //!
    //! @return     @ref NetMdErr
    //--------------------------------------------------------------------------
    int moveTrack(uint16_t from, uint16_t to);

    //--------------------------------------------------------------------------
    //! @brief      Sets the group title.
    //!
    //! @param[in]  group  The group
    //! @param[in]  title  The title
    //!
    //! @return     @ref NetMdErr
    //--------------------------------------------------------------------------
    int setGroupTitle(uint16_t group, const std::string& title);

    //--------------------------------------------------------------------------
    //! @brief      Creates a group.
    //!
    //! @param[in]  title  The title
    //! @param[in]  first  The first track
    //! @param[in]  last   The last track
    //!
    //! @return     @ref NetMdErr
    //--------------------------------------------------------------------------
    int createGroup(const std::string& title, int first, int last);

    //--------------------------------------------------------------------------
    //! @brief      Adds a track to group.
    //!
    //! @param[in]  track  The track
    //! @param[in]  group  The group
    //!
    //! @return     @ref NetMdErr
    //--------------------------------------------------------------------------
    int addTrackToGroup(int track, int group);

    //--------------------------------------------------------------------------
    //! @brief      remove track from group
    //!
    //! @param[in]  track  The track
    //! @param[in]  group  The group
    //!
    //! @return     @ref NetMdErr
    //--------------------------------------------------------------------------
    int delTrackFromGroup(int track, int group);

    //--------------------------------------------------------------------------
    //! @brief      delete a group
    //!
    //! @param[in]  group  The group
    //!
    //! @return     @ref NetMdErr
    //--------------------------------------------------------------------------
    int deleteGroup(int group);

    //--------------------------------------------------------------------------
    //! @brief      delete track
    //!
    //! @param[in]  track  The track number
    //!
    //! @return     @ref NetMdErr
    //--------------------------------------------------------------------------
    int deleteTrack(uint16_t track);

    //--------------------------------------------------------------------------
    //! @brief      get track bitrate data
    //!
    //! @param[in]  track     The track number
    //! @param[out] encoding  The encoding flag
    //! @param[out] channel   The channel flag
    //!
    //! @return     @ref NetMdErr
    //--------------------------------------------------------------------------
    int trackBitRate(uint16_t track, AudioEncoding& encoding, uint8_t& channel);

    //--------------------------------------------------------------------------
    //! @brief      get track flags
    //!
    //! @param[in]  track  The track number
    //! @param[out] flags  The track flags
    //!
    //! @return     @ref NetMdErr
    //--------------------------------------------------------------------------
    int trackFlags(uint16_t track, TrackProtection& flags);

    //--------------------------------------------------------------------------
    //! @brief      get track title
    //!
    //! @param[in]  track  The track number
    //! @param[out] title  The track title
    //!
    //! @return     @ref NetMdErr
    //--------------------------------------------------------------------------
    int trackTitle(uint16_t track, std::string& title);

    //--------------------------------------------------------------------------
    //! @brief      is SP upload supported?
    //!
    //! @return     true if yes
    //--------------------------------------------------------------------------
    bool spUploadSupported();

    //--------------------------------------------------------------------------
    //! @brief      is on the fly encoding supported by device
    //!
    //! @return     true if so
    //--------------------------------------------------------------------------
    bool otfEncodeSupported();

    //--------------------------------------------------------------------------
    //! @brief      is TOC manipulation supported?
    //!
    //! @return     true if supported, false if not
    //--------------------------------------------------------------------------
    bool tocManipSupported();

    //--------------------------------------------------------------------------
    //! @brief      Sends an audio track
    //!
    //! The audio file must be either an WAVE file (44.1kHz / 16 bit), or an
    //! pre-encoded atrac3 file with a WAVE header. If your device supports
    //! on-the-fly encoding (not common), you can set the DiskFormat to
    //! @ref NETMD_DISKFORMAT_LP4 or @ref NETMD_DISKFORMAT_LP2. If you want best
    //! audio quality, use @ref NO_ONTHEFLY_CONVERSION.
    //!
    //! In case your device supports the SP download through Sony Firmware
    //! exploit, the input file might be a plain atrac 1 file.
    //!
    //!
    //! @param[in]  filename  The filename
    //! @param[in]  title     The title
    //! @param[in]  otf       The disk format
    //!
    //! @return     @ref NetMdErr
    //--------------------------------------------------------------------------
    int sendAudioFile(const std::string& filename, const std::string& title, DiskFormat otf);

    //--------------------------------------------------------------------------
    //! @brief      Sets the track title.
    //!
    //! @param[in]  trackNo  The track no
    //! @param[in]  title    The title
    //!
    //! @return     @ref NetMdErr
    //--------------------------------------------------------------------------
    int setTrackTitle(uint16_t trackNo, const std::string& title);

    //--------------------------------------------------------------------------
    //! @brief      get disc capacity
    //!
    //! @param[out] dcap  The buffer for disc capacity
    //!
    //! @return     @ref NetMdErr
    //--------------------------------------------------------------------------
    int discCapacity(DiscCapacity& dcap);

    //--------------------------------------------------------------------------
    //! @brief      get MD track groups
    //!
    //! @return     vector of group structures
    //--------------------------------------------------------------------------
    Groups groups();

    //--------------------------------------------------------------------------
    //! @brief      prepare TOC manipulation
    //!
    //! @return     @ref NetMdErr
    //--------------------------------------------------------------------------
    int prepareTOCManip();

    //--------------------------------------------------------------------------
    //! @brief      Reads an utoc sector.
    //!
    //! @param[in]  s     sector number
    //!
    //! @return     TOC sector data. (error if empty)
    //--------------------------------------------------------------------------
    NetMDByteVector readUTOCSector(UTOCSector s);

    //--------------------------------------------------------------------------
    //! @brief      Writes an utoc sector.
    //!
    //! @param[in]  s     sector names
    //! @param[in]  data  The data to be written
    //!
    //! @return     @ref NetMdErr
    //--------------------------------------------------------------------------
    int writeUTOCSector(UTOCSector s, const NetMDByteVector& data);

    //--------------------------------------------------------------------------
    //! @brief      finalize TOC through exploit
    //!
    //! @param[in]  reset      do reset if true (default: false)
    //! @param[in]  resetWait  The optional reset wait time (15 seconds)
    //!                        Only needed if reset is true
    //!
    //! @return     NetMdErr
    //! @see        NetMdErr
    //--------------------------------------------------------------------------
    int finalizeTOC(bool reset = false, uint8_t resetWait = 15);

private:
    /// disc header
    CMDiscHeader* mpDiscHeader;

    /// access device class
    CNetMdDev* mpNetMd;

    /// secure implementation
    CNetMdSecure* mpSecure;
};

namespace toc
{
    /// internally used TOC structure
    struct TOC;
}

//------------------------------------------------------------------------------
//! @brief      This class describes a net md TOC.
//------------------------------------------------------------------------------
class CNetMdTOC
{
public:
    /// a fragment used in DAO track
    struct DAOFragment
    {
        uint32_t mStart;    ///< start group
        uint32_t mEnd;      ///< end group
    };

    /// type to store all DAO track fragments (for fragmented, non empty discs)
    using DAOFragments = std::vector<DAOFragment>;

    //--------------------------------------------------------------------------
    //! @brief      Constructs a new instance.
    //!
    //! @param[in]     trackCount  The track count
    //! @param[in]     lenInMs     The length in milliseconds
    //! @param         data        The TOC data
    //--------------------------------------------------------------------------
    CNetMdTOC(int trackCount = 0, uint32_t lenInMs = 0, uint8_t* data = nullptr);

    //--------------------------------------------------------------------------
    //! @brief      Destroys the object.
    //--------------------------------------------------------------------------
    ~CNetMdTOC();

    //--------------------------------------------------------------------------
    //! @brief      import TOC data
    //!
    //! @param[in]  trackCount  The track count
    //! @param[in]  lenInMs     The length in milliseconds
    //! @param      data        The TOC data
    //--------------------------------------------------------------------------
    void import(int trackCount = 0, uint32_t lenInMs = 0, uint8_t* data = nullptr);

    //--------------------------------------------------------------------------
    //! @brief      Adds a track.
    //!
    //! This function has to be used to split a DAO transferred disc audio
    //! track into the parts as on the original disc. This functions has to
    //! be called for all tracks in their correct order!
    //! **Breaking the order will break the TOC!**
    //!
    //! @param[in]  no        track number (starting with 1)
    //! @param[in]  lengthMs  The length in milliseconds
    //! @param[in]  title     The track title
    //!
    //! @return     0 -> ok; -1 -> error
    //--------------------------------------------------------------------------
    int addTrack(uint8_t no, uint32_t lengthMs, const std::string& title);

    //--------------------------------------------------------------------------
    //! @brief      Sets the disc title.
    //!
    //! @param[in]  title  The title
    //!
    //! @return     0 -> ok; -1 -> error
    //--------------------------------------------------------------------------
    int setDiscTitle(const std::string& title);

    //--------------------------------------------------------------------------
    //! @brief      get track count
    //!
    //! @return     number of tracks
    //--------------------------------------------------------------------------
    int trackCount() const;

    //--------------------------------------------------------------------------
    //! @brief      get MD title
    //!
    //! @return     title
    //--------------------------------------------------------------------------
    std::string discTitle() const;

    //--------------------------------------------------------------------------
    //! @brief      get track title
    //!
    //! @param[in]  trackNo  The track number
    //!
    //! @return     title
    //--------------------------------------------------------------------------
    std::string trackTitle(int trackNo) const;

    //--------------------------------------------------------------------------
    //! @brief      get track info
    //!
    //! @param[in]  trackNo  The track number
    //!
    //! @return     track info
    //--------------------------------------------------------------------------
    std::string trackInfo(int trackNo) const;

    //--------------------------------------------------------------------------
    //! @brief      get disc info
    //!
    //! @return     disc info
    //--------------------------------------------------------------------------
    std::string discInfo() const;

private:
    /// TOC pointer
    toc::TOC* mpToc;

    /// number of tracks for this TOC
    int       mTracksCount;

    /// complete length of all tracks in ms
    uint32_t  mLengthInMs;

    /// current group position
    uint32_t  mCurPos;

    /// track we need to split
    int       mDAOTrack;

    /// whole groups count of DAO track
    uint32_t  mDAOGroups;

    /// the fragments used for DAO track
    DAOFragments mDAOFragments;
};


} // ~namespace
