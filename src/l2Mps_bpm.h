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
const uint8_t bpmChByteMap[2][4] = { {2, 4, 0, 6}, {3, 5, 1, 7} };

class IMpsBpm;
typedef boost::shared_ptr<IMpsBpm>  MpsBpm;

class bpm_channel;
typedef bpm_channel bpm_channel_t;

// Function pointer data types
typedef const uint32_t (IMpsBpm::*BpmR32_t)(const bpm_channel&) const;
typedef void (IMpsBpm::*BpmW32_t)(const bpm_channel&, const uint32_t) const;
typedef const bool (IMpsBpm::*BpmR1_t)(const bpm_channel&) const;
typedef void (IMpsBpm::*BpmW1_t)(const bpm_channel&, const bool) const;


class IMpsBpm
{
public:
    //Constructor
    IMpsBpm(Path mpsRoot, uint8_t amc);
    ~IMpsBpm();

    uint32_t const  getCh           ( const bpm_channel& ch) const;
    bool     const  getIdleEn       ( const bpm_channel& ch) const;
    bool     const  getAltEn        ( const bpm_channel& ch) const;
    bool     const  getLcls1En      ( const bpm_channel& ch) const;
    uint32_t const  getByteMap      ( const bpm_channel& ch) const;
    uint32_t const  getThrCount     ( const bpm_channel& ch) const;
   
    void            setThreshold    ( const bpm_channel& ch, const uint32_t val) const;
    const uint32_t  getThreshold    ( const bpm_channel& ch) const;
   
    void            setThresholdEn  ( const bpm_channel& ch, const bool val) const;
    const bool      getThresholdEn  ( const bpm_channel& ch) const;

    void printChInfo(const ThrChannel thr) const;

private:
    // 0=X, 1=Y, 2=C, 3=R
    std::array<uint8_t, numBpmChs> _ch;
    std::array<ThrChannel, numBpmChs> _thr;
};

class MpsBpmFactory
{
public:
    static MpsBpm create(Path mpsRoot, uint8_t amc)
    {
        return MpsBpm(new IMpsBpm(mpsRoot, amc));
    }
};

class bpm_channel
{
public:
    bpm_channel(const std::array<int, 4> ch) : _ch(ch), _bpm_ch(ch[0]), _thr_ch(thr_channel_t{{ch[1], ch[2], ch[3]}}) { }
    int const           getBpmCh() const        { return _bpm_ch;   }
    thr_channel_t const getThrCh() const        { return _thr_ch;   }
    int const           operator[](int i) const { return _ch[i];    }

private:
    std::array<int, 4>  _ch;
    int                 _bpm_ch;
    thr_channel_t       _thr_ch;
};

#endif