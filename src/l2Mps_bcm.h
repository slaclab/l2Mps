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

class bcm_channel;
typedef bcm_channel bcm_channel_t;

// Function pointer data typed
typedef const uint32_t (IMpsBcm::*BcmR32_t)(const bcm_channel&) const;
typedef void (IMpsBcm::*BcmW32_t)(const bcm_channel&, const uint32_t) const;
typedef const bool (IMpsBcm::*BcmR1_t)(const bcm_channel&) const;
typedef void (IMpsBcm::*BcmW1_t)(const bcm_channel&, const bool) const;

class IMpsBcm
{
public:
    // Constructor
    IMpsBcm(Path mpsRoot, uint8_t amc);
    ~IMpsBcm();

    uint32_t const  getCh           ( const bcm_channel& ch) const;
    bool     const  getIdleEn       ( const bcm_channel& ch) const;
    bool     const  getAltEn        ( const bcm_channel& ch) const;
    bool     const  getLcls1En      ( const bcm_channel& ch) const;
    uint32_t const  getByteMap      ( const bcm_channel& ch) const;
    uint32_t const  getThrCount     ( const bcm_channel& ch) const;
   
    void            setThresholdMin    ( const bcm_channel& ch, const uint32_t val) const;
    const uint32_t  getThresholdMin    ( const bcm_channel& ch) const;
   
    void            setThresholdMinEn  ( const bcm_channel& ch, const bool val) const;
    const bool      getThresholdMinEn  ( const bcm_channel& ch) const;

    void printChInfo(const ThrChannel thr) const;

private:
    // 0=BCM, 1=DIFF
    std::array<uint8_t, numBcmChs> _ch;
    std::array<ThrChannel, numBcmChs> _thr;
};

class MpsBcmFactory
{
public:
    static MpsBcm create(Path mpsRoot, uint8_t amc)
    {
        return MpsBcm(new IMpsBcm(mpsRoot, amc));
    }
};

class bcm_channel
{
public:
    bcm_channel(const std::array<int, 3> ch) : _ch(ch), _bcm_ch(ch[0]), _thr_ch(thr_table_t{{ch[1], ch[2]}}) { }
    int const           getBcmCh() const        { return _bcm_ch; }
    thr_table_t const   getThrCh() const        { return _thr_ch; }
    int const           operator[](int i) const { return _ch[i];  }

private:
    std::array<int, 3> _ch;
    int                _bcm_ch;
    thr_table_t      _thr_ch;
};

#endif