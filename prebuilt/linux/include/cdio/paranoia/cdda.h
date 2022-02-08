/*
  Copyright (C) 2004, 2005, 2006, 2008, 2011, 2012
    Rocky Bernstein <rocky@gnu.org>
  Copyright (C) 2014 Robert Kausch <robert.kausch@freac.org>
  Copyright (C) 2001 Xiph.org and Heiko Eissfeldt heiko@escape.colossus.de

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/** \file cdda.h
 *
 *  \brief The top-level interface header for libcdio_cdda.
 *  Applications include this for paranoia access.
 *
 ******************************************************************/

#ifndef CDIO__PARANOIA__CDDA_H_
#define CDIO__PARANOIA__CDDA_H_

#include <cdio/cdio.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

  /** cdrom_paranoia is an opaque structure which is used in all of the
      library operations.
  */
  typedef struct cdrom_paranoia_s cdrom_paranoia_t;
  typedef struct cdrom_drive_s   cdrom_drive_t;

  /** For compatibility. cdrom_drive_t is deprecated, use cdrom_drive_t
      instead. */

  /**
     Flags for simulating jitter used in testing.

     The enumeration type one probably wouldn't really use in a program.
     It is here instead of defines to give symbolic names that can be
     helpful in debuggers where wants just to say refer to
     CDDA_TEST_JITTER_SMALL and get the correct value.
  */
  typedef enum {
    CDDA_MESSAGE_FORGETIT = 0,
    CDDA_MESSAGE_PRINTIT  = 1,
    CDDA_MESSAGE_LOGIT    = 2,
    CD_FRAMESAMPLES       = CDIO_CD_FRAMESIZE_RAW / 4,
    MAXTRK                = (CDIO_CD_MAX_TRACKS+1)
  } paranoia_cdda_enums_t;


#include <signal.h>

/** We keep MAXTRK since this header is exposed publicly and other
   programs may have used this.
*/
#define MAXTRK (CDIO_CD_MAX_TRACKS+1)

/** \brief Structure for cdparanoia's CD Table of Contents */
typedef struct TOC_s {
  unsigned char bTrack;
  int32_t       dwStartSector;
} TOC_t;

/** For compatibility. TOC is deprecated, use TOC_t instead. */
#define TOC TOC_t

/** \brief Structure for cdparanoia's CD-ROM access */
struct cdrom_drive_s {

  CdIo_t *p_cdio;
  int opened; /**< This struct may just represent a candidate for opening */

  char *cdda_device_name;

  char *drive_model;
  int drive_type;
  int bigendianp; /**< Whether data returned on the CDDA is bigendian or
		       not. 1 if big endian, 0 if little endian and -1 if
		       we don't know.
		   */
  int nsectors;   /**< Number of sectors use in reading. Multiply by
		   CDIO_CD_FRAMESIZE_RAW to get number of bytes used in
		  the read buffer. */

  int cd_extra;   /**< -1 if we can't get multisession info, 0 if
                       there is one session only or the multi-session
                       LBA is less than or 100 (don't ask me why -- I
                       don't know), and 1 if the multi-session lba is
                       greater than 100. */

  bool b_swap_bytes;  /**< Swap bytes if Endian-ness of drive
			   mismatches the endian-ness of the
			   computer? */
  track_t tracks;
  TOC_t disc_toc[MAXTRK]; /**< info here starts origin 0 rather than the
			     first track number (usually 1). So to take
			     a track number and use it here, subtract
			     off cdio_get_first_track_num() beforehand.
			   */
  lsn_t audio_first_sector;
  lsn_t audio_last_sector;

  int errordest;
  int messagedest;
  char *errorbuf;
  char *messagebuf;

  /* functions specific to particular drives/interfaces */

  int  (*enable_cdda)  (cdrom_drive_t *d, int onoff);
  int  (*read_toc)     (cdrom_drive_t *d);
  long (*read_audio)   (cdrom_drive_t *d, void *p, lsn_t begin,
		       long sectors);
  int  (*set_speed)    (cdrom_drive_t *d, int speed);
  int error_retry;
  int report_all;

  int is_atapi;
  int is_mmc;

  int last_milliseconds;

  int i_test_flags; /**< Normally set 0. But if we are testing
		       paranoia operation this can be set to one of
		       the flag masks to simulate a particular kind of
		       failure.    */

};


  /**
     Flags for simulating jitter used in testing.

     The enumeration type one probably wouldn't really use in a program.
     It is here instead of defines to give symbolic names that can be
     helpful in debuggers where wants just to say refer to
     CDDA_TEST_JITTER_SMALL and get the correct value.
  */
  typedef enum {
    CDDA_TEST_JITTER_SMALL   = 1,
    CDDA_TEST_JITTER_LARGE   = 2,
    CDDA_TEST_JITTER_MASSIVE = 3,
    CDDA_TEST_FRAG_SMALL     = (1<<3),
    CDDA_TEST_FRAG_LARGE     = (2<<3),
    CDDA_TEST_FRAG_MASSIVE   = (3<<3),
    CDDA_TEST_UNDERRUN       = 64
  } paranoia_jitter_t;

/** jitter testing. The first two bits are set to determine the
     byte-distance we will jitter the data; 0 is no shifting.
 */

/**< jitter testing. Set the below bit to always cause jittering on reads.
     The below bit only has any effect if the first two (above) bits are
     nonzero. If the above bits are set, but the below bit isn't we'll
     jitter 90% of the time.
  */
#define CDDA_TEST_ALWAYS_JITTER     4

/** fragment testing */
#define CDDA_TEST_FRAG_SMALL   (1<<3)
#define CDDA_TEST_FRAG_LARGE   (2<<3)
#define CDDA_TEST_FRAG_MASSIVE (3<<3)

/**< under-run testing. The below bit is set for testing.  */
#define CDDA_TEST_UNDERRUN         64

#if TESTING_IS_FINISHED

 /** scratch testing */
#define CDDA_TEST_SCRATCH         128
#undef  CDDA_TEST_BOGUS_BYTES     256
#undef  CDDA_TEST_DROPDUPE_BYTES  512
#endif /* TESTING_IS_FINISHED */

/** autosense functions */

/** Get a CD-ROM drive with a CD-DA in it.
    If mesagedest is CDDA_MESSAGE_LOGIT, then any messages in the
    process will be stored in message.

    When using CDDA_MESSAGE_LOGIT, free the message buffer with
    cdio_cddap_free_messages() after use.
*/
extern cdrom_drive_t *cdio_cddap_find_a_cdrom(int messagedest,
					      char **ppsz_message);

/** Returns a paranoia CD-ROM drive object with a CD-DA in it or NULL
    if there was an error.
    @see cdio_cddap_identify_cdio

    When using CDDA_MESSAGE_LOGIT, free the message buffer with
    cdio_cddap_free_messages() after use.
 */
extern cdrom_drive_t *cdio_cddap_identify(const char *psz_device,
					  int messagedest,
					  char **ppsz_message);

/** Returns a paranoia CD-ROM drive object with a CD-DA in it or NULL
    if there was an error.  In contrast to cdio_cddap_identify, we
    start out with an initialized p_cdio object. For example you may
    have used that for other purposes such as to get CDDB/CD-Text
    information.  @see cdio_cddap_identify

    When using CDDA_MESSAGE_LOGIT, free the message buffer with
    cdio_cddap_free_messages() after use.
 */
cdrom_drive_t *cdio_cddap_identify_cdio(CdIo_t *p_cdio,
					int messagedest, char **ppsz_messages);

/** informational functions */

extern const char *cdio_cddap_version();

/** Returns the current message buffer. Free the returned
    string using cdio_cddap_free_messages() if not NULL.
 */
extern char   *cdio_cddap_messages(cdrom_drive_t *d);

/** Returns the current error buffer. Free the returned
    string using cdio_cddap_free_messages() if not NULL.
 */
extern char   *cdio_cddap_errors(cdrom_drive_t *d);

/** Frees the message string passed in psz_messages.
 */
extern void    cdio_cddap_free_messages(char *psz_messages);

/** drive-oriented functions */

extern int     cdio_cddap_speed_set(cdrom_drive_t *d, int speed);
extern void    cdio_cddap_verbose_set(cdrom_drive_t *d, int err_action,
				      int mes_action);

/*!
  Closes d and releases all storage associated with it except
  the internal p_cdio pointer.

  @param d cdrom_drive_t object to be closed.
  @return 0 if passed a null pointer and 1 if not in which case
  some work was probably done.

  @see cdio_cddap_close
*/
bool cdio_cddap_close_no_free_cdio(cdrom_drive_t *d);

/*!
  Closes d and releases all storage associated with it.
  Doubles as "cdrom_drive_free()".

  @param d cdrom_drive_t object to be closed.
  @return 0 if passed a null pointer and 1 if not in which case
  some work was probably done.

  @see cdio_cddap_close_no_free_cdio
*/
extern int     cdio_cddap_close(cdrom_drive_t *d);

extern int     cdio_cddap_open(cdrom_drive_t *d);

extern long    cdio_cddap_read(cdrom_drive_t *d, void *p_buffer,
			       lsn_t beginsector, long sectors);

extern long    cdio_cddap_read_timed(cdrom_drive_t *d, void *p_buffer,
				     lsn_t beginsector, long sectors, int *milliseconds);

/*! Return the lsn for the start of track i_track */
extern lsn_t   cdio_cddap_track_firstsector(cdrom_drive_t *d,
				      track_t i_track);

/*! Get last lsn of the track. This generally one less than the start
  of the next track. -1 is returned on error. */
extern lsn_t   cdio_cddap_track_lastsector(cdrom_drive_t *d, track_t i_track);

/*! Return the number of tracks on the CD. */
extern track_t cdio_cddap_tracks(cdrom_drive_t *d);

/*! Return the track containing the given LSN. If the LSN is before
    the first track (in the pregap), 0 is returned. If there was an
    error or the LSN after the LEADOUT (beyond the end of the CD), then
    CDIO_INVALID_TRACK is returned.
 */
extern int     cdio_cddap_sector_gettrack(cdrom_drive_t *d, lsn_t lsn);

/*! Return the number of channels in track: 2 or 4; -2 if not
  implemented or -1 for error.
  Not meaningful if track is not an audio track.
*/
extern int     cdio_cddap_track_channels(cdrom_drive_t *d, track_t i_track);

/*! Return 1 is track is an audio track, 0 otherwise. */
extern int     cdio_cddap_track_audiop(cdrom_drive_t *d, track_t i_track);

/*! Return 1 is track has copy permit set, 0 otherwise. */
extern int     cdio_cddap_track_copyp(cdrom_drive_t *d, track_t i_track);

/*! Return 1 is audio track has linear preemphasis set, 0 otherwise.
    Only makes sense for audio tracks.
 */
extern int     cdio_cddap_track_preemp(cdrom_drive_t *d, track_t i_track);

/*! Get first lsn of the first audio track. -1 is returned on error. */
extern lsn_t   cdio_cddap_disc_firstsector(cdrom_drive_t *d);

/*! Get last lsn of the last audio track. The last lsn is generally one
  less than the start of the next track after the audio track. -1 is
  returned on error. */
extern lsn_t   cdio_cddap_disc_lastsector(cdrom_drive_t *d);

/*! Determine Endian-ness of the CD-drive based on reading data from
  it. Some drives return audio data Big Endian while some (most)
  return data Little Endian. Drives known to return data bigendian are
  SCSI drives from Kodak, Ricoh, HP, Philips, Plasmon, Grundig
  CDR100IPW, and Mitsumi CD-R. ATAPI and MMC drives are little endian.

  rocky: As someone who didn't write the code, I have to say this is
  nothing less than brilliant. An FFT is done both ways and the the
  transform is looked at to see which has data in the FFT (or audible)
  portion. (Or so that's how I understand it.)

  @return 1 if big-endian, 0 if little-endian, -1 if we couldn't
  figure things out or some error.
 */
extern int data_bigendianp(cdrom_drive_t *d);

/** transport errors: */

typedef enum {
  TR_OK =            0,
  TR_EWRITE =        1  /**< Error writing packet command (transport) */,
  TR_EREAD =         2  /**< Error reading packet data (transport) */,
  TR_UNDERRUN =      3  /**< Read underrun */,
  TR_OVERRUN =       4  /**< Read overrun */,
  TR_ILLEGAL =       5  /**< Illegal/rejected request */,
  TR_MEDIUM =        6  /**< Medium error */,
  TR_BUSY =          7  /**< Device busy */,
  TR_NOTREADY =      8  /**< Device not ready */,
  TR_FAULT =         9  /**< Device failure */,
  TR_UNKNOWN =      10  /**< Unspecified error */,
  TR_STREAMING =    11  /**< loss of streaming */,
} transport_error_t;


#ifdef NEED_STRERROR_TR
const char *strerror_tr[]={
  "Success",
  "Error writing packet command to device",
  "Error reading command from device",
  "SCSI packet data underrun (too little data)",
  "SCSI packet data overrun (too much data)",
  "Illegal SCSI request (rejected by target)",
  "Medium reading data from medium",
  "Device busy",
  "Device not ready",
  "Target hardware fault",
  "Unspecified error",
  "Drive lost streaming"
};
#endif /*NEED_STERROR_TR*/

/** Errors returned by lib:

\verbatim
001: Unable to set CDROM to read audio mode
002: Unable to read table of contents lead-out
003: CDROM reporting illegal number of tracks
004: Unable to read table of contents header
005: Unable to read table of contents entry
006: Could not read any data from drive
007: Unknown, unrecoverable error reading data
008: Unable to identify CDROM model
009: CDROM reporting illegal table of contents
010: Unaddressable sector

100: Interface not supported
101: Drive is neither a CDROM nor a WORM device
102: Permision denied on cdrom (ioctl) device
103: Permision denied on cdrom (data) device

300: Kernel memory error

400: Device not open
401: Invalid track number
402: Track not audio data
403: No audio tracks on disc
404: No medium present
405: Option not supported by drive
\endverbatim

*/

#ifndef DO_NOT_WANT_PARANOIA_COMPATIBILITY
/** For compatibility with good ol' paranoia */
#define cdda_find_a_cdrom       cdio_cddap_find_a_cdrom
#define cdda_identify           cdio_cddap_identify
#define cdda_version            cdio_cddap_version
#define cdda_speed_set          cdio_cddap_speed_set
#define cdda_verbose_set        cdio_cddap_verbose_set
#define cdda_messages           cdio_cddap_messages
#define cdda_errors             cdio_cddap_errors
#define cdda_close              cdio_cddap_close
#define cdda_open               cdio_cddap_open
#define cdda_read               cdio_cddap_read
#define cdda_read_timed         cdio_cddap_read_timed
#define cdda_track_firstsector  cdio_cddap_track_firstsector
#define cdda_track_lastsector   cdio_cddap_track_lastsector
#define cdda_tracks             cdio_cddap_tracks
#define cdda_sector_gettrack    cdio_cddap_sector_gettrack
#define cdda_track_channels     cdio_cddap_track_channels
#define cdda_track_audiop       cdio_cddap_track_audiop
#define cdda_track_copyp        cdio_cddap_track_copyp
#define cdda_track_preemp       cdio_cddap_track_preemp
#define cdda_disc_firstsector   cdio_cddap_disc_firstsector
#define cdda_disc_lastsector    cdio_cddap_disc_lastsector
#define cdrom_drive             cdrom_drive_t

#endif /*DO_NOT_WANT_PARANOIA_COMPATIBILITY*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

/** The below variables are trickery to force the above enum symbol
    values to be recorded in debug symbol tables. They are used to
    allow one to refer to the enumeration value names in the typedefs
    above in a debugger and debugger expressions
*/

extern paranoia_jitter_t     debug_paranoia_jitter;
extern paranoia_cdda_enums_t debug_paranoia_cdda_enums;

#ifdef __cplusplus
extern "C" {
#endif
  extern const char *cdio_cddap_version(void);
  extern void cdio_cddap_free_messages(char *psz_messages);

#ifdef __cplusplus
}
#endif

#endif /*CDIO__PARANOIA__CDDA_H_*/
