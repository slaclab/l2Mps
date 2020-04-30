#ifndef L2MPS_MPS_H
#define L2MPS_MPS_H

/**
 *-----------------------------------------------------------------------------
 * Title      : Common Platform MPS module class.
 * ----------------------------------------------------------------------------
 * File       : l2Mps_mps.h
 * Author     : Jesus Vasquez, jvasquez@slac.stanford.edu
 * Created    : 2017-10-20
 * ----------------------------------------------------------------------------
 * Description:
 * Class for interfacing the MPS register common to all applications, included
 * as part of the Common Platform.
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
#include <boost/any.hpp>
#include <cpsw_api_user.h>

#include "l2Mps_cpsw.h"
#include "l2Mps_bsi.h"
#include "l2Mps_bpm.h"
#include "l2Mps_blen.h"
#include "l2Mps_bcm.h"
#include "l2Mps_blm.h"
#include "l2Mps_soft_inputs.h"

class IMpsNode;

typedef boost::shared_ptr<IMpsNode>     MpsNode;

// Name of the Base MPS module
const std::string MpsBaseModuleName("AppMpsRegBase/");

// Name of the SALT MPS module
const std::string MpsSaltModuleName("AppMpsSalt/");

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

// Number of AMC bays on a carrier
const uint8_t numberOfBays(2);

// Default MPS Root Path
const std::string defaultMpsRootPath("mmio/AmcCarrierCore/AppMps/");

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

    // Soft input data (used by Link Node applications)
    si_data_t                                softInputs;
};

typedef std::function<void(mps_infoData_t)> p_mpsCBFunc_t;

class IMpsNode
{
public:
    IMpsNode(Path root);
    ~IMpsNode();

    // Factory method, which returns a smart pointer
    static MpsNode create(Path root);

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

    // Get the application object
    boost::any getBayApp(uint8_t bay)                                 { return amc[bay];               };

    // Get the crate ID
    std::pair<bool,uint16_t> getCrateId() const                       { return mpsBsi->getCrateId();   };

    // Get the Slot Numbe
    std::pair<bool,uint8_t> getSlotNumber() const                     { return mpsBsi->getSlotNumber(); };

    // Get the soft input object
    MpsSoftInputs getMpsSoftInputs() const                            { return mpsSoftInputs;           };

    // Get a copy of the MPS root path
    Path getMpsRoot() const                                           { return mpsRoot->clone();        };

    // Load configuration (CPSW's YAML) file
    const uint64_t loadConfigFile(std::string fileName) const;

private:
    p_mpsCBFunc_t       mpsCB;
    unsigned int        pollCB;
    boost::atomic<bool> run;
    std::thread         scanThread;

    // Root path
    Path                cpswRoot;

    // MPS root path
    Path                mpsRoot;

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

    // Application type name
    std::string         appTypeName;

    // Application object (one for each bay)
    boost::any          amc[numberOfBays];

    // BSI interface
    MpsBsi              mpsBsi;

    // Soft inputs object (used only for LN applications)
    MpsSoftInputs       mpsSoftInputs;

    void                        pollThread();
    std::pair<bool,std::string> getConvertedAppType() const;

};

#endif
