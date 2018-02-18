#ifndef LCLS2MPSLN_MPS_H
#define LCLS2MPSLN_MPS_H

#include <stdio.h>
#include <iostream>
#include <string.h>
#include <iomanip>
#include <inttypes.h>
#include <map>
#include <vector>
#include <stdexcept>
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

struct mps_infoScalval_t
{
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

struct mps_infoData_t
{
    uint16_t    appId;
    uint8_t     version;
    bool        enable;
    bool        lcls1Mode;
    uint8_t     byteCount;
    bool        digitalEn;
    uint16_t    beamDestMask;
    uint16_t    altDestMask;
    uint32_t    msgCnt;
    uint16_t    lastMsgAppId;
    bool        lastMsgLcls;
    uint16_t    lastMsgTimestamp;
    std::vector<uint8_t> lastMsgByte;

    bool        txLinkUp;
    uint32_t    txLinkUpCnt;
    uint32_t    rxLinkUp;
    std::vector<uint32_t> rxLinkUpCnt;
    bool        mpsSlot;
    std::string appType;
    bool        pllLocked;
    uint16_t    rollOverEn;
    uint32_t    txPktSentCnt;
    std::vector<uint32_t> rxPktRcvdCnt;
};

typedef void (*p_mpsCBFunc_t)(mps_infoData_t);

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
    uint16_t const getAppId(void) const;
    void setAppId(const uint16_t id) const;

    // Mps Enable
    bool const getEnable() const;
    void setEnable(const bool en) const;

    // Lcls1 Mode (true = LCLS1 mode; false = LCLS2 Mode)
    bool const getLcls1Mode(void) const;
    void setLcls1Mode(const bool mode) const;

    // Number ofbytes in the MPS message
    uint8_t const getByteCount(void) const;

    // Application generates digital messages
    bool const getDigitalEnable(void) const;

    // Beam destination mask
    uint16_t const getBeamDestMask(void) const;
    void setBeamDestMask(const uint16_t mask) const;

    // Alt destination mask
    uint8_t const getAltDestMask(void) const;
    void setAltDestMask(const uint8_t mask) const;

    // Application type
    std::string const getAppType(void) const;

    // MPS Tx Link Up counter
    uint32_t const getTxLinkUpCnt(void) const;
    
    // MPS Rx Link Up counter
    uint32_t const getRxLinkUpCnt(const uint8_t ch) const;
    
    // Status Counter Roll Over Enable
    uint32_t const getRollOverEn(void) const;

    // Mps Tx LinkUp status
    bool     const getTxLinkUp(void) const;
    
    // Mps Rx LinkUp status
    bool     const getRxLinkUp(const uint8_t ch) const;
    
    // Mps Slot
    bool     const getMpsSlot(void) const;
    
    // MPS PLL Lock Status
    bool     const getPllLocked(void) const;

    // Mps TX Packet Sent Counter
    uint32_t const getTxPktSentCnt(void) const;
    
    // MPS RX Packet Received Counter
    uint32_t const getRxPktRcvdSentCnt(const uint8_t ch) const;

    // MpsMessage counter
    uint32_t const getMpsMsgCount(void) const;

    // App ID in the last message
    uint16_t const getLastMsgAppId(void) const;

    // LCLS flag in the last message
    bool     const getLastMsgLcls(void) const;

    // Timestamp in the last message
    uint16_t const getLastMsgTimeStamp(void) const;

    // Bytes from the last message
    uint8_t  const getLastMsgByte(const uint8_t index) const;
    // std::size_t const getLastMsgByteSize(void) const { return lastMsgByteSize; }

    // Reset the SALT conuters
    void const resetSaltCnt(void) const;

    // Reset the SALT PLL
    void const resetSaltPll(void) const;

private:
    Path       _mpsRoot;

    mps_infoScalval_t scalvals;
    std::size_t lastMsgByteSize;
    std::size_t rxLinkUpCntSize;
    std::size_t rxPktRcvdCntSize;

    p_mpsCBFunc_t   mpsCB;
    unsigned int    pollCB;
    pthread_t       scanThread;

    void pollThread();
    static void *createThread(void* p) { static_cast<IMpsNode*>(p)->pollThread(); return NULL; };

};

class MpsNodeFactory
{
public:
    // Mps node factory function
    static MpsNode create(Path mpsRoot)     { return MpsNode(new IMpsNode(mpsRoot)); }
};

#endif
