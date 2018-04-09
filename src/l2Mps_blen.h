#ifndef L2MPS_BLEN_H
#define L2MPS_BLEN_H

#include <stdio.h>
#include <iostream>
#include <string.h>
#include <iomanip>
#include <inttypes.h>
#include <vector>
#include <map>
#include <array>
#include <boost/shared_ptr.hpp>
#include <cpsw_api_user.h>

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

    virtual void printChInfo(void) const;

};

class MpsBlenFactory
{
public:
    static MpsBlen create(Path mpsRoot, uint8_t amc)
    {
        return MpsBlen(new IMpsBlen(mpsRoot, amc));
    }
};

#endif