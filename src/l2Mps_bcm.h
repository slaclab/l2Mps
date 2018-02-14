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

#include "l2Mps_thr.h"

//Number of Threshold channels for BCM is 1 per bay: BCM, DIFF
const uint8_t numBcmChs = 2;

// Channel to byte map
//   - The index is the AMC nuber (0 or 1)
//   - The second index is the channel (0=BCM, 1=DIFF)
const uint8_t bcmChByteMap[2][2] = { {0, 2}, {1, 3} };

class IMpsBcm;
typedef boost::shared_ptr<IMpsBcm> MpsBcm;

// BCM data types
typedef int                                 bcm_channel_t;
typedef std::map<bcm_channel_t, thr_ch_t>   bcm_dataMap_t;
typedef std::map<bcm_channel_t, ThrChannel> bcm_thrMap_t;
struct bcmThr_channel_t 
{
    bcm_channel_t   bcmCh;
    thr_table_t     thrTb;
};

// Callback function pointer
typedef void (*bcm_cb_func_t)(int, bcm_dataMap_t);

// Function pointer data typed
typedef const uint32_t (IMpsBcm::*BcmR32_t)(const bcmThr_channel_t&) const;
typedef void (IMpsBcm::*BcmW32_t)(const bcmThr_channel_t&, const uint32_t) const;
typedef const bool (IMpsBcm::*BcmR1_t)(const bcmThr_channel_t&) const;
typedef void (IMpsBcm::*BcmW1_t)(const bcmThr_channel_t&, const bool) const;

class IMpsBcm
{
public:
    // Constructor
    IMpsBcm(Path mpsRoot, const uint8_t amc);
    ~IMpsBcm();

    // Threhold channel information
    uint32_t const  getChannel          ( const bcm_channel_t& ch) const { return findThrChannel(ch)->getChannel();     };
    bool     const  getIdleEn           ( const bcm_channel_t& ch) const { return findThrChannel(ch)->getIdleEn();      };
    bool     const  getAltEn            ( const bcm_channel_t& ch) const { return findThrChannel(ch)->getAltEn();       };
    bool     const  getLcls1En          ( const bcm_channel_t& ch) const { return findThrChannel(ch)->getLcls1En();     };
    uint32_t const  getByteMap          ( const bcm_channel_t& ch) const { return findThrChannel(ch)->getByteMap();     };
    uint32_t const  getThrCount         ( const bcm_channel_t& ch) const { return findThrChannel(ch)->getThrCount();    };


    // Threshold set enable methods
    void            setThresholdMinEn   ( const bcmThr_channel_t& ch, const bool val) const { findThrChannel(ch.bcmCh)->setThresholdMinEn(ch.thrTb, val); };
    void            setThresholdMaxEn   ( const bcmThr_channel_t& ch, const bool val) const { findThrChannel(ch.bcmCh)->setThresholdMaxEn(ch.thrTb, val); };

    // Threshold get enable methods
    const bool      getThresholdMinEn   ( const bcmThr_channel_t& ch) const { return findThrChannel(ch.bcmCh)->getThresholdMinEn(ch.thrTb); };
    const bool      getThresholdMaxEn   ( const bcmThr_channel_t& ch) const { return findThrChannel(ch.bcmCh)->getThresholdMaxEn(ch.thrTb); };

    // Threshold set methods
    void            setThresholdMin     ( const bcmThr_channel_t& ch, const uint32_t val) const { findThrChannel(ch.bcmCh)->setThresholdMin(ch.thrTb, val); };
    void            setThresholdMax     ( const bcmThr_channel_t& ch, const uint32_t val) const { findThrChannel(ch.bcmCh)->setThresholdMax(ch.thrTb, val); };
       
    // Threshold get methods
    const uint32_t  getThresholdMin     ( const bcmThr_channel_t& ch) const { return findThrChannel(ch.bcmCh)->getThresholdMin(ch.thrTb); };
    const uint32_t  getThresholdMax     ( const bcmThr_channel_t& ch) const { return findThrChannel(ch.bcmCh)->getThresholdMax(ch.thrTb); };

    // Set polling thread with callback function
    const void      startPollThread     ( unsigned int poll, bcm_cb_func_t callBack );

    // Find ThrChannel in the BCM-ThrChannel map
    ThrChannel      findThrChannel(const bcm_channel_t& bcmCh) const;

    // Print BCM channel information
    void            printChInfo     ( void ) const;

private:
    bcm_thrMap_t    _bcmThrMap;
    uint8_t         _amc;
    unsigned int    _poll;
    bcm_cb_func_t   _bcmCB;
    pthread_t       _scanThread;

    // Polling functions
    void        pollThread();
    static void *createThread(void* p) { static_cast<IMpsBcm*>(p)->pollThread(); return NULL; };
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