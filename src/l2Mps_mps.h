#ifndef LCLS2MPSLN_MPS_H
#define LCLS2MPSLN_MPS_H

#define _GLIBCXX_USE_NANOSLEEP    // Workaround to use std::this_thread::sleep_for

#include <stdio.h>
#include <iostream>
#include <string.h>
#include <iomanip>
#include <inttypes.h>
#include <map>
#include <vector>
#include <stdexcept>
#include <thread>
#include <chrono>
#include <boost/atomic.hpp>
#include <boost/shared_ptr.hpp>
#include <cpsw_api_user.h>

#include "l2Mps_base.h"

class IMpsNode;

typedef boost::shared_ptr<IMpsNode>     MpsNode;

// Name of the Base MPS module
const std::string MpsBaseModuleName = "AppMpsRegBase";

// Name of the SALT MPS module
const std::string MpsSaltModuleName = "AppMpsSalt";

// Application type
static std::map<int, std::string> appType = {
    {0,     "NONE"},
    {1,     "DEDBUG"},
    {10,    "TIME_GEN"},
    {11,    "BCM"},
    {12,    "BLEN"},
    {13,    "LLRF"},
    {14,    "EXTREF_GEN"},
    {100,   "BPM"},
    {101,   "BPM"},
    {120,   "MPS_24CH"},
    {121,   "MPS_6CH"}
};

// CPSW interfaces
struct mps_infoScalval_t
{
    // From AppMpsRegAppCh
    ScalVal     appId;
    ScalVal     version;
    ScalVal     enable;
    ScalVal     lcls1Mode;
    ScalVal_RO  byteCount;
    ScalVal_RO  digitalEn;
    ScalVal     beamDestMask;
    ScalVal     altDestMask;
    ScalVal_RO  msgCnt;
    ScalVal_RO  lastMsgAppId;
    ScalVal_RO  lastMsgLcls;
    ScalVal_RO  lastMsgTimestamp;
    ScalVal_RO  lastMsgByte;
    // From AppMpsSalt
    ScalVal_RO  txLinkUp;
    ScalVal_RO  txLinkUpCnt;
    ScalVal_RO  rxLinkUp;
    ScalVal_RO  rxLinkUpCnt;
    ScalVal_RO  mpsSlot;
    ScalVal_RO  appType;
    ScalVal_RO  pllLocked;
    ScalVal     rollOverEn;
    ScalVal_RO  txPktSentCnt;
    ScalVal_RO  rxPktRcvdCnt;
    Command     rstCnt;
    Command     rstPll;
};

// Data
struct mps_infoData_t
{
    // From AppMpsRegAppCh
    uint16_t                appId;
    uint8_t                 version;
    bool                    enable;
    bool                    lcls1Mode;
    uint8_t                 byteCount;
    bool                    digitalEn;
    uint16_t                beamDestMask;
    uint16_t                altDestMask;
    uint32_t                msgCnt;
    uint16_t                lastMsgAppId;
    bool                    lastMsgLcls;
    uint16_t                lastMsgTimestamp;
    std::vector<uint8_t>    lastMsgByte;

    // From AppMpsSalt
    bool                    txLinkUp;
    uint32_t                txLinkUpCnt;
    std::vector<bool>       rxLinkUp;
    std::vector<uint32_t>   rxLinkUpCnt;
    bool                    mpsSlot;
    std::string             appType;
    bool                    pllLocked;
    uint16_t                rollOverEn;
    uint32_t                txPktSentCnt;
    std::vector<uint32_t>   rxPktRcvdCnt;
};

typedef std::function<void(mps_infoData_t)> p_mpsCBFunc_t;

class IMpsNode
{
public:
    // Constructor
    IMpsNode(Path mpsRoot);

    // Destructor
    ~IMpsNode();

    const void readMpsInfo(mps_infoData_t& info) const;

    const void startPollThread(unsigned int poll, p_mpsCBFunc_t cbFunc);

    const std::size_t getLastMsgByteSize()  const { return lastMsgByteSize;  };
    const std::size_t getRxLinkUpCntSize()  const { return rxLinkUpCntSize;  };
    const std::size_t getRxPktRcvdCntSize() const { return rxPktRcvdCntSize; };

    // Mps Application ID
    uint16_t const getAppId(void) const                         { return IMpsBase::get(scalvals.appId);                     };
    void setAppId(const uint16_t id) const                      { IMpsBase::set(scalvals.appId, id);                        };

    // Mps version
    uint8_t const getVersion(void) const                        { return IMpsBase::get(scalvals.version);                   };
    void setVersion(uint8_t ver) const                          {IMpsBase::set(scalvals.version, ver);                      };

    // Mps Enable
    bool const getEnable(void) const                            { return IMpsBase::get(scalvals.enable)?true:false;         };
    void setEnable(const bool en) const                         { IMpsBase::set(scalvals.enable, en);                       };

    // Lcls1 Mode (true = LCLS1 mode; false = LCLS2 Mode)
    bool const getLcls1Mode(void) const                         { return IMpsBase::get(scalvals.lcls1Mode)?true:false;      };
    void setLcls1Mode(const bool mode) const                    { IMpsBase::set(scalvals.lcls1Mode, mode);                  };

    // Number ofbytes in the MPS message
    uint8_t const getByteCount(void) const                      { return IMpsBase::get(scalvals.byteCount);                 };

    // Application generates digital messages
    bool const getDigitalEnable(void) const                     { return IMpsBase::get(scalvals.digitalEn)?true:false;      };

    // Beam destination mask
    uint16_t const getBeamDestMask(void) const                  { return IMpsBase::get(scalvals.beamDestMask);              };
    void setBeamDestMask(const uint16_t mask) const             { IMpsBase::set(scalvals.beamDestMask, mask);               };

    // Alt destination mask
    uint8_t const getAltDestMask(void) const                    { return IMpsBase::get(scalvals.altDestMask);               };
    void setAltDestMask(const uint8_t mask) const               { IMpsBase::set(scalvals.altDestMask, mask);                };

    // Application type
    std::string const getAppType(void) const;

    // MPS Tx Link Up counter
    uint32_t const getTxLinkUpCnt(void) const                   { return IMpsBase::get(scalvals.txLinkUpCnt);               };

    // MPS Rx Link Up counter
    uint32_t const getRxLinkUpCnt(const uint8_t ch) const       { return IMpsBase::get(scalvals.rxLinkUpCnt, ch);           };

    // Status Counter Roll Over Enable
    uint32_t const getRollOverEn(void) const                    { return IMpsBase::get(scalvals.rollOverEn);                };

    // Mps Tx LinkUp status
    bool     const getTxLinkUp(void) const                      { return IMpsBase::get(scalvals.txLinkUp)?true:false;       };

    // Mps Rx LinkUp status
    bool     const getRxLinkUp(const uint8_t ch) const;

    // Mps Slot
    bool     const getMpsSlot(void) const                       { return IMpsBase::get(scalvals.mpsSlot)?true:false;        };

    // MPS PLL Lock Status
    bool     const getPllLocked(void) const                     { return IMpsBase::get(scalvals.pllLocked)?true:false;      };

    // Mps TX Packet Sent Counter
    uint32_t const getTxPktSentCnt(void) const                  { return IMpsBase::get(scalvals.txPktSentCnt);              };

    // MPS RX Packet Received Counter
    uint32_t const getRxPktRcvdCnt(const uint8_t ch) const      { return IMpsBase::get(scalvals.rxPktRcvdCnt, ch);          };

    // MpsMessage counter
    uint32_t const getMsgCount(void) const                      { return IMpsBase::get(scalvals.msgCnt);                    };

    // App ID in the last message
    uint16_t const getLastMsgAppId(void) const                  { return IMpsBase::get(scalvals.lastMsgAppId);              };

    // LCLS flag in the last message
    bool     const getLastMsgLcls(void) const                   { return IMpsBase::get(scalvals.lastMsgLcls)?true:false;    };

    // Timestamp in the last message
    uint16_t const getLastMsgTimeStamp(void) const              { return IMpsBase::get(scalvals.lastMsgTimestamp);          };

    // Bytes from the last message
    uint8_t  const getLastMsgByte(const uint8_t index) const    { return IMpsBase::get(scalvals.lastMsgByte, index);        };

    // Reset the SALT conuters
    void const resetSaltCnt(void) const                         { return IMpsBase::execute(scalvals.rstCnt);                };

    // Reset the SALT PLL
    void const resetSaltPll(void) const                         { return IMpsBase::execute(scalvals.rstPll);                };

private:
    Path                mpsRoot;
    std::size_t         lastMsgByteSize;
    std::size_t         rxLinkUpCntSize;
    std::size_t         rxPktRcvdCntSize;
    mps_infoScalval_t   scalvals;
    p_mpsCBFunc_t       mpsCB;
    unsigned int        pollCB;
    std::thread         scanThread;
    boost::atomic<bool> run;

    void pollThread();
};

class MpsNodeFactory
{
public:
    // Mps node factory function
    static MpsNode create(Path mpsRoot)     { return MpsNode(new IMpsNode(mpsRoot)); }
};

#endif
