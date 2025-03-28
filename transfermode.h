/**
 * Copyright (C) 2025 Jo2003 (olenka.joerg@gmail.com)
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
#include <cstdint>
#include "cnetmd.h"
#include "cxenc.h"

/**
 * @brief The TransferMode class
 */
class TransferMode
{
public:
    using XEncCmd  = CXEnc::XEncCmd;
    using NetMDCmd = CNetMD::NetMDCmd;

    enum ETransferMode
    {
        TM_TAO_START,
        TM_TAO_SP,
        TM_TAO_SP_MONO,
        TM_TAO_LP2,
        TM_TAO_LP4,
        TM_TAO_END,
        TM_DAO_START,
        TM_DAO_SP,
        TM_DAO_SP_MONO,
        TM_DAO_SP_PREENC,
        TM_DAO_LP2,
        TM_DAO_LP4,
        TM_DAO_END,
        TM_UNKNOWN
    };

    struct TFModeDescr
    {
        ETransferMode mMode;
        const char*   mName;
        const char*   mTrackMode;
        const char*   mIconSrc;
        int           mMulti;
        XEncCmd       mEncCmd;
        NetMDCmd      mNetMDCmd;
        XEncCmd       mEncCmdOtf;
        NetMDCmd      mNetMDCmdOtf;
    };

    /**
     * @brief TransferMode
     * @param m
     */
    TransferMode(ETransferMode m = ETransferMode::TM_UNKNOWN)
        : mTransferMode(m)
    {
    }

    /**
     * @brief operator ETransferMode
     */
    operator ETransferMode() const
    {
        return mTransferMode;
    }

    /**
     * @brief operator =
     * @param other
     * @return *this
     */
    TransferMode& operator=(ETransferMode other)
    {
        mTransferMode = other;
        return *this;
    }

    /**
     * @brief operator =
     * @param other
     * @return *this
     */
    TransferMode& operator=(int other)
    {
        mTransferMode = static_cast<ETransferMode>(other);
        return *this;
    }

    /**
     * @brief isDao
     * @return true if DAO
     */
    bool isDao() const
    {
        return ((mTransferMode > TM_DAO_START) && (mTransferMode < TM_DAO_END));
    }

    /**
     * @brief isTao
     * @return true if TAO
     */
    bool isTao() const
    {
        return ((mTransferMode > TM_TAO_START) && (mTransferMode < TM_TAO_END));
    }

    /**
     * @brief isValid
     * @return true if valid
     */
    bool isValid() const
    {
        return (isDao() || isTao());
    }

    /**
     * @brief tokManip
     * @return true if TOC edit is needed
     */
    bool tokManip() const
    {
        return ((mTransferMode == TM_DAO_SP) || (mTransferMode == TM_DAO_SP_MONO) || (mTransferMode == TM_DAO_SP_PREENC));
    }

    /**
     * @brief name
     * @return mode name or nullptr
     */
    const char* name() const
    {
        if (const auto* m = find(mTransferMode))
        {
            return m->mName;
        }
        return nullptr;
    }

    /**
     * @brief trackMode
     * @return track mode or nullptr
     */
    const char* trackMode() const
    {
        if (const auto* m = find(mTransferMode))
        {
            return m->mTrackMode;
        }
        return nullptr;
    }

    /**
     * @brief iconSrc
     * @return icon source or nullptr
     */
    const char* iconSrc() const
    {
        if (const auto* m = find(mTransferMode))
        {
            return m->mIconSrc;
        }
        return nullptr;
    }

    /**
     * @brief multi
     * @return time multiplicator
     */
    int multi() const
    {
        if (const auto* m = find(mTransferMode))
        {
            return m->mMulti;
        }
        return 1;
    }

    /**
     * @brief netMDCmd
     * @param otf
     * @return NetMd command
     */
    NetMDCmd netMDCmd(bool otf) const
    {
        if (const auto* m = find(mTransferMode))
        {
            return otf ? m->mNetMDCmdOtf : m->mNetMDCmd;
        }
        return NetMDCmd::UNKNWON;
    }

    /**
     * @brief xencCmd
     * @param otf
     * @return command for the external encoder
     */
    XEncCmd xencCmd(bool otf) const
    {
        if (const auto* m = find(mTransferMode))
        {
            return otf ? m->mEncCmdOtf : m->mEncCmd;
        }
        return XEncCmd::NONE;
    }

protected:

    /**
     * @brief find
     * @param m
     * @return transfer mode info or nullptr
     */
    const TFModeDescr* find(ETransferMode m) const
    {
        for (int i = 0; i < VALID_TF_COUNT; i++)
        {
            if (mModeMap[i].mMode == m)
            {
                return &mModeMap[i];
            }
        }
        return nullptr;
    }

private:
    ETransferMode mTransferMode;
    static constexpr int VALID_TF_COUNT = 9;

    const TFModeDescr mModeMap[VALID_TF_COUNT] = {
        { ETransferMode::TM_TAO_SP       , "SP"       , "SP"     , ":/view/audio", 1, XEncCmd::NONE          , NetMDCmd::WRITE_TRACK_SP       , XEncCmd::NONE          , NetMDCmd::WRITE_TRACK_SP        },
        { ETransferMode::TM_TAO_SP_MONO  , "SP Mono"  , "SP Mono", ":/view/audio", 2, XEncCmd::NONE          , NetMDCmd::WRITE_TRACK_SP_MONO  , XEncCmd::NONE          , NetMDCmd::WRITE_TRACK_SP_MONO   },
        { ETransferMode::TM_TAO_LP2      , "LP2"      , "LP2"    , ":/view/audio", 2, XEncCmd::LP2_ENCODE    , NetMDCmd::WRITE_TRACK_SP       , XEncCmd::NONE          , NetMDCmd::WRITE_TRACK_LP2       },
        { ETransferMode::TM_TAO_LP4      , "LP4"      , "LP4"    , ":/view/audio", 4, XEncCmd::LP4_ENCODE    , NetMDCmd::WRITE_TRACK_SP       , XEncCmd::NONE          , NetMDCmd::WRITE_TRACK_LP4       },
        { ETransferMode::TM_DAO_SP       , "SP"       , "SP"     , ":/buttons/cd", 1, XEncCmd::NONE          , NetMDCmd::WRITE_TRACK_SP       , XEncCmd::NONE          , NetMDCmd::WRITE_TRACK_SP        },
        { ETransferMode::TM_DAO_SP_MONO  , "SP Mono"  , "SP Mono", ":/buttons/cd", 2, XEncCmd::NONE          , NetMDCmd::WRITE_TRACK_SP_MONO  , XEncCmd::NONE          , NetMDCmd::WRITE_TRACK_SP_MONO   },
        { ETransferMode::TM_DAO_SP_PREENC, "SP Preenc", "SP"     , ":/buttons/cd", 1, XEncCmd::DAO_SP_ENCODE , NetMDCmd::WRITE_TRACK_SP_PREENC, XEncCmd::DAO_SP_ENCODE , NetMDCmd::WRITE_TRACK_SP_PREENC },
        { ETransferMode::TM_DAO_LP2      , "LP2"      , "LP2"    , ":/buttons/cd", 2, XEncCmd::DAO_LP2_ENCODE, NetMDCmd::WRITE_TRACK_SP       , XEncCmd::DAO_LP2_ENCODE, NetMDCmd::WRITE_TRACK_SP        },
        { ETransferMode::TM_DAO_LP4      , "LP4"      , "LP4"    , ":/buttons/cd", 4, XEncCmd::DAO_LP4_ENCODE, NetMDCmd::WRITE_TRACK_SP       , XEncCmd::DAO_LP4_ENCODE, NetMDCmd::WRITE_TRACK_SP        },
    };
};
