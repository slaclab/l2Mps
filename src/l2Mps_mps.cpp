#include "l2Mps_mps.h"

IMpsNode::IMpsNode(Path mpsRoot) :
    _mpsRoot             ( mpsRoot-> clone()                                             ),
    _mpsAppId            ( IScalVal::create(    _mpsRoot->findByName("AppMpsThr/mpsAppId") )         ),
    _mpsEnable           ( IScalVal::create(    _mpsRoot->findByName("AppMpsThr/mpsEnable") )        ),
    _lcls1Mode           ( IScalVal::create(    _mpsRoot->findByName("AppMpsThr/lcls1Mode") )        ),
    _byteCount           ( IScalVal_RO::create( _mpsRoot->findByName("AppMpsThr/byteCount") )        ),
    _digitalEn           ( IScalVal_RO::create( _mpsRoot->findByName("AppMpsThr/digitalEn") )        ),
    _beamDestMask        ( IScalVal::create(    _mpsRoot->findByName("AppMpsThr/beamDestMask") )     ),
    _altDestMask         ( IScalVal::create(    _mpsRoot->findByName("AppMpsThr/altDestMask") )      ),
    _mpsTxLinkUpCnt      ( IScalVal_RO::create( _mpsRoot->findByName("AppMpsSalt/MpsTxLinkUpCnt") )  ),
    _mpsRxLinkUpCnt      ( IScalVal_RO::create( _mpsRoot->findByName("AppMpsSalt/MpsRxLinkUpCnt") )  ),
    _mpsTxLinkUP         ( IScalVal_RO::create( _mpsRoot->findByName("AppMpsSalt/MpsTxLinkUP") )     ),
    _mpsRxLinkUP         ( IScalVal_RO::create( _mpsRoot->findByName("AppMpsSalt/MpsRxLinkUP") )     ),
    _mpsSlotG            ( IScalVal_RO::create( _mpsRoot->findByName("AppMpsSalt/MPS_SLOT_G") )      ),
    _appTypeG            ( IScalVal_RO::create( _mpsRoot->findByName("AppMpsSalt/APP_TYPE_G") )      ),
    _mpsPllLocked        ( IScalVal_RO::create( _mpsRoot->findByName("AppMpsSalt/MpsPllLocked") )    ),
    _rollOverEn          ( IScalVal::create(    _mpsRoot->findByName("AppMpsSalt/RollOverEn") )      ),
    _mpsTxPktSentCnt     ( IScalVal_RO::create( _mpsRoot->findByName("AppMpsSalt/MpsTxPktSentCnt") ) ),
    _mpsRxPktRcvdSentCnt ( IScalVal_RO::create( _mpsRoot->findByName("AppMpsSalt/MpsRxPktRcvdSentCnt") )  ),
    
    _mpsMsgCnt           ( IScalVal_RO::create( _mpsRoot->findByName("AppMpsThr/mpsMsgCount") )         ),
    _mpsLastMsgAppId     ( IScalVal_RO::create( _mpsRoot->findByName("AppMpsThr/lastMsgAppId") )        ),
    _mpsLastMsgLcls      ( IScalVal_RO::create( _mpsRoot->findByName("AppMpsThr/lastMsgLcls") )         ),
    _mpsLastMsgTimestamp ( IScalVal_RO::create( _mpsRoot->findByName("AppMpsThr/lastMsgTimeStamp") )    ),
    _mpsLastMsgByte      ( IScalVal_RO::create( _mpsRoot->findByName("AppMpsThr/lastMsgByte") )         ),
    _lastMsgByteSize     ( _mpsLastMsgByte->getNelms() )
{
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
