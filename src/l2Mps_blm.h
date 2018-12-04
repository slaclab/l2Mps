#ifndef LCLS2MPSLN_BLM_H
#define LCLS2MPSLN_BLM_H

/**
 *-----------------------------------------------------------------------------
 * Title      : BLM application class
 * ----------------------------------------------------------------------------
 * File       : l2Mps_blm.h
 * Author     : Jesus Vasquez, jvasquez@slac.stanford.edu
 * Created    : 2017-10-20
 * ----------------------------------------------------------------------------
 * Description:
 * Class for interfacing the BLM's MPS application.
 * ----------------------------------------------------------------------------
 * This file is part of l2Mps. It is subject to
 * the license terms in the LICENSE.txt file found in the top-level directory
 * of this distribution and at:
    * https://confluence.slac.stanford.edu/display/ppareg/LICENSE.html.
 * No part of l2Mps, including this file, may be
 * copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE.txt file.
 * ----------------------------------------------------------------------------
**/

#include "l2Mps_base.h"

// Number of Channel for BLM is 3 (per AMC)
const uint8_t numBlmChs = 3;
// Number of Integration channel for BLM is 4 (per each channel)
const uint8_t numBlmIntChs = 4;

// Channel to byte map:
//    - Fisrt index is the AMC number (0 or 1)
//    - The second index is the channel (0 to 5)
//    - Teh third index is the integration channel (0 to 3)
const uint8_t blmChByteMap[2][numBlmChs][numBlmIntChs] =
{
    {
        {  0,  6, 12, 18 },
        {  1,  7, 13, 19 },
        {  2,  8, 14, 20 },
    },
    {
        {  3,  9, 15, 21 },
        {  4, 10, 16, 22 },
        {  5, 11, 17, 23 },
    }
};

class IMpsBlm;
typedef boost::shared_ptr<IMpsBlm> MpsBlm;

// BLM data types
typedef std::array<int,2>                   blm_channel_t;
typedef std::map<blm_channel_t, thr_ch_t>   blm_dataMap_t;
typedef std::map<blm_channel_t, ThrChannel> blm_thrMap_t;
typedef appThr_channel_t<blm_channel_t>     blmThr_channel_t;


class IMpsBlm : public IMpsAppBase<blm_channel_t>
{
public:
    IMpsBlm(Path mpsRoot, const uint8_t amc);
    ~IMpsBlm() {};

    // Factory method, which reurns an smart pointers
    static MpsBlm create(Path mpsRoot, const uint8_t amc);

    virtual void printChInfo(void) const;
};

#endif
