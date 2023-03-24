/*
 * libnetmd_intern.h
 *
 * This file is part of libnetmd, a library for accessing Sony NetMD devices.
 *
 * Copyright (C) 2002, 2003 Marc Britten
 * Copyright (C) 2011 Alexander Sulfrian
 * Copyright (C) 2021 - 2023 Jo2003 (olenka.joerg@gmail.com)
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
#ifndef LIBNETMD_H
#define LIBNETMD_H
#include <stdio.h>
#include <libusb-1.0/libusb.h>
#include <stdint.h>

/**
   Error codes of the USB transport layer
*/
#define NETMDERR_USB -1         /**< general USB error */
#define NETMDERR_NOTREADY -2    /**< player not ready for command */
#define NETMDERR_TIMEOUT -3     /**< timeout while waiting for response */
#define NETMDERR_CMD_FAILED -4  /**< minidisc responded with 08 response */
#define NETMDERR_CMD_INVALID -5 /**< minidisc responded with 0A response */

/**
   Playmode values to be sent to netmd_set_playmode. These can be combined by
   OR-ing them to do shuffle repeat for example.

   See also: http://article.gmane.org/gmane.comp.audio.netmd.devel/848
*/
#define NETMD_PLAYMODE_SINGLE 0x0040
#define NETMD_PLAYMODE_REPEAT 0x0080
#define NETMD_PLAYMODE_SHUFFLE 0x0100

/**
   playback controll commands
*/
#define NETMD_PLAY 0x75
#define NETMD_PAUSE 0x7d
#define NETMD_REWIND 0x49
#define NETMD_FFORWARD 0x39

/**
   change track commands
*/
#define NETMD_TRACK_PREVIOUS 0x0002
#define NETMD_TRACK_NEXT 0x8001
#define NETMD_TRACK_RESTART 0x0001

/**
   NetMD Protocol return status (first byte of request)
*/
#define NETMD_STATUS_CONTROL 0x00
#define NETMD_STATUS_STATUS 0x01
#define NETMD_STATUS_SPECIFIC_INQUIRY 0x02
#define NETMD_STATUS_NOTIFY 0x03
#define NETMD_STATUS_GENERAL_INQUIRY 0x04

/**
   NetMD Protocol return status (first byte of response)
*/
#define NETMD_STATUS_NOT_IMPLEMENTED 0x08
#define NETMD_STATUS_ACCEPTED 0x09
#define NETMD_STATUS_REJECTED 0x0a
#define NETMD_STATUS_IN_TRANSITION 0x0b
#define NETMD_STATUS_IMPLEMENTED 0x0c
#define NETMD_STATUS_CHANGED 0x0d
#define NETMD_STATUS_INTERIM 0x0f

#define NETMD_ENCODING_SP 0x90
#define NETMD_ENCODING_LP2 0x92
#define NETMD_ENCODING_LP4 0x93

#define NETMD_CHANNELS_MONO 0x01
#define NETMD_CHANNELS_STEREO 0x00

#define NETMD_OPERATING_STATUS_USB_RECORDING 0x56ff
#define NETMD_OPERATING_STATUS_RECORDING 0xc275
#define NETMD_OPERATING_STATUS_RECORDING_PAUSED 0xc27d
#define NETMD_OPERATING_STATUS_FAST_FORWARDING 0xc33f
#define NETMD_OPERATING_STATUS_REWINDING 0xc34f
#define NETMD_OPERATING_STATUS_PLAYING 0xc375
#define NETMD_OPERATING_STATUS_PAUSED 0xc37d
#define NETMD_OPERATING_STATUS_STOPPED 0xc5ff

#define NETMD_TRACK_FLAG_PROTECTED 0x03

#define NETMD_DISC_FLAG_WRITABLE 0x10
#define NETMD_DISC_FLAG_WRITE_PROTECTED 0x40

#define NETMD_DISKFORMAT_LP4 0
#define NETMD_DISKFORMAT_LP2 2
#define NETMD_DISKFORMAT_SP_MONO 4
#define NETMD_DISKFORMAT_SP_STEREO 6
#define NO_ONTHEFLY_CONVERSION 0xf

#define NETMD_RIFF_FORMAT_TAG_ATRAC3 0x270
#define NETMD_DATA_BLOCK_SIZE_LP2 384
#define NETMD_DATA_BLOCK_SIZE_LP4 192

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
   Enum with possible error codes the netmd_* functions could return.

   @see netmd_strerror
*/
typedef enum {

    NETMD_NO_ERROR = 0,
    NETMD_NOT_IMPLEMENTED,

    NETMD_USB_OPEN_ERROR,
    NETMD_USB_ERROR,

    NETMD_ERROR,

    NETMD_RESPONSE_TO_SHORT,
    NETMD_RESPONSE_NOT_EXPECTED,

    NETMD_COMMAND_FAILED_NO_RESPONSE,
    NETMD_COMMAND_FAILED_NOT_IMPLEMENTED,
    NETMD_COMMAND_FAILED_REJECTED,
    NETMD_COMMAND_FAILED_UNKNOWN_ERROR,

    NETMD_DES_ERROR,

    NETMD_USE_HOTPLUG

} netmd_error;

/**
   Function that could be used to get a string describing the given error
   number.

   @param error Error number to get the description for.
   @return Pointer to static char buffer to the error description. (Should not
           be freed.)
*/
const char* netmd_strerror(netmd_error error);


/**
   log level
*/
typedef enum {
        /** Not a log level. Should only be used to filter all log messages so
            that no messages are displayed */
        NETMD_LOG_NONE,

        /** fatal error message */
        NETMD_LOG_ERROR,

        /** warning messages */
        NETMD_LOG_WARNING,

        /** debug messages */
        NETMD_LOG_VERBOSE,

        /** messages to display */
        NETMD_LOG_DEBUG,

        /** Not a log level. Should only be used to display all messages. Should
            be the level with the highest value. */
        NETMD_LOG_ALL
} netmd_loglevel;

/**
   Sets the global log level.

   @param level The maximal log level. All messages with a higher log level are
                filtered out and will not be displayed.
*/
void netmd_set_log_level(netmd_loglevel level);

/**
   Shows a hexdump of binary data.

   @param level Log level of this message.
   @param data Pointer to binary data to display.
   @param len Length of the data.
*/
void netmd_log_hex(netmd_loglevel level, const unsigned char* const data, const size_t len);

/**
   Printf like log function.

   @param level Log level of this message.
   @param fmt printf-like format string
*/
void netmd_log(netmd_loglevel level, const char* const fmt, ...);

/**
 * @brief      Sets the log file descriptor
 *
 * @param[in]  fdid  se fdid as stdout
 */
void netmd_log_set_fd(FILE* fdid);


/**
   Typedef that nearly all netmd_* functions use to identify the USB connection
   with the minidisc player.
*/
typedef libusb_device_handle *netmd_dev_handle;

/**
  Function to exchange command/response buffer with minidisc player.

  @param dev device handle
  @param cmd buffer with the command, that should be send to the player
  @param cmdlen length of the command
  @param rsp buffer where the response should be written to
  @return number of bytes received if >0, or error if <0
*/
int netmd_exch_message(netmd_dev_handle *dev, unsigned char *cmd,
                       const size_t cmdlen, unsigned char *rsp);

//------------------------------------------------------------------------------
//! @brief      exchange command / response (extended version)
//!
//! @param      devh    The devh
//! @param      cmd     The command
//! @param[in]  cmdlen  The cmdlen
//! @param      rspPtr  The rsp pointer (must be freed afterwards if not NULL)
//!
//! @return     < 0 -> error; else -> received bytes
//------------------------------------------------------------------------------
int netmd_exch_message_ex(netmd_dev_handle *devh, unsigned char *cmd,
                          const size_t cmdlen, unsigned char **rspPtr);

/**
  Function to send a command to the minidisc player.

  @param dev device handle
  @param cmd buffer with the command, that should be send to the player
  @param cmdlen length of the command
*/
int netmd_send_message(netmd_dev_handle *dev, unsigned char *cmd,
                       const size_t cmdlen);


/**
  Function to recieve a response from the minidisc player.

  @param rsp buffer where the response should be written to
  @return number of bytes received if >0, or error if <0
*/
int netmd_recv_message(netmd_dev_handle *dev, unsigned char *rsp);

//------------------------------------------------------------------------------
//! @brief      receive a message (extended version)
//!
//! @param      devh    The devh
//! @param      rspPtr  The rsp pointer (must be freed afterwards)
//!
//! @return     length of data stored in *rspPtr
//------------------------------------------------------------------------------
int netmd_recv_message_ex(netmd_dev_handle *devh, unsigned char** rspPtr);

/**
   Wait for the device to respond to commands. Should only be used
   when the device needs to be given "breathing room" and is not
   expected to have anything to send.

   @param dev device handle
   @return 1 if success, 0 if there was no response
*/
int netmd_wait_for_sync(netmd_dev_handle* dev);

//------------------------------------------------------------------------------
//! @brief      enable / disable factory write
//!
//! @param      devh    The devh
//! @param      enable  1 to enable factory write, 0 to disable
//------------------------------------------------------------------------------
void netmd_set_factory_write(int enable);


//! define a MD Header handle
typedef void* HndMdHdr;

//! c structure to hold a groups
typedef struct {
    int      mGid;      //!< group id
    int16_t  mFirst;    //!< first track
    int16_t  mLast;     //!< last track
    char*    mpName;    //!< group name
} MDGroup;

//! a groups container
typedef struct {
    int      mCount;
    MDGroup* mpGroups;
} MDGroups;

//------------------------------------------------------------------------------
//! @brief      Creates a md header.
//!
//! @param[in]  content  The content
//!
//! @return     The handle md header.
//------------------------------------------------------------------------------
HndMdHdr create_md_header(const char* content);

//------------------------------------------------------------------------------
//! @brief         free the MD header
//!
//! @param[in/out] hdl   handle to MD header
//------------------------------------------------------------------------------
void free_md_header(HndMdHdr* hdl);

//------------------------------------------------------------------------------
//! @brief      create C string from MD header
//!
//! @param[in]  hdl   The MD header handle
//!
//! @return     C string or NULL
//------------------------------------------------------------------------------
const char* md_header_to_string(HndMdHdr hdl);

//------------------------------------------------------------------------------
//! @brief      add a group to the MD header
//!
//! @param[in]  hdl    The MD header handlehdl
//! @param[in]  name   The name
//! @param[in]  first  The first
//! @param[in]  last   The last
//!
//! @return     > -1 -> group id; else -> error
//------------------------------------------------------------------------------
int md_header_add_group(HndMdHdr hdl, const char* name, int16_t first, int16_t last);

//------------------------------------------------------------------------------
//! @brief      list groups in MD header
//!
//! @param[in]  hdl   The MD header handle
//------------------------------------------------------------------------------
void md_header_list_groups(HndMdHdr hdl);

//------------------------------------------------------------------------------
//! @brief      Adds a track to group.
//!
//! @param[in]  hdl    The MD header handle
//! @param[in]  gid    The group id
//! @param[in]  track  The track number
//!
//! @return     0 -> ok; -1 -> error
//------------------------------------------------------------------------------
int md_header_add_track_to_group(HndMdHdr hdl, int gid, int16_t track);

//-----------------------------------------------------------------------------
//! @brief      remove a track from a group.
//!
//! @param[in]  hdl    The MD header handle
//! @param[in]  gid    The group id
//! @param[in]  track  The track number
//!
//! @return     0 -> ok; -1 -> error
//-----------------------------------------------------------------------------
int md_header_del_track_from_group(HndMdHdr hdl, int gid, int16_t track);

//-----------------------------------------------------------------------------
//! @brief      remove a track
//!
//! @param[in]  hdl    The MD header handle
//! @param[in]  track  The track number
//!
//! @return     0 -> ok; -1 -> error
//-----------------------------------------------------------------------------
int md_header_del_track(HndMdHdr hdl, int16_t track);

//-----------------------------------------------------------------------------
//! @brief      remove a group (included tracks become ungrouped)
//!
//! @param[in]  hdl   The MD header handle
//! @param[in]  gid   The group id
//!
//! @return     0 -> ok; -1 -> error
//-----------------------------------------------------------------------------
int md_header_del_group(HndMdHdr hdl, int gid);

//-----------------------------------------------------------------------------
//! @brief      Sets the disc title.
//!
//! @param[in]  hdl    The MD header handle
//! @param[in]  title  The title
//!
//! @return     0 -> ok; else -> error
//-----------------------------------------------------------------------------
int md_header_set_disc_title(HndMdHdr hdl, const char* title);

//-----------------------------------------------------------------------------
//! @brief      rename one group
//!
//! @param[in]  hdl    The MD header handle
//! @param[in]  gid    The group id
//! @param[in]  title  The new title
//!
//! @return     0 -> ok; else -> error
//-----------------------------------------------------------------------------
int md_header_rename_group(HndMdHdr hdl, int gid, const char* title);

//------------------------------------------------------------------------------
//! @brief      get disc title
//!
//! @param[in]  hdl   The MD header handle
//!
//! @return     C string
//------------------------------------------------------------------------------
const char* md_header_disc_title(HndMdHdr hdl);

//------------------------------------------------------------------------------
//! @brief      get group name for track
//!
//! @param[in]  hdl    The MD header handle
//! @param[in]  track  The track number
//! @param[out] pGid   The buffer for gid
//!
//! @return     C string or nullptr
//------------------------------------------------------------------------------
const char* md_header_track_group(HndMdHdr hdl, int16_t track, int16_t* pGid);

//------------------------------------------------------------------------------
//! @brief      unhroup track
//!
//! @param[in]  hdl    The MD header handle
//! @param[in]  track  The track
//!
//! @return     0 -> ok; -1 -> error
//------------------------------------------------------------------------------
int md_header_ungroup_track(HndMdHdr hdl, int16_t track);

//------------------------------------------------------------------------------
//! @brief      export all minidisc groups
//!
//! @param[in]  hdl The MD header handle
//!
//! @return     array of groups
//------------------------------------------------------------------------------
MDGroups* md_header_groups(HndMdHdr hdl);

//------------------------------------------------------------------------------
//! @brief      free groups you've got through md_header_groups()
//!
//! @param      groups  The groups
//------------------------------------------------------------------------------
void md_header_free_groups(MDGroups** groups);


/**
   Data about a group, start track, finish track and name. Used to generate disc
   header info.
*/
typedef struct netmd_group
{
    uint16_t start;
    uint16_t finish;
    char* name;
} netmd_group_t;

/**
   Basic track data.
*/
struct netmd_track
{
    int track;
    int minute;
    int second;
    int tenth;
};

/**
   stores hex value from protocol and text value of name
*/
typedef struct netmd_pair
{
    unsigned char hex;
    const char* const name;
} netmd_pair_t;

/**
   stores misc data for a minidisc
*/
typedef struct {
    size_t header_length;
    struct netmd_group *groups;
    unsigned int group_count;
} minidisc;

typedef enum {
    // TD - text database
    discTitleTD,
    audioUTOC1TD,
    audioUTOC4TD,
    DSITD,
    audioContentsTD,
    rootTD,

    discSubunitIndentifier,
    operatingStatusBlock, // Real name unknown
} netmd_descriptor_t;

typedef struct
{
    netmd_descriptor_t descr;
    uint8_t data[3];
    size_t sz;
} netmd_descr_val_t;

typedef enum {
    nda_openread = 0x01,
    nda_openwrite = 0x03,
    nda_close = 0x00,
} netmd_descriptor_action_t;

/**
   Global variable containing netmd_group data for each group. There will be
   enough for group_count total in the alloced memory
*/
extern struct netmd_group* groups;
extern struct netmd_pair const trprot_settings[];
extern struct netmd_pair const bitrates[];
extern struct netmd_pair const unknown_pair;

/**
   enum through an array of pairs looking for a specific hex code.
   @param hex hex code to find.
   @param pair array of pairs to look through.
*/
struct netmd_pair const* find_pair(int hex, struct netmd_pair const* pair);

/**
 * @brief      get track time
 *
 * @param      dev     The MD dev
 * @param[in]  track   The track number
 * @param      buffer  The time buffer
 *
 * @return     int
 */
int netmd_request_track_time(netmd_dev_handle* dev, const uint16_t track, struct netmd_track* buffer);

/**
   Sets title for the specified track. If making multiple changes,
   call netmd_cache_toc before netmd_set_title and netmd_sync_toc
   afterwards.

   @param dev pointer to device returned by netmd_open
   @param track Zero based index of track your requesting.
   @param buffer buffer to hold the name.
   @return returns 0 for fail 1 for success.
*/
int netmd_set_title(netmd_dev_handle* dev, const uint16_t track, const char* const buffer);

/**
   Moves track around the disc.

   @param dev pointer to device returned by netmd_open
   @param start Zero based index of track to move
   @param finish Zero based track to make it
   @return 0 for fail 1 for success
*/
int netmd_move_track(netmd_dev_handle* dev, const uint16_t start, const uint16_t finish);

/**
   Sets title for the specified track.

   @param dev pointer to device returned by netmd_open
   @param md pointer to minidisc structure
   @param group Zero based index of group your renaming (zero is disc title).
   @param title buffer holding the name.
   @return returns 0 for fail 1 for success.
*/
int netmd_set_group_title(netmd_dev_handle* dev, HndMdHdr md, unsigned int group, const char* title);

/**
   sets up buffer containing group info.

  @param dev pointer to device returned by netmd_open
  @param md pointer to minidisc structure
  @return total size of disc header Group[0] is disc name.  You need to make
          sure you call clean_disc_info before recalling
*/
int netmd_initialize_disc_info(netmd_dev_handle* devh, HndMdHdr* md);

void print_groups(HndMdHdr md);

int netmd_create_group(netmd_dev_handle* dev, HndMdHdr md, char* name, int first, int last);

int netmd_set_disc_title(netmd_dev_handle* dev, char* title, size_t title_length);

/**
   Moves track into group

   @param dev pointer to device returned by netmd_open
   @param md pointer to minidisc structure
   @param track Zero based track to add to group.
   @param group number of group (0 is title group).
*/
int netmd_put_track_in_group(netmd_dev_handle* dev, HndMdHdr md, const uint16_t track, const unsigned int group);

/**
   Removes track from group

   @param dev pointer to device returned by netmd_open
   @param md pointer to minidisc structure
   @param track Zero based track to add to group.
   @param group number of group (0 is title group).
*/
int netmd_pull_track_from_group(netmd_dev_handle* dev, HndMdHdr md, const uint16_t track, const unsigned int group);

/**
   Deletes group from disc (but not the tracks in it)

   @param dev pointer to device returned by netmd_open
   @param md pointer to minidisc structure
   @param group Zero based track to delete
*/
int netmd_delete_group(netmd_dev_handle* dev, HndMdHdr md, const unsigned int group);

/**
   Creates disc header out of groups and writes it to disc

   @param devh pointer to device returned by netmd_open
   @param md pointer to minidisc structure
*/
int netmd_write_disc_header(netmd_dev_handle* devh, HndMdHdr md);

/**
   Writes atrac file to device

   @param dev pointer to device returned by netmd_open
   @param szFile Full path to file to write.
   @return < 0 on fail else 1
   @bug doesnt work yet
*/
int netmd_write_track(netmd_dev_handle* devh, char* szFile);

/**
   Deletes track from disc (does not update groups)

   @param dev pointer to device returned by netmd_open
   @param track Zero based track to delete
*/
int netmd_delete_track(netmd_dev_handle* dev, const uint16_t track);

/**
   Erase all disc contents

   @param dev pointer to device returned by netmd_open
*/
int netmd_erase_disc(netmd_dev_handle* dev);

/* AV/C Description Spefication OPEN DESCRIPTOR (0x08),
 * subfunction "open for write" (0x03) */
int netmd_cache_toc(netmd_dev_handle* dev);

/* AV/C Description Spefication OPEN DESCRIPTOR (0x08),
 * subfunction "close" (0x00) */
int netmd_sync_toc(netmd_dev_handle* dev);

/* Calls need for Sharp devices */
int netmd_acquire_dev(netmd_dev_handle* dev);

int netmd_release_dev(netmd_dev_handle* dev);

/*! @brief      request disc title (raw header)

    @param      dev     The dev
    @param      buffer  The buffer
    @param[in]  size    The size

    @return     title size
 */
int netmd_request_raw_header(netmd_dev_handle* dev, char* buffer, size_t size);

int netmd_request_raw_header_ex(netmd_dev_handle* dev, char** buffer);

//------------------------------------------------------------------------------
//! @brief      get track count from MD device
//!
//! @param[in]  dev     The dev handle
//! @param[out] tcount  The tcount buffer
//!
//! @return     0 -> ok; -1 -> error
//------------------------------------------------------------------------------
int netmd_request_track_count(netmd_dev_handle* dev, uint16_t* tcount);

//------------------------------------------------------------------------------
//! @brief      get disc flags 
//!
//! @param      dev    The dev
//! @param      flags  buffer for the flags
//!
//! @return     0 -> ok; -1 -> error
//------------------------------------------------------------------------------
int netmd_request_disc_flags(netmd_dev_handle* dev, uint8_t* flags);

//------------------------------------------------------------------------------
//! @brief      change descriptor state
//!
//! @param      devh[in]   device handle
//! @param      descr[in]  descriptor
//! @param      act[in]    descriptor action
//!
//! @return     0 -> ok; -1 -> error
//------------------------------------------------------------------------------
int netmd_change_descriptor_state(netmd_dev_handle* devh, netmd_descriptor_t descr, netmd_descriptor_action_t act);


typedef struct netmd_device {
    struct netmd_device *link;
    char name[32];
    char *model;
    struct libusb_device *usb_dev;
    int otf_conv;
    int idVendor;
} netmd_device;

/**
  Struct to hold the vendor and product id's for each unit.
*/
struct netmd_devices {
    int	idVendor;
    int	idProduct;
    char *model;
    int otf_conv;
};

/**
  Intialises the netmd device layer, scans the USB and fills in a list of
  supported devices.

  @param device_list Linked list of netmd_device_t structures to fill.
  @param libusb_context of a running instance of libusb
*/
netmd_error netmd_init(netmd_device **device_list, libusb_context * hctx);

/**
  Opens a NetMD device.

  @param dev Pointer to a device discoverd by netmd_init.
  @param dev_handle Pointer to variable to save the handle of the opened
                    device used for communication in all other netmd_
                    functions.
*/
netmd_error netmd_open(netmd_device *dev, netmd_dev_handle **dev_handle);

/**
  Get the device name stored in USB device.

  @param devh Pointer to device, returned by netmd_open.
  @param buf Buffer to hold the name.
  @param buffsize Available size in buf.
*/
netmd_error netmd_get_devname(netmd_dev_handle* devh, char *buf, size_t buffsize);

/**
  Closes the usb descriptors.

  @param dev Pointer to device returned by netmd_open.
*/
netmd_error netmd_close(netmd_dev_handle* dev);

/**
  Cleans structures created by netmd_init.

  @param device_list List of devices filled by netmd_init.
*/
void netmd_clean(netmd_device **device_list);


//------------------------------------------------------------------------------
//! @brief      send audio file to netmd device
//!
//! @param      devh[in]     device handle
//! @param      filename[in] audio track file name
//! @param      in_title[in] track title
//! @param      otf[in]      on the fly convert flag
//!
//! @return     netmd_error
//! @see        betmd_error
//------------------------------------------------------------------------------
netmd_error netmd_send_track(netmd_dev_handle *devh, const char *filename, const char *in_title, unsigned char otf);


//------------------------------------------------------------------------------
//! @brief      appy SP patch
//!
//! @param[in]  devh         device handle
//! @param[in]  chan_no      number of audio channels (1: mono, 2: stereo)
//!
//! @return     netmd_error
//! @see        netmd_error
//------------------------------------------------------------------------------
netmd_error netmd_apply_sp_patch(netmd_dev_handle *devh, int chan_no);

//------------------------------------------------------------------------------
//! @brief      undo SP upload patch
//!
//! @param[in]  devh  device handle
//------------------------------------------------------------------------------
void netmd_undo_sp_patch(netmd_dev_handle *devh);

//------------------------------------------------------------------------------
//! @brief      check if device supports sp upload
//!
//! @param[in]  devh  device handle
//!
//! @return     0 -> no support; esle
//------------------------------------------------------------------------------
int netmd_dev_supports_sp_upload(netmd_dev_handle *devh);


/**
   linked list to store a list of 16-byte keys
*/
typedef struct netmd_keychain {
    char *key;
    struct netmd_keychain *next;
} netmd_keychain;

/**
   enabling key block
*/
typedef struct {
    /** The ID of the EKB. */
    uint32_t id;

    /** A chain of encrypted keys. The one end of the chain is the encrypted
       root key, the other end is a key encrypted by a key the device has in
       it's key set. The direction of the chain is not yet known. */
    netmd_keychain *chain;

    /** Selects which key from the devices keyset has to be used to start
       decrypting the chain. Each key in the key set corresponds to a specific
       depth in the tree of device IDs. */
    uint32_t depth;

    /** Signature of the root key (24 byte). Used to verify integrity of the
       decrypted root key by the device. */
    char *signature;
} netmd_ekb;

/**
   linked list, storing all information of the single packets, send to the device
   while uploading a track
*/
typedef struct netmd_track_packets {
    /** encrypted key for this packet (8 bytes) */
    unsigned char *key;

    /** IV for the encryption  (8 bytes) */
    unsigned char *iv;

    /** the packet data itself */
    unsigned char *data;

    /** length of the data */
    size_t length;

    /** next packet to transfer (linked list) */
    struct netmd_track_packets *next;
} netmd_track_packets;

/**
   Format of the song data packets, that are transfered over USB.
*/
typedef enum {
    NETMD_WIREFORMAT_PCM = 0,
    NETMD_WIREFORMAT_105KBPS = 0x90,
    NETMD_WIREFORMAT_LP2 = 0x94,
    NETMD_WIREFORMAT_LP4 = 0xa8
} netmd_wireformat;

/**
   Enter a session secured by a root key found in an EKB. The EKB for this
   session has to be download after entering the session.
*/
netmd_error netmd_secure_enter_session(netmd_dev_handle *dev);

/**
   Forget the key material from the EKB used in the secure session.
*/
netmd_error netmd_secure_leave_session(netmd_dev_handle *dev);

/**
   Read the leaf ID of the present NetMD device. The leaf ID tells which keys the
   device posesses, which is needed to find out which parts of the EKB needs to
   be sent to the device for it to decrypt the root key.
*/
netmd_error netmd_secure_get_leaf_id(netmd_dev_handle *dev, uint64_t *player_id);

/**
   Send key data to the device. The device uses it's builtin key
   to decrypt the root key from an EKB.
*/
netmd_error netmd_secure_send_key_data(netmd_dev_handle *dev, netmd_ekb *ekb);

/**
   Exchange a session key with the device. Needs to have a root key sent to the
   device using sendKeyData before.

   @param rand_in 8 bytes random binary data
   @param rand_out device nonce, another 8 bytes random data
*/
netmd_error netmd_secure_session_key_exchange(netmd_dev_handle *dev,
                                              unsigned char *rand_in,
                                              unsigned char *rand_out);

/**
   Invalidate the session key established by nonce exchange. Does not invalidate
   the root key set up by sendKeyData.
*/
netmd_error netmd_secure_session_key_forget(netmd_dev_handle *dev);

/**
   Prepare the download of a music track to the device.

   @param contentid 20 bytes Unique Identifier for the DRM system.
   @param keyenckey 8 bytes DES key used to encrypt the block data keys
   @param sessionkey 8 bytes DES key used for securing the current session, the
                     key has to be calculated by the caller from the data
                     exchanged in sessionKeyExchange and the root key selected
                     by sendKeyData.
*/
netmd_error netmd_secure_setup_download(netmd_dev_handle *dev,
                                        unsigned char *contentid,
                                        unsigned char *key_encryption_key,
                                        unsigned char *sessionkey);

/**
   Send a track to the NetMD unit.

   @param wireformat Format of the packets that are transported over usb
   @param discformat Format of the song in the minidisc
   @param frames Number of frames we need to transfer. Framesize depends on the
                 wireformat.
   @param packets Linked list with all packets that are nessesary to transfer
                  the complete song.
   @param packet_count Count of the packets in the linked list.
   @param sessionkey 8 bytes DES key used for securing the current session,
   @param track Pointer to where the new track number should be written to after
                trackupload.
   @param uuid Pointer to 8 byte of memory where the uuid of the new track is
               written to after upload.
   @param content_id Pointer to 20 byte of memory where the content id of the
                     song is written to afte upload.
*/
netmd_error netmd_secure_send_track(netmd_dev_handle *dev,
                                    netmd_wireformat wireformat,
                                    unsigned char discformat,
                                    unsigned int frames,
                                    netmd_track_packets *packets,
                                    size_t packet_length,
                                    unsigned char *sessionkey,
                                    uint16_t *track, unsigned char *uuid,
                                    unsigned char *content_id);

netmd_error netmd_secure_recv_track(netmd_dev_handle *dev, uint16_t track,
                                    FILE* file);


/**
   Commit a track. The idea is that this command tells the device hat the license
   for the track has been checked out from the computer.

   @param track  Track number returned from downloading command
   @param sessionkey 8-byte DES key used for securing the download session
*/
netmd_error netmd_secure_commit_track(netmd_dev_handle *dev, uint16_t track,
                                      unsigned char *sessionkey);

/**
   Gets the DRM tracking ID for a track.
   NetMD downloaded tracks have an 8-byte identifier (instead of their content
   ID) stored on the MD medium. This is used to verify the identity of a track
   when checking in.

   @param track The track number
   @param uuid  Pointer to the memory, where the 8-byte uuid of the track sould
                be saved.
*/
netmd_error netmd_secure_get_track_uuid(netmd_dev_handle *dev, uint16_t track,
                                        unsigned char *uuid);

/**
   Secure delete with 8-byte signature?

   @param track track number to delete
   @param signature 8-byte signature of deleted track
*/
netmd_error netmd_secure_delete_track(netmd_dev_handle *dev, uint16_t track,
                                                   unsigned char *signature);

netmd_error netmd_prepare_packets(unsigned char* data, size_t data_lenght,
                                  netmd_track_packets **packets,
                                  size_t *packet_count, unsigned int *frames, size_t channels, size_t *packet_length,
                                  unsigned char *key_encryption_key, netmd_wireformat format);

void netmd_cleanup_packets(netmd_track_packets **packets);

netmd_error netmd_secure_set_track_protection(netmd_dev_handle *dev,
                                              unsigned char mode);


/**
   Get the bitrate used to encode a specific track.

   @param dev pointer to device returned by netmd_open
   @param track Zero based index of track your requesting.
   @param data pointer to store the hex code representing the bitrate.
*/
int netmd_request_track_bitrate(netmd_dev_handle*dev, const uint16_t track,
                                unsigned char* encoding, unsigned char* channel);

/**
   Get the flags used for a specific track.

   @param dev pointer to device returned by netmd_open
   @param track Zero based index of track your requesting.
   @param data pointer to store the hex code representing the codec.
*/
int netmd_request_track_flags(netmd_dev_handle* dev, const uint16_t track, unsigned char* data);

/**
   Get the title for a specific track.

   @param dev pointer to device returned by netmd_open
   @param track Zero based index of track your requesting.
   @param buffer buffer to hold the name.
   @param size of buf.
   @return Actual size of buffer, if your buffer is too small resize buffer and
           recall function.
*/
int netmd_request_title(netmd_dev_handle* dev, const uint16_t track, char* buffer, const size_t size);


typedef struct {
        unsigned char content[255];
        size_t length;
        size_t position;
} netmd_response;

#ifndef netmd_min
    #define netmd_min(a,b) ((a)<(b)?(a):(b))
#endif


#ifdef WIN32
    #include <windows.h>
    #define msleep(x) Sleep(x)
#else
    #define msleep(x) usleep(1000*x)
#endif

/**
 * union to hold data used by netmd_format_query() function
 */
typedef union {
    uint16_t u16;
    uint32_t u32;
    uint64_t u64;
    uint8_t  u8;
    uint8_t *pu8;
} netmd_query_data_store_t;

/**
 * structure to be used for data detail by netmd_format_query() function
 */
typedef struct {
    netmd_query_data_store_t data;
    size_t size;
} netmd_query_data_t;

/**
 * format place holder used in netmd_format_query()
 */
typedef enum {
    netmd_fmt_byte   = 'b',
    netmd_fmt_word   = 'w',
    netmd_fmt_dword  = 'd',
    netmd_fmt_qword  = 'q',
    netmd_fmt_barray = '*',
} netmd_format_items_t;

/**
 * help endianess helper used by netmd_format_query() function
 */
typedef enum {
    netmd_hto_littleendian = '<',
    netmd_hto_bigendian    = '>',
    netmd_no_convert
} netmd_endianess_t;

/**
 * structure to be used for capture data by netmd_scan_query() function
 */
typedef struct {
    netmd_format_items_t tp;
    netmd_query_data_store_t data;
    size_t size;
} netmd_capture_data_t;


unsigned char proper_to_bcd_single(unsigned char value);
unsigned char* proper_to_bcd(unsigned int value, unsigned char* target, size_t len);
unsigned char bcd_to_proper_single(unsigned char value);
unsigned int bcd_to_proper(unsigned char* value, size_t len);

void netmd_check_response_bulk(netmd_response *response, const unsigned char* const expected,
                               const size_t expected_length, netmd_error *error);

void netmd_check_response_word(netmd_response *response, const uint16_t expected,
                               netmd_error *error);

void netmd_check_response(netmd_response *response, const unsigned char expected,
                          netmd_error *error);

void netmd_read_response_bulk(netmd_response *response, unsigned char* target,
                              const size_t length, netmd_error *error);



unsigned char *netmd_copy_word_to_buffer(unsigned char **buf, uint16_t value, int little_endian);
unsigned char *netmd_copy_doubleword_to_buffer(unsigned char **buf, uint32_t value, int little_endian);
unsigned char *netmd_copy_quadword_to_buffer(unsigned char **buf, uint64_t value);

unsigned char netmd_read(netmd_response *response);
uint16_t netmd_read_word(netmd_response *response);
uint32_t netmd_read_doubleword(netmd_response *response);
uint64_t netmd_read_quadword(netmd_response *response);

//------------------------------------------------------------------------------
//! @brief      htons short and dirty
//!
//! @param[in]  in    value to convert
//!
//! @return     converted or original value
//------------------------------------------------------------------------------
uint16_t netmd_htons(uint16_t in);

//------------------------------------------------------------------------------
//! @brief      htonl short and dirty
//!
//! @param[in]  in    value to convert
//!
//! @return     converted or original value
//------------------------------------------------------------------------------
uint32_t netmd_htonl(uint32_t in);

//------------------------------------------------------------------------------
//! @brief      ntohs short and dirty
//!
//! @param[in]  in    value to convert
//!
//! @return     converted or original value
//------------------------------------------------------------------------------
uint16_t netmd_ntohs(uint16_t in);

//------------------------------------------------------------------------------
//! @brief      ntohl short and dirty
//!
//! @param[in]  in    value to convert
//!
//! @return     converted or original value
//------------------------------------------------------------------------------
uint32_t netmd_ntohl(uint32_t in);

//------------------------------------------------------------------------------
//! @brief      host to little endian short
//!
//! @param[in]  in    value to convert
//!
//! @return     converted or original value
//------------------------------------------------------------------------------
uint16_t netmd_htoles(uint16_t in);

//------------------------------------------------------------------------------
//! @brief      host to little endian long
//!
//! @param[in]  in    value to convert
//!
//! @return     converted or original value
//------------------------------------------------------------------------------
uint32_t netmd_htolel(uint32_t in);

//------------------------------------------------------------------------------
//! @brief      host to little endian long long
//!
//! @param[in]  in    value to convert
//!
//! @return     converted or original value
//------------------------------------------------------------------------------
uint64_t netmd_htolell(uint64_t in);

//------------------------------------------------------------------------------
//! @brief      host to big endian long long
//!
//! @param[in]  in    value to convert
//!
//! @return     converted or original value
//------------------------------------------------------------------------------
uint64_t netmd_htonll(uint64_t in);

//------------------------------------------------------------------------------
//! @brief      little to host endian short
//!
//! @param[in]  in    value to convert
//!
//! @return     converted or original value
//------------------------------------------------------------------------------
uint16_t netmd_letohs(uint16_t in);

//------------------------------------------------------------------------------
//! @brief      little endian to host long
//!
//! @param[in]  in    value to convert
//!
//! @return     converted or original value
//------------------------------------------------------------------------------
uint32_t netmd_letohl(uint32_t in);

//------------------------------------------------------------------------------
//! @brief      little endian to host long long
//!
//! @param[in]  in    value to convert
//!
//! @return     converted or original value
//------------------------------------------------------------------------------
uint64_t netmd_letohll(uint64_t in);

//------------------------------------------------------------------------------
//! @brief     calculate NetMD checksum
//!
//! @param[in] data pointer to data
//! @param[in] size data size
//!
//! @return    checksum
//------------------------------------------------------------------------------
unsigned int netmd_calculate_checksum(unsigned char data[], size_t size);

//------------------------------------------------------------------------------
//! @brief      format a netmd device query
//!
//! @param[in]  format    format string
//! @param[in]  argv      arguments array
//! @param[in]  argc      argument count
//! @param[in]  query_sz  buffer for query size
//!
//! @return     formatted byte array (you MUST call free() if not NULL);
//!             NULL on error
//------------------------------------------------------------------------------
uint8_t* netmd_format_query(const char* format, const netmd_query_data_t argv[], int argc, size_t* query_sz);

//------------------------------------------------------------------------------
//! @brief      scan data for format options
//!
//! @param[in]  data      byte array to scan
//! @param[in]  size      data size
//! @param[in]  format    format string
//! @param[out] argv      buffer pointer for argument array
//!                       (call free() if not NULL!)
//!                       Note: If stored data is an byte array,
//!                       you have to free it using free() as well!
//! @param[out] argc      pointer to argument count
//!
//! @return     0 -> ok; -1 -> error
//------------------------------------------------------------------------------
int netmd_scan_query(const uint8_t data[], size_t size, const char* format, netmd_capture_data_t** argv, int* argc);

//------------------------------------------------------------------------------
//! @brief      prepare AUDIO for SP upload
//!
//! @param[in/out]  audio_data audio data (on out: must be freed afterwards)
//! @param[in/out]  data_size  size of audio data
//!
//! @return     netmd_error
//! @see        netmd_error
//------------------------------------------------------------------------------
netmd_error netmd_prepare_audio_sp_upload(uint8_t** audio_data, size_t* data_size);


typedef struct {
    uint16_t hour;
    uint8_t minute;
    uint8_t second;
    uint8_t frame;
} netmd_time;

/**
   Structure to hold the capacity information of a disc.
*/
typedef struct {
    /** Time allready recorded on the disc. */
    netmd_time recorded;

    /** Total time, that could be recorded on the disc. This depends on the
       current recording settings. */
    netmd_time total;

    /** Time that is available on the disc. This depends on the current
       recording settings. */
    netmd_time available;
} netmd_disc_capacity;

/**
   Starts playing the current track. If no track is selected, starts playing the
   first track.

   @param dev Handle to the open minidisc player.
*/
netmd_error netmd_play(netmd_dev_handle* dev);

/**
   Pause playing. If uses netmd_play afterwards the player continues at the
   current position.

   @param dev Handle to the open minidisc player.
*/
netmd_error netmd_pause(netmd_dev_handle* dev);

/**
   Spin the track fast forward.

   @param dev Handle to the open minidisc player.
*/
netmd_error netmd_fast_forward(netmd_dev_handle* dev);

/**
   Spin the track backwards in time (aka rewind it).

   @param dev Handle to the open minidisc player.
*/
netmd_error netmd_rewind(netmd_dev_handle* dev);

/**
   Stop playing. The current position is discarded.

   @param dev Handle to the open minidisc player.
*/
netmd_error netmd_stop(netmd_dev_handle* dev);

/**
   Set the playmode.

   @param dev Handle to the open minidisc player.
   @param playmode Playmode to set. Could be a OR'ed combination of the
                   corresponding defines from const.h.
   @see NETMD_PLAYMODE_SINGLE
   @see NETMD_PLAYMODE_REPEAT
   @see NETMD_PLAYMODE_SHUFFLE

*/
netmd_error netmd_set_playmode(netmd_dev_handle* dev, const uint16_t playmode);

/**
   Jump to the given track.

   @param dev Handle to the open minidisc player.
   @param track Number of the track to jump to.
*/
netmd_error netmd_set_track(netmd_dev_handle* dev, const uint16_t track);

/**
   Jump to the next track. If you currently playing the last track, nothing
   happens.

   @param dev Handle to the open minidisc player.
*/
netmd_error netmd_track_next(netmd_dev_handle* dev);

/**
   Jump to the previous track. If you currently playing the first track, nothing
   happens.

   @param dev Handle to the open minidisc player.
*/
netmd_error netmd_track_previous(netmd_dev_handle* dev);

/**
   Jump to the beginning of the current track.

   @param dev Handle to the open minidisc player.
*/
netmd_error netmd_track_restart(netmd_dev_handle* dev);

/**
   Jump to a specific time of the given track.

   @param dev Handle to the open minidisc player.
   @param track Track, where to jump to the given time.
   @param time Time to jump to.
*/
netmd_error netmd_set_time(netmd_dev_handle* dev, const uint16_t track,
                           const netmd_time* time);

/**
   Gets the currently playing track.

   @param dev Handle to the open minidisc player.
   @param track Pointer where to save the current track.
*/
netmd_error netmd_get_track(netmd_dev_handle* dev, uint16_t *track);

/**
   Gets the position within the currently playing track

   @param dev Handle to the open minidisc player.
   @param time Pointer to save the current time to.
*/
netmd_error netmd_get_position(netmd_dev_handle* dev, netmd_time* time);

/**
   Gets the used, total and available disc capacity (total and available
   capacity depend on current recording settings)

   @param dev Handle to the open minidisc player.
   @param capacity Pointer to a netmd_disc_capacity structure to save the
                   capacity information of the current minidisc to.
*/
netmd_error netmd_get_disc_capacity(netmd_dev_handle* dev,
                                    netmd_disc_capacity* capacity);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* LIBNETMD_H */
