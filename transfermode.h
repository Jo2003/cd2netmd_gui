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

//------------------------------------------------------------------------------
//! @brief      This class describes a transfer mode.
//------------------------------------------------------------------------------
class TransferMode
{
public:
    using XEncCmd    = CXEnc::XEncCmd;
    using NetMDCmd   = CNetMD::NetMDCmd;
    using HBFeatures = netmd::HomebrewFeatures;

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

    //--------------------------------------------------------------------------
    //! @brief      Constructs a new instance.
    //!
    //! @param[in]  m     transfer mode value
    //--------------------------------------------------------------------------
    TransferMode(ETransferMode m = ETransferMode::TM_UNKNOWN)
        : mTransferMode(m)
    {
    }

    //--------------------------------------------------------------------------
    //! @brief      Constructs a new instance.
    //!
    //! @param[in]  m     transfer mode value
    //--------------------------------------------------------------------------
    TransferMode(int m)
        : mTransferMode(static_cast<ETransferMode>(m))
    {
    }

    //--------------------------------------------------------------------------
    //! @brief      Etransfermode conversion operator.
    //--------------------------------------------------------------------------
    operator ETransferMode() const
    {
        return mTransferMode;
    }

    //--------------------------------------------------------------------------
    //! @brief      Bool conversion operator.
    //--------------------------------------------------------------------------
    operator bool() const
    {
        return isValid();
    }

    //--------------------------------------------------------------------------
    //! @brief      Assignment operator.
    //!
    //! @param[in]  other  The other
    //!
    //! @return     The result of the assignment
    //--------------------------------------------------------------------------
    TransferMode& operator=(ETransferMode other)
    {
        mTransferMode = other;
        return *this;
    }

    //--------------------------------------------------------------------------
    //! @brief      Assignment operator.
    //!
    //! @param[in]  other  The other
    //!
    //! @return     The result of the assignment
    //--------------------------------------------------------------------------
    TransferMode& operator=(int other)
    {
        mTransferMode = static_cast<ETransferMode>(other);
        return *this;
    }

    //--------------------------------------------------------------------------
    //! @brief      Determines if dao.
    //!
    //! @return     True if dao, False otherwise.
    //--------------------------------------------------------------------------
    bool isDao() const
    {
        return ((mTransferMode > TM_DAO_START) && (mTransferMode < TM_DAO_END));
    }

    //--------------------------------------------------------------------------
    //! @brief      Determines if tao.
    //!
    //! @return     True if tao, False otherwise.
    //--------------------------------------------------------------------------
    bool isTao() const
    {
        return ((mTransferMode > TM_TAO_START) && (mTransferMode < TM_TAO_END));
    }

    //--------------------------------------------------------------------------
    //! @brief      Determines if valid.
    //!
    //! @return     True if valid, False otherwise.
    //--------------------------------------------------------------------------
    bool isValid() const
    {
        return (isDao() || isTao());
    }

    //--------------------------------------------------------------------------
    //! @brief      does this mode require TOC manipulation
    //!
    //! @return     true if yes, False otherwise
    //--------------------------------------------------------------------------
    bool tocManip() const
    {
        return ((mTransferMode == TM_DAO_SP) || (mTransferMode == TM_DAO_SP_MONO) || (mTransferMode == TM_DAO_SP_PREENC));
    }

    //--------------------------------------------------------------------------
    //! @brief      does this mode require SP upload
    //!
    //! @return     true if yes, False otherwise
    //--------------------------------------------------------------------------
    bool spUpld() const
    {
        return mTransferMode == TM_DAO_SP_PREENC;
    }

    //--------------------------------------------------------------------------
    //! @brief      is this PCM upload?
    //!
    //! @return     true if yes, False otherwise
    //--------------------------------------------------------------------------
    bool pcmUpld() const
    {
        return ((mTransferMode == TM_TAO_SP) || (mTransferMode == TM_TAO_SP_MONO) || (mTransferMode == TM_DAO_SP) || (mTransferMode == TM_DAO_SP_MONO));
    }

    //--------------------------------------------------------------------------
    //! @brief      get short mode name
    //!
    //! @return     name or nullptr
    //--------------------------------------------------------------------------
    const char* name() const
    {
        if (const auto* m = find(mTransferMode))
        {
            return m->mName;
        }
        return nullptr;
    }

    //--------------------------------------------------------------------------
    //! @brief      get track mode
    //!
    //! @return     mode or nullptr
    //--------------------------------------------------------------------------
    const char* trackMode() const
    {
        if (const auto* m = find(mTransferMode))
        {
            return m->mTrackMode;
        }
        return nullptr;
    }

    //--------------------------------------------------------------------------
    //! @brief      get icon source
    //!
    //! @return     icon source or nullptr
    //--------------------------------------------------------------------------
    const char* iconSrc() const
    {
        if (const auto* m = find(mTransferMode))
        {
            return m->mIconSrc;
        }
        return nullptr;
    }

    //--------------------------------------------------------------------------
    //! @brief      get time multiplicator
    //!
    //! @return     multiplicator
    //--------------------------------------------------------------------------
    int multi() const
    {
        if (const auto* m = find(mTransferMode))
        {
            return m->mMulti;
        }
        return 1;
    }

    //--------------------------------------------------------------------------
    //! @brief      get NetMD command
    //!
    //! @param[in]  otf   on the fly marker
    //!
    //! @return     The net md command.
    //--------------------------------------------------------------------------
    NetMDCmd netMDCmd(bool otf) const
    {
        if (const auto* m = find(mTransferMode))
        {
            return otf ? m->mNetMDCmdOtf : m->mNetMDCmd;
        }
        return NetMDCmd::UNKNWON;
    }

    //--------------------------------------------------------------------------
    //! @brief      get encoder command
    //!
    //! @param[in]  otf   on the fly marker
    //!
    //! @return     The encode command.
    //--------------------------------------------------------------------------
    XEncCmd xencCmd(bool otf) const
    {
        if (const auto* m = find(mTransferMode))
        {
            return otf ? m->mEncCmdOtf : m->mEncCmd;
        }
        return XEncCmd::NONE;
    }

    //--------------------------------------------------------------------------
    //! @brief      check if mode is supported (device feature dependent)
    //!
    //! @param[in]  tocMnp     The toc manipulation flag
    //! @param[in]  spUpd      The sp update flag
    //! @param[in]  pcm2mono   The PCM to mono support flag
    //! @param[in]  nativeMono The native mono support flag
    //!
    //! @return     true if supported, false otherwise
    //--------------------------------------------------------------------------
    bool supports(bool tocMnp, bool spUpd, bool pcm2mono, bool nativeMono) const
    {
        return ((!tocManip() || (tocManip() && tocMnp))
                && (!spUpld() || (spUpld() && spUpd))
                && (!isMono() || (isMono() && pcm2mono) || (isMono() && nativeMono && !tocManip())));
    }

    //--------------------------------------------------------------------------
    //! @brief      const char* conversion operator.
    //--------------------------------------------------------------------------
    operator const char*() const
    {
#define mkCase(x_) case x_: c = #x_; break
        const char* c;
        switch(mTransferMode)
        {
        mkCase(TM_TAO_SP);
        mkCase(TM_TAO_SP_MONO);
        mkCase(TM_TAO_LP2);
        mkCase(TM_TAO_LP4);
        mkCase(TM_DAO_SP);
        mkCase(TM_DAO_SP_MONO);
        mkCase(TM_DAO_SP_PREENC);
        mkCase(TM_DAO_LP2);
        mkCase(TM_DAO_LP4);
        default: c = ""; break;
        }
        return c;
#undef mkCase
    }

    //--------------------------------------------------------------------------
    //! @brief      create a C vector of needed homebrew features
    //!
    //! @param[in]  tocMnp     The toc manipulation flag
    //! @param[in]  spUpd      The sp update flag
    //! @param[in]  pcm2mono   The PCM to mono support flag
    //! @param[in]  nativeMono The native mono support flag
    //!
    //! @return     C vector with needed features marked
    //--------------------------------------------------------------------------
    uint32_t hbFeatures(bool pcmSpeedup = false) const
    {
        uint32_t features = HBFeatures::NOTHING;

        if (spUpld())
        {
            features |= HBFeatures::SP_UPLOAD;
        }

        if (isMono())
        {
            features |= HBFeatures::PCM_2_MONO;
        }

        if (pcmSpeedup && pcmUpld())
        {
            features |= HBFeatures::PCM_SPEEDUP;
        }
        return features;
    }

    //--------------------------------------------------------------------------
    //! @brief      Determines if mono.
    //!
    //! @return     True if mono, False otherwise.
    //--------------------------------------------------------------------------
    bool isMono() const
    {
        return ((mTransferMode == TM_TAO_SP_MONO) || (mTransferMode == TM_DAO_SP_MONO));
    }

    //--------------------------------------------------------------------------
    //! @brief      Determines if lp.
    //!
    //! @return     True if lp, False otherwise.
    //--------------------------------------------------------------------------
    bool isLP() const
    {
        return ((mTransferMode == TM_TAO_LP2) || (mTransferMode == TM_TAO_LP4)
                || (mTransferMode == TM_DAO_LP2) || (mTransferMode == TM_DAO_LP4));
    }

protected:

    //--------------------------------------------------------------------------
    //! @brief      Searches for the first match.
    //!
    //! @param[in]  m     ETransferMode to search for
    //!
    //! @return     pointer to description, nullptr if not found
    //--------------------------------------------------------------------------
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
