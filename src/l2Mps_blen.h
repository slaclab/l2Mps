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
const uint8_t blenChByteMap[2] = {0, 1};

class IMpsBlen;
typedef boost::shared_ptr<IMpsBlen> MpsBlen;

typedef std::array<int,2> blen_channel_t;

// Function pointer data typed
typedef const uint32_t (IMpsBlen::*BlenR32_t)(const blen_channel_t) const;
typedef void (IMpsBlen::*BlenW32_t)(const blen_channel_t, const uint32_t) const;
typedef const bool (IMpsBlen::*BlenR1_t)(const blen_channel_t) const;
typedef void (IMpsBlen::*BlenW1_t)(const blen_channel_t, const bool) const;

class IMpsBlen
{
public:
    IMpsBlen(Path mpsRoot, uint8_t amc);
    ~IMpsBlen();

    uint32_t const  getCh           ( const blen_channel_t ch) const;
    bool     const  getIdleEn       ( const blen_channel_t ch) const;
    bool     const  getAltEn        ( const blen_channel_t ch) const;
    bool     const  getLcls1En      ( const blen_channel_t ch) const;
    uint32_t const  getByteMap      ( const blen_channel_t ch) const;
    uint32_t const  getThrCount     ( const blen_channel_t ch) const;
   
    void            setThresholdMin    ( const blen_channel_t ch, const uint32_t val) const;
    const uint32_t  getThresholdMin    ( const blen_channel_t ch) const;
   
    void            setThresholdMinEn  ( const blen_channel_t ch, const bool val) const;
    const bool      getThresholdMinEn  ( const blen_channel_t ch) const;

    void printChInfo(const ThrChannel thr) const;

private:
    uint8_t     _ch;
    ThrChannel  _thr;
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