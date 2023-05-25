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
#include <QByteArray>
#include <sstream>
#include "ctocmanip.h"
#include "defines.h"

using namespace netmd;

//--------------------------------------------------------------------------
//! @brief      constructs the object
//!
//! @param[in]  api    NetMD API pointer
//--------------------------------------------------------------------------
CTocManip::CTocManip(netmd_pp* api)
    : mpApi(api)
{
}

//------------------------------------------------------------------------------
//! @brief      add bytes to byte vector
//!
//! @param      vec     The vector
//! @param[in]  data    The data
//! @param[in]  dataSz  The data size
//------------------------------------------------------------------------------
void CTocManip::addArrayData(NetMDByteVector& vec, const char* data, size_t dataSz)
{
    for (size_t i = 0; i < dataSz; i++)
    {
        vec.push_back(static_cast<uint8_t>(data[i]));
    }
}

//--------------------------------------------------------------------------
//! @brief      do the TOC manipulation
//!
//! @param[in]  trackData list of track titles and lengths
//!
//! @return     NetMdErr
//--------------------------------------------------------------------------
int CTocManip::manipulateTOC(const TitleVector &trackData)
{
    if (!mpApi)
    {
        qCritical() << "NetMD Api not initialized!";
        return -1;
    }

    int ret = mpApi->prepareTOCManip();

    if (ret == NETMDERR_NO_ERROR)
    {
        NetMDByteVector toc;
        QByteArray tocData;
        std::size_t i;
        for (i = 0; i < 3; i++)
        {
            toc += mpApi->readUTOCSector(static_cast<UTOCSector>(i));
            if (toc.size() != ((i + 1) * 2352))
            {
                qCritical() << "Error: Can't read UTOC sector" << i;
                break;
            }
        }

        if (toc.size() == 2352 * 3)
        {
            for (i = 0; i < toc.size(); i++)
            {
                tocData.append(static_cast<char>(toc.at(i)));
            }

            CNetMdTOC tocHlp(trackData.size() - 1, trackData.at(0).mLengthInMs, reinterpret_cast<uint8_t*>(tocData.data()));
            std::ostringstream oss;

            oss << std::endl
                << "Read Disc Layout:" << std::endl
                << "=================" << std::endl
                << tocHlp.discInfo() << std::endl;
            for(int j = 1; j <= tocHlp.trackCount(); j++)
            {
                oss << tocHlp.trackInfo(j);
            }

            for (int j = 1; j < trackData.size(); j++)
            {
                tocHlp.addTrack(j, trackData.at(j).mLengthInMs, trackData.at(j).mName.toStdString());
            }
            tocHlp.setDiscTitle(trackData.at(0).mName.toStdString());

            oss << "Finished Disc Layout:" << std::endl
                << "=====================" << std::endl
                << tocHlp.discInfo() << std::endl;
            for(int j = 1; j <= tocHlp.trackCount(); j++)
            {
                oss << tocHlp.trackInfo(j);
            }

            qInfo() << oss.str().c_str();

            bool good = true;
            for (i = 0; i < 3; i++)
            {
                toc.clear();
                addArrayData(toc, tocData.constData() + i * 2352, 2352);
                if (mpApi->writeUTOCSector(static_cast<UTOCSector>(i), toc) == NETMDERR_NO_ERROR)
                {
                    qDebug() << "TOC sector" << i << "written!";
                }
                else
                {
                    qWarning() << "Can't write TOC sector" << i << "!";
                    good = false;
                }
            }

            if (good)
            {
                return mpApi->finalizeTOC();
            }
            else
            {
                ret = NETMDERR_USB;
            }
        }
        else
        {
            ret = NETMDERR_USB;
        }
    }

    return ret;
}
