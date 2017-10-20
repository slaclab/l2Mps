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

class blm_channel;
typedef blm_channel blm_channel_t;

// Function pointer data types
typedef const uint32_t (IMpsBlm::*BlmR32_t)(const blm_channel&) const;
typedef void (IMpsBlm::*BlmW32_t)(const blm_channel&, const uint32_t) const;
typedef const bool (IMpsBlm::*BlmR1_t)(const blm_channel&) const;
typedef void (IMpsBlm::*BlmW1_t)(const blm_channel&, const bool) const;

class IMpsBlm
{
public:
    IMpsBlm(Path mpsRoot, const uint8_t amc);
    ~IMpsBlm();

    uint32_t const  getCh           ( const blm_channel& ch) const;
    bool     const  getIdleEn       ( const blm_channel& ch) const;
    bool     const  getAltEn        ( const blm_channel& ch) const;
    bool     const  getLcls1En      ( const blm_channel& ch) const;
    uint32_t const  getByteMap      ( const blm_channel& ch) const;
    uint32_t const  getThrCount     ( const blm_channel& ch) const;

    void            setThreshold    ( const blm_channel& ch, const uint32_t val) const;
    const uint32_t  getThreshold    ( const blm_channel& ch) const;
    
    void            setThresholdEn  ( const blm_channel& ch, const bool val) const;
    const bool      getThresholdEn  ( const blm_channel& ch) const;
    
    void            printChInfo     ( void ) const;

private:
    std::map<std::pair<int, int>, int> _ch;
    std::map<std::pair<int, int>, ThrChannel> _thr;


};

class MpsBlmFactory
{
public:
    static MpsBlm create(Path mpsRoot, const uint8_t amc)
    {
        return MpsBlm(new IMpsBlm(mpsRoot, amc));
    }
};

class blm_channel
{
public:
    blm_channel(const std::array<int, 5> ch) : _ch(ch), _blm_ch(std::make_pair(ch[0], ch[1])), _thr_ch(thr_channel_t{{ch[2], ch[3], ch[4]}}) { }
    thr_channel_t       const   getThrCh()          const   { return _thr_ch;   }
    std::pair<int, int> const   getBlenCh()         const   { return _blm_ch;   }
    int                 const   operator[](int i)   const   { return _ch[i];    }

private:
    std::array<int, 5>  _ch;
    std::pair<int, int> _blm_ch;
    thr_channel_t       _thr_ch;
};

#endif