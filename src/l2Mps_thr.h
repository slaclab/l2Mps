#ifndef LCLS2MPSLN_THR_H
#define LCLS2MPSLN_THR_H

#include <stdio.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <string.h>
#include <iomanip>
#include <inttypes.h>
#include <stdexcept>
#include <array>
#include <map>
#include <boost/shared_ptr.hpp>
#include <cpsw_api_user.h>

#include "l2Mps_base.h"

class IThrChannel;

typedef boost::shared_ptr<const IThrChannel>    ThrChannel;

typedef std::array<int, 2>                          thr_table_t;
// typedef std::array<int, 3>                          thr_table_t;


struct thr_tableData_t
{
    bool minEn;
    bool maxEn;
    int  min;
    int  max;
};
//typedef std::pair<int, bool>                        thr_tableData_t;

typedef std::map<thr_table_t, thr_tableData_t>      thr_chData_t;

struct thr_tableScalval_t
{
    ScalVal minEn;
    ScalVal maxEn;
    ScalVal min;
    ScalVal max;
};
// typedef std::pair<ScalVal, ScalVal>                 thr_tableScalval_t;

typedef std::map<thr_table_t, thr_tableScalval_t>   thr_chScalval_t;

struct thr_chInfo_t
{
    int  ch;
    int  count;
    int  byteMap;
    bool idleEn;
    bool altEn;
    bool lcls1En;
};

struct thr_infoScalval_t
{
    ScalVal_RO count;
    ScalVal_RO byteMap;
    ScalVal    idleEn;
    ScalVal_RO altEn;
    ScalVal_RO lcls1En;
};

struct thr_ch_t 
{
    thr_chInfo_t info;
    thr_chData_t data;
};

struct thr_scalval_t
{
    thr_infoScalval_t info;
    thr_chScalval_t   data;

};

// Maximum number of channels
const uint8_t maxChannelCount = 24;

// Number of threshold tables (Lcls1, Idle, Std, ALt)
const uint8_t numThrTables = 4;

// Number of threshold limits (Min, Max)
const uint8_t numThrLimits = 2;

// Number of threshold in each table
const uint8_t numThrCounts[numThrTables] = {1, 1, 8, 8};
const uint8_t maxThrCount = 8; // Maxium 

// Name of the MPS module where the threshold channels are defined
const std::string MpsThrModuleName = "AppMpsRegAppCh";

class IThrChannel
{
public:
    IThrChannel(Path mpsRoot, uint8_t channel);
    ~IThrChannel();

    // Channel header
    uint8_t getChannel()             const   { return ch; } 
    uint8_t getThrCount()            const;
    bool    getIdleEn()              const;
    void    setIdleEn(const bool en) const;
    bool    getAltEn()               const;
    bool    getLcls1En()             const;
    uint8_t getByteMap()             const;

    
    // Read threshold register
    const uint32_t getThresholdMin(thr_table_t ch) const;
    const uint32_t getThresholdMax(thr_table_t ch) const;

    // Read threshold enable register
    const bool getThresholdMinEn(thr_table_t ch) const;
    const bool getThresholdMaxEn(thr_table_t ch) const;

    // Write threshold registers
    void setThresholdMin(thr_table_t ch, const uint32_t val) const;
    void setThresholdMax(thr_table_t ch, const uint32_t val) const;

    // Write threshold enable register
    void setThresholdMinEn(thr_table_t ch, const bool val) const;
    void setThresholdMaxEn(thr_table_t ch, const bool val) const;

    void readAll(thr_ch_t& data) const;
    void readThrChInfo(thr_chInfo_t& info) const;
    void readThrChData(thr_chData_t& data) const;


private:
    // Root path to the channel register space
    Path       _chRoot;
    // Channel information
    // uint8_t    _ch;
    // ScalVal_RO _thrCount;
    // ScalVal    _idleEn;
    // ScalVal_RO _altEn;
    // ScalVal_RO _lcls1En;
    // ScalVal_RO _byteMap;

    // Threshod arrays:
    //   - Indexes:
    //       [table=0..numThrTables] [limit=0..numThrLimits] [threshold=0..maxThrCount]
    //       - Table index: 
    //           - 0: Lcls1
    //           - 1: Idle
    //           - 2: Std
    //           - 3: Alt
    //       - Limit index: 
    //           - 0: Min
    //           - 1: Max
    // Threshold registers
    // thr_chScalval_t _thrScalvalMap;
    int ch;
    thr_scalval_t thrScalvals;
};

// Factory class
class ThrChannelFactory
{
public:
    static ThrChannel create(Path mpsRoot, uint8_t channel)
    {
        return ThrChannel(new IThrChannel(mpsRoot, channel));
    }
};

#endif
