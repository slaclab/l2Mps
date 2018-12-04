#ifndef L2MPS_BCM_H
#define L2MPS_BCM_H

/**
 *-----------------------------------------------------------------------------
 * Title      : BCM application class
 * ----------------------------------------------------------------------------
 * File       : l2Mps_bcm.h
 * Author     : Jesus Vasquez, jvasquez@slac.stanford.edu
 * Created    : 2017-10-20
 * ----------------------------------------------------------------------------
 * Description:
 * Class for interfacing the BCM's MPS application.
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

//Number of Threshold channels for BCM is 1 per bay: BCM, DIFF
const uint8_t numBcmChs = 2;

// Channel to byte map
//   - The index is the AMC nuber (0 or 1)
//   - The second index is the channel (0=BCM, 1=DIFF)
const uint8_t bcmChByteMap[2][numBcmChs] = { {0, 2}, {1, 3} };

class IMpsBcm;
typedef boost::shared_ptr<IMpsBcm> MpsBcm;

// BCM data types
typedef int                                 bcm_channel_t;
typedef std::map<bcm_channel_t, thr_ch_t>   bcm_dataMap_t;
typedef std::map<bcm_channel_t, ThrChannel> bcm_thrMap_t;
typedef appThr_channel_t<bcm_channel_t>     bcmThr_channel_t;


class IMpsBcm : public IMpsAppBase<bcm_channel_t>
{
public:
    // Constructor
    IMpsBcm(Path mpsRoot, const uint8_t amc);
    ~IMpsBcm() {};

    // Factory method, which returns an smart pointer
    static MpsBcm create(Path mpsRoot, uint8_t amc);

    virtual void printChInfo(void) const;
};

#endif
