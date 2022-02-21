/**
 * Copyright (C) 2021 Jo2003 (olenka.joerg@gmail.com)
 * This file is part of cd2netmd_gui
 *
 * cd2netmd is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * cd2netmd is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 */
#pragma once
#include <QObject>
#include <QNetworkAccessManager>
#include "defines.h"

//------------------------------------------------------------------------------
//! @brief      This class describes a CDDB helper..
//------------------------------------------------------------------------------
class CCDDB : public QObject
{
    Q_OBJECT
    static constexpr const char* CDDB_SERVER = "https://gnudb.gnudb.org:443";

public:
    /// request type
    enum class REQ_WHAT : uint8_t {
        REQ_UNKNOWN,    ///< unknown request
        REQ_ENTRIES,    ///< request all entries
        REQ_MATCH       ///< request matching entries
    };

    //--------------------------------------------------------------------------
    //! @brief      Constructs a new instance.
    //!
    //! @param      parent  The parent object
    //--------------------------------------------------------------------------
    explicit CCDDB(QObject *parent = nullptr);

    //--------------------------------------------------------------------------
    //! @brief      Gets the entries.
    //!
    //! @param[in]  queryPart  The query part
    //! @param[in]  tracks     The prepared audio tracks
    //!
    //! @return     0 -> ok; -1 -> error
    //--------------------------------------------------------------------------
    int getEntries(const QString& queryPart, c2n::AudioTracks& tracks);
    
    //--------------------------------------------------------------------------
    //! @brief      Gets the matching entry.
    //!
    //! @param[in]  queryPart  The query part
    //!
    //! @return     0 -> ok; -1 -> error
    //--------------------------------------------------------------------------
    int getEntry(const QString& queryPart);

private slots:
    //--------------------------------------------------------------------------
    //! @brief      get server response
    //!
    //! @param      reply  The reply
    //!
    //! @return     0 -> ok; -1 -> error
    //--------------------------------------------------------------------------
    int catchResponse(QNetworkReply *reply);

protected:

    //--------------------------------------------------------------------------
    //! @brief      parse the network reply
    //!
    //! @param[in]  type   The type
    //! @param[in]  reply  The reply
    //!
    //! @return     0 -> ok; -1 -> error
    //--------------------------------------------------------------------------
    int parseReply(REQ_WHAT type, QString reply);
    
    QNetworkAccessManager *mpNetwork;

signals:
    void entries(QStringList l);
    void match(c2n::AudioTracks tracks);

private:
    c2n::AudioTracks mAudioTracks;
};

