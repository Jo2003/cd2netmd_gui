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
    //! @param[in]  resetWait  The optional reset wait time (15 seconds)
    //!
    //! @return     NetMdErr
    //! @see        NetMdErr
    //--------------------------------------------------------------------------
    int finalizeTOC(uint8_t resetWait = 15);

private:
    /// disc header
    CMDiscHeader* mpDiscHeader;

    /// access device class
    CNetMdDev* mpNetMd;

    /// secure implementation
    CNetMdSecure* mpSecure;
};

/// cluster / sector / group helper
class CSG;

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

    /// group number where audio starts
    uint32_t  mAudioStart;

    /// group number where audio ends
    uint32_t  mAudioEnd;

    /// number of tracks for this TOC
    int       mTracksCount;

    /// complete length of all tracks in ms
    uint32_t  mLengthInMs;

    /// current group position
    CSG*      mpCurPos;

    /// track we need to split
    int       mDAOTrack;
};


} // ~namespace
