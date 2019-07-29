#ifndef LCLS2MPSLN_THR_H
#define LCLS2MPSLN_THR_H

/**
 *-----------------------------------------------------------------------------
 * Title      : MPS threshold class
 * ----------------------------------------------------------------------------
 * File       : l2Mps_thr.h
 * Author     : Jesus Vasquez, jvasquez@slac.stanford.edu
 * Created    : 2017-10-20
 * ----------------------------------------------------------------------------
 * Description:
 * Class for interfacing the MPS thershold registers.
 * ----------------------------------------------------------------------------
 * This file is part of l2Mps. It is subject to
 * the license terms in the LICENSE.txt file found in the top-level directory
 * of this distribution and at:
    * https://confluence.slac.stanford.edu/display/ppareg/LICENSE.html.
 * No part of l2Mps, including this file, may be
 * copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE.txt file.
 * ----------------------------------------------------------------------------
**/

#include <sstream>
#include <array>
#include <map>
#include <stdexcept>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

#include "l2Mps_cpsw.h"

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
    std::pair< bool, bool  >  minEn;
    std::pair< bool, bool  >  maxEn;
    std::pair< bool, float >  min;
    std::pair< bool, float >  max;
    std::pair< bool, uint32_t > minRaw;
    std::pair< bool, uint32_t > maxRaw;
};

// Threshold table data map
typedef std::map<thr_table_t, thr_tableData_t>      thr_chData_t;

// Threhold table scalval interfaces
struct thr_tableScalval_t
{
    CpswRegRW<uint8_t>  minEn;
    CpswRegRW<uint8_t>  maxEn;
    CpswRegRW<uint32_t> min;
    CpswRegRW<uint32_t> max;
};

// Threshold table scalval interfaces map
typedef std::map<thr_table_t, thr_tableScalval_t>   thr_chScalval_t;

// Threhold channel information data
struct thr_chInfoData_t
{
    int                        ch;
    std::pair< bool, uint8_t > count;
    std::pair< bool, uint8_t > byteMap;
    std::pair< bool, bool    > idleEn;
    std::pair< bool, bool    > altEn;
    std::pair< bool, bool    > lcls1En;
    float                      scaleSlope;
    float                      scaleOffset;
};

// Threshold channel information scalval interfaces
struct thr_chInfoScalval_t
{
    CpswRegRO<uint8_t> count;
    CpswRegRO<uint8_t> byteMap;
    CpswRegRW<uint8_t> idleEn;
    CpswRegRO<uint8_t> altEn;
    CpswRegRO<uint8_t> lcls1En;
};

// Threshold data (information + table data)
struct thr_ch_t
{
    thr_chInfoData_t info;
    thr_chData_t data;
};

// Threshold scalval interfaces (information + table data)
struct thr_scalval_t
{
    thr_chInfoScalval_t info;
    thr_chScalval_t     data;
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
    ~IThrChannel() {}

    // Factory method, which returns an smart pointer
    static ThrChannel create(Path mpsRoot, uint8_t channel);

    // Channel header
    uint8_t                    getChannel()             const { return ch;                              };
    std::pair< bool, uint8_t > getThrCount()            const { return thrScalvals.info.count.get();    };
    std::pair< bool, bool    > getIdleEn()              const { return thrScalvals.info.idleEn.get();   };
    bool                       setIdleEn(const bool en) const { return thrScalvals.info.idleEn.set(en); };
    std::pair< bool, bool    > getAltEn()               const { return thrScalvals.info.altEn.get();    };
    std::pair< bool, bool    > getLcls1En()             const { return thrScalvals.info.lcls1En.get();  };
    std::pair< bool, uint8_t > getByteMap()             const { return thrScalvals.info.byteMap.get();  };


    // Read threshold register
    std::pair< bool, float > getThresholdMin(thr_table_t ch);
    std::pair< bool, float > getThresholdMax(thr_table_t ch);

    // Read threshold enable register
    std::pair< bool, bool > getThresholdMinEn(thr_table_t ch);
    std::pair< bool, bool > getThresholdMaxEn(thr_table_t ch);

    // Write threshold registers
    bool setThresholdMin(thr_table_t ch, const float val);
    bool setThresholdMax(thr_table_t ch, const float val);

    // Write threshold enable register
    bool setThresholdMinEn(thr_table_t ch, const bool val);
    bool setThresholdMaxEn(thr_table_t ch, const bool val);

    // Set/get scale slope and offset
    bool        setScaleSlope(const float slope);
    const float getScaleSlope() const;
    bool        setScaleOffset(const float offset);
    const float getScaleOffset() const;

    void readAll(thr_ch_t& data) const;
    void readThrChInfo(thr_chInfoData_t& info) const;
    void readThrChData(thr_chData_t& data) const;


private:
    Path            chRoot;         // Root path to the channel register space
    int             ch;             // Threhold channel number
    thr_scalval_t   thrScalvals;    // Threshold scalval interfaces
    float           scaleSlope;     // Scale slope (egu/raw)
    float           scaleOffset;    // Scale offset (raw)

    // Helper functions
    thr_tableScalval_t createTableScalVal(const std::string& prefix) const;
    thr_tableScalval_t* findDataTableScalval(const thr_table_t& ch);
};

#endif
