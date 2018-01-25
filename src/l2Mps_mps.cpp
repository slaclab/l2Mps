#include "l2Mps_mps.h"

IMpsNode::IMpsNode(Path mpsRoot) :
    // MPS root path
    _mpsRoot             ( mpsRoot->clone())

{
    // MPS Base interfaces
    _mpsAppId            = IMpsBase::createInterface<ScalVal>(    _mpsRoot, MpsBaseModuleName + "/mpsAppId");
    _mpsEnable           = IMpsBase::createInterface<ScalVal>(    _mpsRoot, MpsBaseModuleName + "/mpsEnable");
    _lcls1Mode           = IMpsBase::createInterface<ScalVal>(    _mpsRoot, MpsBaseModuleName + "/lcls1Mode");
    _byteCount           = IMpsBase::createInterface<ScalVal_RO>( _mpsRoot, MpsBaseModuleName + "/byteCount");
    _digitalEn           = IMpsBase::createInterface<ScalVal_RO>( _mpsRoot, MpsBaseModuleName + "/digitalEn");
    _beamDestMask        = IMpsBase::createInterface<ScalVal>(    _mpsRoot, MpsBaseModuleName + "/beamDestMask");
    _altDestMask         = IMpsBase::createInterface<ScalVal>(    _mpsRoot, MpsBaseModuleName + "/altDestMask");
    _mpsMsgCnt           = IMpsBase::createInterface<ScalVal_RO>( _mpsRoot, MpsBaseModuleName + "/mpsMsgCount");
    _mpsLastMsgAppId     = IMpsBase::createInterface<ScalVal_RO>( _mpsRoot, MpsBaseModuleName + "/lastMsgAppId");
    _mpsLastMsgLcls      = IMpsBase::createInterface<ScalVal_RO>( _mpsRoot, MpsBaseModuleName + "/lastMsgLcls");
    _mpsLastMsgTimestamp = IMpsBase::createInterface<ScalVal_RO>( _mpsRoot, MpsBaseModuleName + "/lastMsgTimeStamp");
    _mpsLastMsgByte      = IMpsBase::createInterface<ScalVal_RO>( _mpsRoot, MpsBaseModuleName + "/lastMsgByte");

    if (_mpsLastMsgByte)
    {
        try
        {
            _lastMsgByteSize = _mpsLastMsgByte->getNelms();
        }
        catch (CPSWError &e)
        {
            std::cout << "Couldn't read the number of elements from the last message: " << e.getInfo() << std::endl;
        }
    }

    // MPS SALT interfaces
    _mpsTxLinkUpCnt      = IMpsBase::createInterface<ScalVal_RO>( _mpsRoot, MpsSaltModuleName + "/MpsTxLinkUpCnt");
    _mpsRxLinkUpCnt      = IMpsBase::createInterface<ScalVal_RO>( _mpsRoot, MpsSaltModuleName + "/MpsRxLinkUpCnt");
    _mpsTxLinkUP         = IMpsBase::createInterface<ScalVal_RO>( _mpsRoot, MpsSaltModuleName + "/MpsTxLinkUP");
    _mpsRxLinkUP         = IMpsBase::createInterface<ScalVal_RO>( _mpsRoot, MpsSaltModuleName + "/MpsRxLinkUP");
    _mpsSlotG            = IMpsBase::createInterface<ScalVal_RO>( _mpsRoot, MpsSaltModuleName + "/MPS_SLOT_G");
    _appTypeG            = IMpsBase::createInterface<ScalVal_RO>( _mpsRoot, MpsSaltModuleName + "/APP_TYPE_G");
    _mpsPllLocked        = IMpsBase::createInterface<ScalVal_RO>( _mpsRoot, MpsSaltModuleName + "/MpsPllLocked");
    _rollOverEn          = IMpsBase::createInterface<ScalVal>(    _mpsRoot, MpsSaltModuleName + "/RollOverEn");
    _mpsTxPktSentCnt     = IMpsBase::createInterface<ScalVal_RO>( _mpsRoot, MpsSaltModuleName + "/MpsTxPktSentCnt");
    _mpsRxPktRcvdSentCnt = IMpsBase::createInterface<ScalVal_RO>( _mpsRoot, MpsSaltModuleName + "/MpsRxPktRcvdCnt");
    _rstCnt              = IMpsBase::createInterface<Command>(    _mpsRoot, MpsSaltModuleName + "/RstCnt");
    _rstPll              = IMpsBase::createInterface<Command>(    _mpsRoot, MpsSaltModuleName + "/RstPll");
    
}


IMpsNode::~IMpsNode()
{
    std::cout << "MPS node destroyed" << std::endl;
}

uint16_t const IMpsNode::getAppId(void) const
{
    if (!_mpsAppId)
        throw std::runtime_error("Register interface not implemented\n");

    uint32_t reg;
    _mpsAppId->getVal(&reg);
    return reg;
}

void IMpsNode::setAppId(const uint16_t id) const
{
    if (!_mpsAppId)
        throw std::runtime_error("Register interface not implemented\n");

    _mpsAppId->setVal(id);
}

bool const IMpsNode::getEnable() const
{
    if (!_mpsEnable)
        throw std::runtime_error("Register interface not implemented\n");

    uint8_t reg;
    _mpsEnable->getVal(&reg);
    return reg ? true : false;
}

void IMpsNode::setEnable(const bool en) const
{
    if (!_mpsEnable)
        throw std::runtime_error("Register interface not implemented\n");

     _mpsEnable->setVal((uint8_t)en);
}

bool const IMpsNode::getLcls1Mode(void) const
{ 
    if (!_lcls1Mode)
        throw std::runtime_error("Register interface not implemented\n");

    uint8_t reg;
    _lcls1Mode->getVal(&reg);
    return reg ? true : false;
}

void IMpsNode::setLcls1Mode(const bool mode) const
{ 
    if (!_lcls1Mode)
        throw std::runtime_error("Register interface not implemented\n");

    _lcls1Mode->setVal((uint8_t)mode);
}

uint8_t const IMpsNode::getByteCount(void) const
{ 
    if (!_byteCount)
        throw std::runtime_error("Register interface not implemented\n");

    uint8_t reg;
    _byteCount->getVal(&reg);
    return reg;
}

bool const IMpsNode::getDigitalEnable(void) const
{
    if (!_digitalEn)
        throw std::runtime_error("Register interface not implemented\n");

    uint8_t reg;
    _digitalEn->getVal(&reg);
    return reg ? true : false;
}

uint16_t const IMpsNode::getBeamDestMask(void) const
{
    if (!_beamDestMask)
        throw std::runtime_error("Register interface not implemented\n");

    uint16_t reg;
    _beamDestMask->getVal(&reg);
    return reg;
}

void IMpsNode::setBeamDestMask(const uint16_t mask) const
{
    if (!_beamDestMask)
        throw std::runtime_error("Register interface not implemented\n");

    _beamDestMask->setVal(mask);
}

uint8_t const IMpsNode::getAltDestMask(void) const
{
    if (!_altDestMask)
        throw std::runtime_error("Register interface not implemented\n");

    uint8_t reg;
    _altDestMask->getVal(&reg);
    return reg;
}

void IMpsNode::setAltDestMask(const uint8_t mask) const
{
    if (!_altDestMask)
        throw std::runtime_error("Register interface not implemented\n");

    _altDestMask->setVal(mask);
}

std::string const IMpsNode::getAppType(void) const
{
    if (!_appTypeG)
        throw std::runtime_error("Register interface not implemented\n");

    uint8_t reg;
    _appTypeG->getVal(&reg);

    std::map<int, std::string>::iterator it = appType.find(reg);
    if (it != appType.end())
        return (*it).second;
    else
        return std::string();
}

///////////////////////////

uint32_t const IMpsNode::getTxLinkUpCnt(void) const
{
    if (!_mpsTxLinkUpCnt)
        throw std::runtime_error("Register interface not implemented\n");

    uint32_t reg;
    _mpsTxLinkUpCnt->getVal(&reg);
    return reg;
}

uint32_t const IMpsNode::getRxLinkUpCnt(const uint8_t ch) const
{
    if (!_mpsRxLinkUpCnt)
        throw std::runtime_error("Register interface not implemented\n");

    uint8_t n = _mpsRxLinkUpCnt->getNelms();

    if (ch > n)
        throw std::runtime_error("Register index aout of range\n");

    uint32_t reg[n];
    _mpsRxLinkUpCnt->getVal(reg, n);
    return reg[ch];
}

uint32_t const IMpsNode::getRollOverEn(void) const
{
    if (!_rollOverEn)
        throw std::runtime_error("Register interface not implemented\n");

    uint32_t reg;
    _rollOverEn->getVal(&reg);
    return reg;
}

bool const IMpsNode::getTxLinkUp(void) const
{
    if (!_mpsTxLinkUP)
        throw std::runtime_error("Register interface not implemented\n");
    
    uint8_t reg;
    _mpsTxLinkUP->getVal(&reg);
    return reg ? true : false;
}

bool const IMpsNode::getRxLinkUp(const uint8_t ch) const
{
    if (!_mpsRxLinkUP)
        throw std::runtime_error("Register interface not implemented\n");

    uint8_t n = _mpsRxLinkUP->getSizeBits();
    uint16_t mask = (1<<ch);

    if (ch > n)
        throw std::runtime_error("Register index aout of range\n");

    uint8_t reg;
    _mpsRxLinkUP->getVal(&reg);
    return (reg & mask) ? true : false;
}

bool const IMpsNode::getMpsSlot(void) const
{
    if (!_mpsSlotG)
        throw std::runtime_error("Register interface not implemented\n");
    
    uint8_t reg;
    _mpsSlotG->getVal(&reg);
    return reg ? true : false;
}

bool const IMpsNode::getPllLocked(void) const
{
    if (!_mpsPllLocked)
        throw std::runtime_error("Register interface not implemented\n");
    
    uint8_t reg;
    _mpsPllLocked->getVal(&reg);
    return reg ? true : false;
}

uint32_t const IMpsNode::getTxPktSentCnt(void) const
{
    if (!_mpsTxPktSentCnt)
        throw std::runtime_error("Register interface not implemented\n");

    uint32_t reg;
    _mpsTxPktSentCnt->getVal(&reg);
    return reg;
}
    
uint32_t const IMpsNode::getRxPktRcvdSentCnt(const uint8_t ch) const
{
      if (!_mpsRxPktRcvdSentCnt)
        throw std::runtime_error("Register interface not implemented\n");
    
    uint8_t n = _mpsRxPktRcvdSentCnt->getNelms();

    if (ch > n)
        throw std::runtime_error("Register index aout of range\n");

    uint32_t reg[n];
    _mpsRxPktRcvdSentCnt->getVal(reg, n);

    return reg[ch];
}

uint32_t const IMpsNode::getMpsMsgCount(void) const
{
    if (!_mpsMsgCnt)
        throw std::runtime_error("Register interface not implemented\n");

    uint32_t reg;
    _mpsMsgCnt->getVal(&reg);
    return reg;
}

uint16_t const IMpsNode::getLastMsgAppId(void) const
{
    if (!_mpsLastMsgAppId)
        throw std::runtime_error("Register interface not implemented\n");

    uint16_t reg;
    _mpsLastMsgAppId->getVal(&reg);
    return reg;
}

bool const IMpsNode::getLastMsgLcls(void) const
{
    if (!_mpsLastMsgLcls)
        throw std::runtime_error("Register interface not implemented\n");
    uint8_t reg;
    _mpsLastMsgLcls->getVal(&reg);
    return reg ? true : false;
}

uint16_t const IMpsNode::getLastMsgTimeStamp(void) const
{
    if (!_mpsLastMsgTimestamp)
        throw std::runtime_error("Register interface not implemented\n");
    uint16_t reg;
    _mpsLastMsgTimestamp->getVal(&reg);
    return reg;
}

uint8_t const IMpsNode::getLastMsgByte(const uint8_t index) const
{
    if (!_mpsLastMsgByte)
        throw std::runtime_error("Register interface not implemented\n");

    if (index > _lastMsgByteSize)
        throw std::runtime_error("LastMsgByte: request index is out of range \n");

    uint8_t reg[_lastMsgByteSize];
    _mpsLastMsgByte->getVal(reg, _lastMsgByteSize);
    return reg[index];
}

void const IMpsNode::resetSaltCnt(void) const
{
    if (!_rstCnt)
        throw std::runtime_error("Command interface not implemented\n");

    _rstCnt->execute();
}

void const IMpsNode::resetSaltPll(void) const
{
    if (!_rstPll)
        throw std::runtime_error("Command interface not implemented\n");

    _rstPll->execute();
}