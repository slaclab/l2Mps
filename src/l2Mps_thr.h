#ifndef LCLS2MPSLN_THR_H
#define LCLS2MPSLN_THR_H

#include <stdio.h>
#include <iostream>
#include <sstream>
#include <string.h>
#include <iomanip>
#include <inttypes.h>
#include <stdexcept>
#include <array>
#include <map>
#include <boost/shared_ptr.hpp>
#include <cpsw_api_user.h>

class IThrChannel;

typedef boost::shared_ptr<const IThrChannel>    ThrChannel;
typedef const std::array<int, 3>                thr_channel_t;
typedef std::map<thr_channel_t, ScalVal>        thr_channel_map_t;

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
    uint8_t getChannel()    const   { return _ch;       } 
    uint8_t getThrCount()   const   { return _thrCount; }
    // bool    getIdleEn()     const   { return _idleEn;   }
    bool    getAltEn()      const   { return _altEn;    }  
    bool    getLcls1En()    const   { return _lcls1En;  }  
    uint8_t getByteMap()    const   { return _byteMap;  }

    bool    getIdleEn() const;
    void    setIdleEn(const bool en) const;

    // Read threshold register
    const uint32_t getThreshold(thr_channel_t ch) const;

    // Read threshold enable register
    const bool getThresholdEn(thr_channel_t ch) const;

    // Write threshold registers
    void setThreshold(thr_channel_t ch, const uint32_t val) const;

    // Write threshold enable register
    void setThresholdEn(thr_channel_t ch, const bool val) const;

private:

    // Channel information
    uint8_t _ch;
    uint8_t _thrCount;
    // bool    _idleEn;
    ScalVal _idleEn;
    bool    _altEn;
    bool    _lcls1En;
    uint8_t _byteMap;

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
    thr_channel_map_t _thrMap;
    // Threshold enable registers
    thr_channel_map_t _thrEnMap;
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