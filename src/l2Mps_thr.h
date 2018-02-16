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

typedef boost::shared_ptr<IThrChannel>    ThrChannel;

// Threshod table arrays:
//   - Indexes:
//       [table=0..numThrTables] [threshold=0..maxThrCount]
//       - Table index: 
//           - 0: Lcls1
//           - 1: Idle
//           - 2: Std
//           - 3: Alt
typedef std::array<int, 2>                          thr_table_t;

// Threshold table data
struct thr_tableData_t
{
    bool    minEn;
    bool    maxEn;
    float   min;
    float   max;
};

// Threshold table data map
typedef std::map<thr_table_t, thr_tableData_t>      thr_chData_t;

// Threhold table scalval interfaces
struct thr_tableScalval_t
{
    ScalVal minEn;
    ScalVal maxEn;
    ScalVal min;
    ScalVal max;
};

// Threshold table scalval interfaces map
typedef std::map<thr_table_t, thr_tableScalval_t>   thr_chScalval_t;

// Threhold channel information data
struct thr_chInfoData_t
{
    int  ch;
    int  count;
    int  byteMap;
    bool idleEn;
    bool altEn;
    bool lcls1En;
};

// Threshold channel information scalval interfaces 
struct thr_chInfoScalval_t
{
    ScalVal_RO count;
    ScalVal_RO byteMap;
    ScalVal    idleEn;
    ScalVal_RO altEn;
    ScalVal_RO lcls1En;
};

// Threhold data (information + table data)
struct thr_ch_t 
{
    thr_chInfoData_t info;
    thr_chData_t data;
};

// Threhold scalval interfaces (information + table data)
struct thr_scalval_t
{
    thr_chInfoScalval_t info;
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
    const float getThresholdMin(thr_table_t ch) const;
    const float getThresholdMax(thr_table_t ch) const;

    // Read threshold enable register
    const bool getThresholdMinEn(thr_table_t ch) const;
    const bool getThresholdMaxEn(thr_table_t ch) const;

    // Write threshold registers
    void setThresholdMin(thr_table_t ch, const float val) const;
    void setThresholdMax(thr_table_t ch, const float val) const;

    // Write threshold enable register
    void setThresholdMinEn(thr_table_t ch, const bool val) const;
    void setThresholdMaxEn(thr_table_t ch, const bool val) const;

    // Set/get scale factor
    void        setScaleFactor(const float sf);       
    const float getScaleFactor() const;

    void readAll(thr_ch_t& data) const;
    void readThrChInfo(thr_chInfoData_t& info) const;
    void readThrChData(thr_chData_t& data) const;


private:
    Path            _chRoot;        // Root path to the channel register space
    int             ch;             // Threhold channel number
    thr_scalval_t   thrScalvals;    // Threshold scalval interfaces
    float           scaleFactor;    // Scale factor (egu/raw)
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
