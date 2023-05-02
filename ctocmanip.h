/**
 * Copyright (C) 2023 Jo2003 (olenka.joerg@gmail.com)
 * This file is part of NetMD Wizard
 *
 * NetMD Wizard is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * NetMD Wizard is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 */
#pragma once
#include <QObject>
#include <netmd++.h>
#include <QVector>

//------------------------------------------------------------------------------
//! @brief      This class helps with TOC manipulation.
//------------------------------------------------------------------------------
class CTocManip : public QObject
{
    Q_OBJECT
public:

    //--------------------------------------------------------------------------
    //! @brief      title description
    //--------------------------------------------------------------------------
    struct TitleDescr
    {
        QString mName;          //!< track / disc name
        uint32_t mLengthInMs;   //!< track- or disc length in milli seconds
    };

    //--------------------------------------------------------------------------
    //! @brief      disc titles with length, index 0 is the disc entry
    //--------------------------------------------------------------------------
    using TitleVector = QVector<TitleDescr>;

    //--------------------------------------------------------------------------
    //! @brief      constructs the object
    //!
    //! @param parent[in] parent pointer
    //! @param api[in]    NetMD API pointer
    //--------------------------------------------------------------------------
    explicit CTocManip(QObject *parent = nullptr, netmd::netmd_pp* api = nullptr);

    //--------------------------------------------------------------------------
    //! @brief      do the TOC manipulation
    //!
    //! @param[in]  trackData list of track titles and lengths
    //!
    //! @return     NetMdErr
    //--------------------------------------------------------------------------
    int manipulateTOC(const TitleVector& trackData);

    //--------------------------------------------------------------------------
    //! @brief      add data to byte vector
    //!
    //! @param[in]  vec byte vectot to append data
    //! @param[in]  data data pointer to data to add
    //! @param[in]  dataSz size of data to copy
    //--------------------------------------------------------------------------
    static void addArrayData(netmd::NetMDByteVector &vec, const char *data, size_t dataSz);

private:
    /// net md api pointer
    netmd::netmd_pp* mpApi;
};
