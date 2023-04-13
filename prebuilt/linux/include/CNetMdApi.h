/*
 * CNetMdApi.h
 *
 * This file is part of netmd++, a library for accessing NetMD devices.
 *
 * It makes use of knowledge / code collected by Marc Britten and
 * Alexander Sulfrian for the Linux Minidisc project.
 *
 * Asivery helped to make this possible!
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
#pragma once
#include <cstdint>
#include <sstream>
#include <vector>

namespace netmd {

/// disk format
enum DiskFormat : uint8_t
{
    NETMD_DISKFORMAT_LP4       =   0,
    NETMD_DISKFORMAT_LP2       =   2,
    NETMD_DISKFORMAT_SP_MONO   =   4,
    NETMD_DISKFORMAT_SP_STEREO =   6,
    NO_ONTHEFLY_CONVERSION     = 0xf
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
    int mMinutes;
    int mSeconds;
    int mTenthSecs;
};

/// type safe protection flags
enum class TrackProtection : uint8_t
{
    UNPROTECTED = 0x00,
    PROTECTED   = 0x03,
    UNKNOWN     = 0xFF
};

/// type safe encoding flags
enum class AudioEncoding : uint8_t
{
    SP      = 0x90,
    LP2     = 0x92,
    LP4     = 0x93,
    UNKNOWN = 0xff
};

/// log severity
enum typelog
{
    DEBUG,
    INFO,
    WARN,
    CRITICAL,
    CAPTURE //!< needed for log parcing!
};

/// NetMD time
struct NetMdTime
{
    uint16_t hour;
    uint8_t  minute;
    uint8_t  second;
    uint8_t  frame;
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

using Groups = std::vector<Group>;

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

/// disc header
class CMDiscHeader;

/// access device class
class CNetMdDev;

/// secure implmentation
class CNetMdSecure;

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
    //! @brief      Initializes the disc header.
    //!
    //! @return     NetMdErr
    //--------------------------------------------------------------------------
    int initDiscHeader();

    //--------------------------------------------------------------------------
    //! @brief      Gets the device name.
    //!
    //! @return     The device name.
    //--------------------------------------------------------------------------
    std::string getDeviceName();

    //--------------------------------------------------------------------------
    //! @brief      Sets the log level.
    //!
    //! @param[in]  severity  The severity
    //--------------------------------------------------------------------------
    static void setLogLevel(int severity);

    //--------------------------------------------------------------------------
    //! @brief      Sets the log stream.
    //!
    //! @param      os    The stream instance to log to
    //--------------------------------------------------------------------------
    static void setLogStream(std::ostream& os);

    //--------------------------------------------------------------------------
    //! @brief      request track count
    //!
    //! @return     < 0 -> NetMdErr; else -> track count
    //--------------------------------------------------------------------------
    int trackCount();

    //--------------------------------------------------------------------------
    //! @brief      request disc flags
    //!
    //! @return     < 0 -> NetMdErr; else -> flags
    //--------------------------------------------------------------------------
    int discFlags();

    //--------------------------------------------------------------------------
    //! @brief      erase MD
    //!
    //! @return     NetMdErr
    //--------------------------------------------------------------------------
    int eraseDisc();

    //--------------------------------------------------------------------------
    //! @brief      get track time
    //!
    //! @param[in]  trackNo    The track no
    //! @param      trackTime  The track time
    //!
    //! @return     NetMdErr
    //--------------------------------------------------------------------------
    int trackTime(int trackNo, TrackTime& trackTime);

    //--------------------------------------------------------------------------
    //! @brief      get raw disc header
    //!
    //! @param[out] header  The buffer for disc header
    //!
    //! @return     NetMdErr
    //--------------------------------------------------------------------------
    int rawDiscHeader(std::string& header);

    //--------------------------------------------------------------------------
    //! @brief      get disc title
    //!
    //! @param[out] title  The title
    //!
    //! @return     NetMdErr
    //--------------------------------------------------------------------------
    int discTitle(std::string& title);

    //--------------------------------------------------------------------------
    //! @brief      Sets the disc title.
    //!
    //! @param[in]  title  The title
    //!
    //! @return     NetMdErr
    //--------------------------------------------------------------------------
    int setDiscTitle(const std::string& title);

    //--------------------------------------------------------------------------
    //! @brief      Writes a disc header.
    //!
    //! @return     NetMdErr
    //--------------------------------------------------------------------------
    int writeRawDiscHeader();

    //--------------------------------------------------------------------------
    //! @brief      move a track (number)
    //!
    //! @param[in]  from  from position
    //! @param[in]  to    to position
    //!
    //! @return     NetMdErr
    //--------------------------------------------------------------------------
    int moveTrack(uint16_t from, uint16_t to);

    //--------------------------------------------------------------------------
    //! @brief      Sets the group title.
    //!
    //! @param[in]  group  The group
    //! @param[in]  title  The title
    //!
    //! @return     NetMdErr
    //--------------------------------------------------------------------------
    int setGroupTitle(uint16_t group, const std::string& title);

    //--------------------------------------------------------------------------
    //! @brief      Creates a group.
    //!
    //! @param[in]  title  The title
    //! @param[in]  first  The first track
    //! @param[in]  last   The last track
    //!
    //! @return     NetMdErr
    //--------------------------------------------------------------------------
    int createGroup(const std::string& title, int first, int last);

    //--------------------------------------------------------------------------
    //! @brief      Adds a track to group.
    //!
    //! @param[in]  track  The track
    //! @param[in]  group  The group
    //!
    //! @return     NetMdErr
    //--------------------------------------------------------------------------
    int addTrackToGroup(int track, int group);

    //--------------------------------------------------------------------------
    //! @brief      remove track from group
    //!
    //! @param[in]  track  The track
    //! @param[in]  group  The group
    //!
    //! @return     NetMdErr
    //--------------------------------------------------------------------------
    int delTrackFromGroup(int track, int group);

    //--------------------------------------------------------------------------
    //! @brief      delete a group
    //!
    //! @param[in]  group  The group
    //!
    //! @return     NetMdErr
    //--------------------------------------------------------------------------
    int deleteGroup(int group);

    //--------------------------------------------------------------------------
    //! @brief      delete track
    //!
    //! @param[in]  track  The track number
    //!
    //! @return     NetMdErr
    //--------------------------------------------------------------------------
    int deleteTrack(uint16_t track);

    //--------------------------------------------------------------------------
    //! @brief      get track bitrate data
    //!
    //! @param[in]  track     The track number
    //! @param[out] encoding  The encoding flag
    //! @param[out] channel   The channel flag
    //!
    //! @return     NetMdErr
    //--------------------------------------------------------------------------
    int trackBitRate(uint16_t track, AudioEncoding& encoding, uint8_t& channel);

    //--------------------------------------------------------------------------
    //! @brief      get track flags
    //!
    //! @param[in]  track  The track number
    //! @param[out] flags  The track flags
    //!
    //! @return     NetMdErr
    //--------------------------------------------------------------------------
    int trackFlags(uint16_t track, TrackProtection& flags);

    //--------------------------------------------------------------------------
    //! @brief      get track title
    //!
    //! @param[in]  track  The track number
    //! @param[out] title  The track title
    //!
    //! @return     NetMdErr
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
    //! @brief      Sends an audio track
    //!
    //! @param[in]  filename  The filename
    //! @param[in]  title     The title
    //! @param[in]  otf       The disk format
    //!
    //! @return     NetMdErr
    //! @see        NetMdErr
    //--------------------------------------------------------------------------
    int sendAudioFile(const std::string& filename, const std::string& title, DiskFormat otf);

    //--------------------------------------------------------------------------
    //! @brief      Sets the track title.
    //!
    //! @param[in]  trackNo  The track no
    //! @param[in]  title    The title
    //!
    //! @return     NetMdErr
    //! @see        NetMdErr
    //--------------------------------------------------------------------------
    int setTrackTitle(uint16_t trackNo, const std::string& title);

    //--------------------------------------------------------------------------
    //! @brief      get disc capacity
    //!
    //! @param      dcap  The buffer for disc capacity
    //!
    //! @return     NetMdErr
    //! @see        NetMdErr
    //--------------------------------------------------------------------------
    int discCapacity(DiscCapacity& dcap);

    //--------------------------------------------------------------------------
    //! @brief      get MD track groups
    //!
    //! @return     vector of group structures
    //--------------------------------------------------------------------------
    Groups groups();

private:
    /// disc header
    CMDiscHeader* mpDiscHeader;

    /// access device class
    CNetMdDev* mpNetMd;

    /// secure implmentation
    CNetMdSecure* mpSecure;
};

} // ~namespace
