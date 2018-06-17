#ifndef L2MPS_BPM_H
#define L2MPS_BPM_H

#include "l2Mps_base.h"

// Number of Threshold channels for BPM is 4: X, Y, C, R
const uint8_t numBpmChs = 4;

// Channel to byte map:
//   - Fisrt index is the AMC number (0 or 1)
//   - The second index is the channel (0=X, 1=Y, 2=Charge, 3=Radius)
const uint8_t bpmChByteMap[2][numBpmChs] = { {2, 4, 0, 6}, {3, 5, 1, 7} };

class IMpsBpm;
typedef boost::shared_ptr<IMpsBpm>  MpsBpm;

// BPM data types
typedef int                                 bpm_channel_t;
typedef std::map<bpm_channel_t, thr_ch_t>   bpm_dataMap_t;
typedef std::map<bpm_channel_t, ThrChannel> bpm_thrMap_t;
typedef appThr_channel_t<bpm_channel_t>     bpmThr_channel_t;

class IMpsBpm : public IMpsAppBase<bpm_channel_t>
{
public:
    //Constructor
    IMpsBpm(Path mpsRoot, uint8_t amc);
    ~IMpsBpm() {};

    virtual void printChInfo(void) const;
};

class MpsBpmFactory
{
public:
    static MpsBpm create(Path mpsRoot, uint8_t amc)
    {
        return boost::make_shared<IMpsBpm>(mpsRoot, amc);
    }
};

#endif
