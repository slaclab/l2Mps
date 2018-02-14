#ifndef L2MPS_BPM_H
#define L2MPS_BPM_H

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
struct bpmThr_channel_t 
{
    bpm_channel_t   bpmCh;
    thr_table_t     thrTb;
};

// Callback function pointer
typedef void (*bpm_cb_func_t)(int, bpm_dataMap_t);

// Function pointer data types
typedef const uint32_t (IMpsBpm::*BpmR32_t)(const bpmThr_channel_t&) const;
typedef void (IMpsBpm::*BpmW32_t)(const bpmThr_channel_t&, const uint32_t) const;
typedef const bool (IMpsBpm::*BpmR1_t)(const bpmThr_channel_t&) const;
typedef void (IMpsBpm::*BpmW1_t)(const bpmThr_channel_t&, const bool) const;

class IMpsBpm
{
public:
    //Constructor
    IMpsBpm(Path mpsRoot, uint8_t amc);
    ~IMpsBpm();

    // Threhold channel information
    uint32_t const  getChannel          ( const bpm_channel_t& ch) const { return findThrChannel(ch)->getChannel();     };
    bool     const  getIdleEn           ( const bpm_channel_t& ch) const { return findThrChannel(ch)->getIdleEn();      };
    bool     const  getAltEn            ( const bpm_channel_t& ch) const { return findThrChannel(ch)->getAltEn();       };
    bool     const  getLcls1En          ( const bpm_channel_t& ch) const { return findThrChannel(ch)->getLcls1En();     };
    uint32_t const  getByteMap          ( const bpm_channel_t& ch) const { return findThrChannel(ch)->getByteMap();     };
    uint32_t const  getThrCount         ( const bpm_channel_t& ch) const { return findThrChannel(ch)->getThrCount();    };


    // Threshold set enable methods
    void            setThresholdMinEn   ( const bpmThr_channel_t& ch, const bool val) const { findThrChannel(ch.bpmCh)->setThresholdMinEn(ch.thrTb, val); };
    void            setThresholdMaxEn   ( const bpmThr_channel_t& ch, const bool val) const { findThrChannel(ch.bpmCh)->setThresholdMaxEn(ch.thrTb, val); };

    // Threshold get enable methods
    const bool      getThresholdMinEn   ( const bpmThr_channel_t& ch) const { return findThrChannel(ch.bpmCh)->getThresholdMinEn(ch.thrTb); };
    const bool      getThresholdMaxEn   ( const bpmThr_channel_t& ch) const { return findThrChannel(ch.bpmCh)->getThresholdMaxEn(ch.thrTb); };

    // Threshold set methods
    void            setThresholdMin     ( const bpmThr_channel_t& ch, const uint32_t val) const { findThrChannel(ch.bpmCh)->setThresholdMin(ch.thrTb, val); };
    void            setThresholdMax     ( const bpmThr_channel_t& ch, const uint32_t val) const { findThrChannel(ch.bpmCh)->setThresholdMax(ch.thrTb, val); };
       
    // Threshold get methods
    const uint32_t  getThresholdMin     ( const bpmThr_channel_t& ch) const { return findThrChannel(ch.bpmCh)->getThresholdMin(ch.thrTb); };
    const uint32_t  getThresholdMax     ( const bpmThr_channel_t& ch) const { return findThrChannel(ch.bpmCh)->getThresholdMax(ch.thrTb); };

    // Set polling thread with callback function
    const void      startPollThread     ( unsigned int poll, bpm_cb_func_t callBack );

    // Find ThrChannel in the BPM-ThrChannel map
    ThrChannel      findThrChannel(const bpm_channel_t& bpmCh) const;

    // Print BPM channel information
    void            printChInfo     ( void ) const;

private:
    bpm_thrMap_t    _bpmThrMap;
    uint8_t         _amc;
    unsigned int    _poll;
    bpm_cb_func_t   _bpmCB;
    pthread_t       _scanThread;

    // Polling functions
    void        pollThread();
    static void *createThread(void* p) { static_cast<IMpsBpm*>(p)->pollThread(); return NULL; };    
};

class MpsBpmFactory
{
public:
    static MpsBpm create(Path mpsRoot, uint8_t amc)
    {
        return MpsBpm(new IMpsBpm(mpsRoot, amc));
    }
};

#endif