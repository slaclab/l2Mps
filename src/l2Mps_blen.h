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

#include "l2Mps_thr.h"


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
struct blenThr_channel_t 
{
    blen_channel_t   blenCh;
    thr_table_t     thrTb;
};

// Callback function pointer
typedef void (*blen_cb_func_t)(int, blen_dataMap_t);

// Function pointer data typed
typedef const uint32_t (IMpsBlen::*BlenR32_t)(const blenThr_channel_t&) const;
typedef void (IMpsBlen::*BlenW32_t)(const blenThr_channel_t&, const uint32_t) const;
typedef const bool (IMpsBlen::*BlenR1_t)(const blenThr_channel_t&) const;
typedef void (IMpsBlen::*BlenW1_t)(const blenThr_channel_t&, const bool) const;

class IMpsBlen
{
public:
    IMpsBlen(Path mpsRoot, uint8_t amc);
    ~IMpsBlen();

    // Threhold channel information
    uint32_t const  getChannel          ( const blen_channel_t& ch) const { return findThrChannel(ch)->getChannel();     };
    bool     const  getIdleEn           ( const blen_channel_t& ch) const { return findThrChannel(ch)->getIdleEn();      };
    bool     const  getAltEn            ( const blen_channel_t& ch) const { return findThrChannel(ch)->getAltEn();       };
    bool     const  getLcls1En          ( const blen_channel_t& ch) const { return findThrChannel(ch)->getLcls1En();     };
    uint32_t const  getByteMap          ( const blen_channel_t& ch) const { return findThrChannel(ch)->getByteMap();     };
    uint32_t const  getThrCount         ( const blen_channel_t& ch) const { return findThrChannel(ch)->getThrCount();    };


    // Threshold set enable methods
    void            setThresholdMinEn   ( const blenThr_channel_t& ch, const bool val) const { findThrChannel(ch.blenCh)->setThresholdMinEn(ch.thrTb, val); };
    void            setThresholdMaxEn   ( const blenThr_channel_t& ch, const bool val) const { findThrChannel(ch.blenCh)->setThresholdMaxEn(ch.thrTb, val); };

    // Threshold get enable methods
    const bool      getThresholdMinEn   ( const blenThr_channel_t& ch) const { return findThrChannel(ch.blenCh)->getThresholdMinEn(ch.thrTb); };
    const bool      getThresholdMaxEn   ( const blenThr_channel_t& ch) const { return findThrChannel(ch.blenCh)->getThresholdMaxEn(ch.thrTb); };

    // Threshold set methods
    void            setThresholdMin     ( const blenThr_channel_t& ch, const uint32_t val) const { findThrChannel(ch.blenCh)->setThresholdMin(ch.thrTb, val); };
    void            setThresholdMax     ( const blenThr_channel_t& ch, const uint32_t val) const { findThrChannel(ch.blenCh)->setThresholdMax(ch.thrTb, val); };
       
    // Threshold get methods
    const uint32_t  getThresholdMin     ( const blenThr_channel_t& ch) const { return findThrChannel(ch.blenCh)->getThresholdMin(ch.thrTb); };
    const uint32_t  getThresholdMax     ( const blenThr_channel_t& ch) const { return findThrChannel(ch.blenCh)->getThresholdMax(ch.thrTb); };

    // Set polling thread with callback function
    const void      startPollThread     ( unsigned int poll, blen_cb_func_t callBack );

    // Find ThrChannel in the BLEN-ThrChannel map
    ThrChannel      findThrChannel(const blen_channel_t& blenCh) const;

    // Print BLEN channel information
    void            printChInfo     ( void ) const;

private:
    blen_thrMap_t    _blenThrMap;
    uint8_t         _amc;
    unsigned int    _poll;
    blen_cb_func_t   _blenCB;
    pthread_t       _scanThread;

    // Polling functions
    void        pollThread();
    static void *createThread(void* p) { static_cast<IMpsBlen*>(p)->pollThread(); return NULL; };
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