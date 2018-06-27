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
#include <boost/make_shared.hpp>
#include <cpsw_api_user.h>

#include "l2Mps_cpsw.h"

class IMpsNode;

typedef boost::shared_ptr<IMpsNode>     MpsNode;

// Name of the Base MPS module
const std::string MpsBaseModuleName = "AppMpsRegBase";

// Name of the SALT MPS module
const std::string MpsSaltModuleName = "AppMpsSalt";

// Application type
static std::map<int, std::string> appTypeList = {
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

// Data
struct mps_infoData_t
{
    // From AppMpsRegAppCh
    std::pair< bool, uint16_t             > appId;
    std::pair< bool, uint8_t              > version;
    std::pair< bool, bool                 > enable;
    std::pair< bool, bool                 > lcls1Mode;
    std::pair< bool, uint8_t              > byteCount;
    std::pair< bool, bool                 > digitalEn;
    std::pair< bool, uint16_t             > beamDestMask;
    std::pair< bool, uint16_t             > altDestMask;
    std::pair< bool, uint32_t             > msgCnt;
    std::pair< bool, uint16_t             > lastMsgAppId;
    std::pair< bool, bool                 > lastMsgLcls;
    std::pair< bool, uint16_t             > lastMsgTimestamp;
    std::pair< bool, std::vector<uint8_t> > lastMsgByte;
                                              
    // From AppMpsSalt                        
    std::pair< bool, bool                  > txLinkUp;
    std::pair< bool, uint32_t              > txLinkUpCnt;
    std::pair< bool, std::vector<bool>     > rxLinkUp;
    std::pair< bool, std::vector<uint32_t> > rxLinkUpCnt;
    std::pair< bool, bool                  > mpsSlot;
    std::pair< bool, std::string           > appType;
    std::pair< bool, bool                  > pllLocked;
    std::pair< bool, uint16_t              > rollOverEn;
    std::pair< bool, uint32_t              > txPktSentCnt;
    std::pair< bool, std::vector<uint32_t> > rxPktRcvdCnt;
};

typedef std::function<void(mps_infoData_t)> p_mpsCBFunc_t;

class IMpsNode
{
public:
    IMpsNode(Path mpsRoot);
    ~IMpsNode();

    const void readMpsInfo(mps_infoData_t& info) const;

    const void startPollThread(unsigned int poll, p_mpsCBFunc_t cbFunc);

    const size_t getLastMsgByteSize()  const { return lastMsgByte.getNelms();  };
    const size_t getRxLinkUpCntSize()  const { return rxLinkUpCnt.getNelms();  };
    const size_t getRxPktRcvdCntSize() const { return rxPktRcvdCnt.getNelms(); };

    // Mps Application ID
    std::pair<bool,uint16_t> getAppId(void) const                     { return appId.get();            };
    bool setAppId(const uint16_t id) const                            { return appId.set(id);          };

    // Mps version
    std::pair<bool,uint8_t>  getVersion(void) const                   { return version.get();          };
    bool setVersion(uint8_t ver) const                                { return version.set(ver);       };

    // Mps Enable
    std::pair<bool,bool> getEnable(void) const                        { return enable.get();           };
    bool setEnable(const bool en) const                               { return enable.set(en);         };

    // Lcls1 Mode (true = LCLS1 mode; false = LCLS2 Mode)
    std::pair<bool,bool>  getLcls1Mode(void) const                    { return lcls1Mode.get();        };
    bool  setLcls1Mode(const bool mode) const                         { return lcls1Mode.set(mode);    };

    // Number ofbytes in the MPS message
    std::pair<bool,uint8_t>  getByteCount(void) const                 { return byteCount.get();        };

    // Application generates digital messages
    std::pair<bool,bool> getDigitalEnable(void) const                 { return digitalEn.get();        };

    // Beam destination mask
    std::pair<bool,int16_t> getBeamDestMask(void) const               { return beamDestMask.get();     };
    bool setBeamDestMask(const uint16_t mask) const                   { return beamDestMask.set(mask); };

    // Alt destination mask
    std::pair<bool,uint8_t> getAltDestMask(void) const                { return altDestMask.get();      };
    bool setAltDestMask(const uint8_t mask) const                     { return altDestMask.set(mask);  };

    // Application type
    std::pair<bool,std::string> getAppType(void) const                { return getConvertedAppType();  };

    // MPS Tx Link Up counter
    std::pair<bool,uint32_t> getTxLinkUpCnt(void) const               { return txLinkUpCnt.get();      };

    // MPS Rx Link Up counter
    std::pair<bool,uint32_t> getRxLinkUpCnt(const uint8_t ch) const   { return rxLinkUpCnt.get(ch);    };

    // Status Counter Roll Over Enable
    std::pair<bool,uint32_t> getRollOverEn(void) const                { return rollOverEn.get();       };

    // Mps Tx LinkUp status
    std::pair<bool,bool> getTxLinkUp(void) const                      { return txLinkUp.get();         };
 
    // Mps Rx LinkUp status
    std::pair<bool,bool> getRxLinkUp(const uint8_t ch) const;

    // Mps Slot
    std::pair<bool,bool> getMpsSlot(void) const                       { return mpsSlot.get();          };

    // MPS PLL Lock Status
    std::pair<bool,bool> getPllLocked(void) const                     { return pllLocked.get();        };

    // Mps TX Packet Sent Counter
    std::pair<bool,uint32_t> getTxPktSentCnt(void) const              { return txPktSentCnt.get();     };

    // MPS RX Packet Received Counter
    std::pair<bool,uint32_t> getRxPktRcvdCnt(const uint8_t ch) const  { return rxPktRcvdCnt.get(ch);   };

    // MpsMessage counter
    std::pair<bool,uint32_t> getMsgCount(void) const                  { return msgCnt.get();           };

    // App ID in the last message
    std::pair<bool,int16_t> getLastMsgAppId(void) const               { return lastMsgAppId.get();     };

    // LCLS flag in the last message
    std::pair<bool,bool> getLastMsgLcls(void) const                   { return lastMsgLcls.get();      };

    // Timestamp in the last message
    std::pair<bool,uint16_t> getLastMsgTimeStamp(void) const          { return lastMsgTimestamp.get(); };

    // Bytes from the last message
    std::pair<bool,uint8_t> getLastMsgByte(const uint8_t index) const { return lastMsgByte.get(index); };

    // Reset the SALT conuters
    bool resetSaltCnt(void) const                                     { return rstCnt.exe();           };

    // Reset the SALT PLL
    bool resetSaltPll(void) const                                     { return rstPll.exe();           };

private:
    p_mpsCBFunc_t       mpsCB;
    unsigned int        pollCB;
    boost::atomic<bool> run;
    std::thread         scanThread;

    // CPSW interface from AppMpsRegAppCh
    CpswRegRW<uint16_t> appId;
    CpswRegRW<uint8_t>  version;
    CpswRegRW<uint8_t>  enable;
    CpswRegRW<uint8_t>  lcls1Mode;
    CpswRegRO<uint8_t>  byteCount;
    CpswRegRO<uint8_t>  digitalEn;
    CpswRegRW<uint16_t> beamDestMask;
    CpswRegRW<uint16_t> altDestMask;
    CpswRegRO<uint32_t> msgCnt;
    CpswRegRO<uint16_t> lastMsgAppId;
    CpswRegRO<uint8_t>  lastMsgLcls;
    CpswRegRO<uint16_t> lastMsgTimestamp;
    CpswRegRO<uint8_t>  lastMsgByte;
    // CPSW interfaces from AppMpsSalt
    CpswRegRO<uint8_t>  txLinkUp;
    CpswRegRO<uint32_t> txLinkUpCnt;
    CpswRegRO<uint8_t>  rxLinkUp;
    CpswRegRO<uint32_t> rxLinkUpCnt;
    CpswRegRO<uint8_t>  mpsSlot;
    CpswRegRO<uint8_t>  appType;
    CpswRegRO<uint8_t>  pllLocked;
    CpswRegRW<uint16_t> rollOverEn;
    CpswRegRO<uint32_t> txPktSentCnt;
    CpswRegRO<uint32_t> rxPktRcvdCnt;
    CpswCmd             rstCnt;
    CpswCmd             rstPll;

    void                        pollThread();
    std::pair<bool,std::string> getConvertedAppType() const;
};

class MpsNodeFactory
{
public:
    // Mps node factory function
    static MpsNode create(Path mpsRoot)     { return boost::make_shared<IMpsNode>(mpsRoot); }
};

#endif
