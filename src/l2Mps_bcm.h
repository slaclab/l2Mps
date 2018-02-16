#ifndef L2MPS_BCM_H
#define L2MPS_BCM_H

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
    ~IMpsBcm();

    virtual void printChInfo(void) const;   
};

class MpsBcmFactory
{
public:
    static MpsBcm create(Path mpsRoot, uint8_t amc)
    {
        return MpsBcm(new IMpsBcm(mpsRoot, amc));
    }
};

#endif