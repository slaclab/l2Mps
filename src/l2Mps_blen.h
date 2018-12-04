#ifndef L2MPS_BLEN_H
#define L2MPS_BLEN_H

/**
 *-----------------------------------------------------------------------------
 * Title      : BLEN application class
 * ----------------------------------------------------------------------------
 * File       : l2Mps_blen.h
 * Author     : Jesus Vasquez, jvasquez@slac.stanford.edu
 * Created    : 2017-10-20
 * ----------------------------------------------------------------------------
 * Description:
 * Class for interfacing the BLEN's MPS application.
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


//Number of Threshold channels for BLEN is 1 pre bay
const uint8_t numBlenChs = 1;

// Channel to byte map
//   - The index is the AMC nuber (0 or 1)
//   - The second index is the channel (0=BLEN)
const uint8_t blenChByteMap[2][numBlenChs] = {0, 1};

class IMpsBlen;
typedef boost::shared_ptr<IMpsBlen> MpsBlen;

// BLEN data types
typedef int                                  blen_channel_t;
typedef std::map<blen_channel_t, thr_ch_t>   blen_dataMap_t;
typedef std::map<blen_channel_t, ThrChannel> blen_thrMap_t;
typedef appThr_channel_t<blen_channel_t>     blenThr_channel_t;

class IMpsBlen : public IMpsAppBase<blen_channel_t>
{
public:
    IMpsBlen(Path mpsRoot, uint8_t amc);
    ~IMpsBlen() {};

    // Factory method, which returns an smart pointer
    static MpsBlen create(Path mpsRoot, uint8_t amc);

    virtual void printChInfo(void) const;

};

#endif
