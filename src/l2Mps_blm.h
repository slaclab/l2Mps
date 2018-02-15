#ifndef LCLS2MPSLN_BLM_H
#define LCLS2MPSLN_BLM_H

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
#include <pthread.h>

#include "l2Mps_thr.h"

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
    // {
    //     {  0,  1,  2,  3 },
    //     {  4,  5,  6,  7 },
    //     {  8,  9, 10, 11 },
    // },
    // {
    //     { 12, 13, 14, 15 },
    //     { 16, 17, 18, 19 },
    //     { 20, 21, 22, 23 },
    // }
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

typedef std::array<int,2>                   blm_channel_t;
typedef std::map<blm_channel_t, thr_ch_t>   blm_dataMap_t;
typedef std::map<blm_channel_t, ThrChannel> blm_thrMap_t;

typedef void (*p_func_t)(int, blm_dataMap_t);

struct blmThr_channel_t 
{
    blm_channel_t   blmCh;
    thr_table_t     thrTb;
};

class IMpsBlm
{
public:
    IMpsBlm(Path mpsRoot, const uint8_t amc);
    ~IMpsBlm();

    // Threhold channel information
    uint32_t const  getChannel          ( const blm_channel_t& ch) const { return findThrChannel(ch)->getChannel();     };
    bool     const  getIdleEn           ( const blm_channel_t& ch) const { return findThrChannel(ch)->getIdleEn();      };
    bool     const  getAltEn            ( const blm_channel_t& ch) const { return findThrChannel(ch)->getAltEn();       };
    bool     const  getLcls1En          ( const blm_channel_t& ch) const { return findThrChannel(ch)->getLcls1En();     };
    uint32_t const  getByteMap          ( const blm_channel_t& ch) const { return findThrChannel(ch)->getByteMap();     };
    uint32_t const  getThrCount         ( const blm_channel_t& ch) const { return findThrChannel(ch)->getThrCount();    };


    // Threshold set enable methods
    void            setThresholdMinEn   ( const blmThr_channel_t& ch, const bool val) const { findThrChannel(ch.blmCh)->setThresholdMinEn(ch.thrTb, val); };
    void            setThresholdMaxEn   ( const blmThr_channel_t& ch, const bool val) const { findThrChannel(ch.blmCh)->setThresholdMaxEn(ch.thrTb, val); };

    // Threshold get enable methods
    const bool      getThresholdMinEn   ( const blmThr_channel_t& ch) const { return findThrChannel(ch.blmCh)->getThresholdMinEn(ch.thrTb); };
    const bool      getThresholdMaxEn   ( const blmThr_channel_t& ch) const { return findThrChannel(ch.blmCh)->getThresholdMaxEn(ch.thrTb); };

    // Threshold set methods
    void            setThresholdMin     ( const blmThr_channel_t& ch, const uint32_t val) const { findThrChannel(ch.blmCh)->setThresholdMin(ch.thrTb, val); };
    void            setThresholdMax     ( const blmThr_channel_t& ch, const uint32_t val) const { findThrChannel(ch.blmCh)->setThresholdMax(ch.thrTb, val); };
       
    // Threshold get methods
    const uint32_t  getThresholdMin     ( const blmThr_channel_t& ch) const { return findThrChannel(ch.blmCh)->getThresholdMin(ch.thrTb); };
    const uint32_t  getThresholdMax     ( const blmThr_channel_t& ch) const { return findThrChannel(ch.blmCh)->getThresholdMax(ch.thrTb); };

    // Set polling thread with callback function
    const void      startPollThread     ( unsigned int poll, p_func_t callBack );

    // Find ThrChannel in the Blm-ThrChannel map
    ThrChannel      findThrChannel(const blm_channel_t& blmCh) const;

    // Print BLM channel information
    void            printChInfo     ( void ) const;

private:
    blm_thrMap_t    _blmThrMap;
    uint8_t         _amc;
    unsigned int    _poll;
    p_func_t        _blmCB;
    pthread_t       _scanThread;

    // Polling functions
    void        pollThread();
    static void *createThread(void* p) { static_cast<IMpsBlm*>(p)->pollThread(); return NULL; };


};

class MpsBlmFactory
{
public:
    static MpsBlm create(Path mpsRoot, const uint8_t amc)
    {
        return MpsBlm(new IMpsBlm(mpsRoot, amc));
    }
};

#endif
