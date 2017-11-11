#ifndef LCLS2MPSLN_MPS_H
#define LCLS2MPSLN_MPS_H

#include <stdio.h>
#include <iostream>
#include <string.h>
#include <iomanip>
#include <inttypes.h>
#include <map>
#include <stdexcept>
#include <boost/shared_ptr.hpp>
#include <cpsw_api_user.h>

class IMpsNode;

typedef boost::shared_ptr<const IMpsNode>     MpsNode;

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


class IMpsNode
{
public:
    // Constructor
    IMpsNode(Path mpsRoot);

    // Destructor
    ~IMpsNode();

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
    std::size_t const getLastMsgByteSize(void) const { return _lastMsgByteSize; }

private:
    Path       _mpsRoot;
    ScalVal    _mpsAppId;
    ScalVal    _mpsEnable;
    ScalVal    _lcls1Mode;
    ScalVal_RO _byteCount;
    ScalVal_RO _digitalEn;
    ScalVal    _beamDestMask;
    ScalVal    _altDestMask;

    ScalVal_RO _mpsTxLinkUpCnt;
    ScalVal_RO _mpsRxLinkUpCnt;
    ScalVal_RO _mpsTxLinkUP;
    ScalVal_RO _mpsRxLinkUP;
    ScalVal_RO _mpsSlotG;
    ScalVal_RO _appTypeG;
    ScalVal_RO _mpsPllLocked;
    ScalVal    _rollOverEn;

    ScalVal_RO _mpsTxPktSentCnt;
    ScalVal_RO _mpsRxPktRcvdSentCnt;

    ScalVal_RO _mpsMsgCnt;
    ScalVal_RO _mpsLastMsgAppId;
    ScalVal_RO _mpsLastMsgLcls;
    ScalVal_RO _mpsLastMsgTimestamp;
    ScalVal_RO _mpsLastMsgByte;
    std::size_t _lastMsgByteSize;
    // Command    RstCnt;


};

class MpsNodeFactory
{
public:
    // Mps node factory function
    static MpsNode create(Path mpsRoot)     { return MpsNode(new IMpsNode(mpsRoot)); }
};

#endif
