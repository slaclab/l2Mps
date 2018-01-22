#include "l2Mps_mps.h"

IMpsNode::IMpsNode(Path mpsRoot) :

    // MPS root path
    _mpsRoot             ( mpsRoot-> clone()                                             ),

    // MPS Base interfaces
    _mpsAppId            ( IScalVal::create(    _mpsRoot->findByName( std::string(MpsBaseModuleName + "/mpsAppId").c_str()            ))),
    _mpsEnable           ( IScalVal::create(    _mpsRoot->findByName( std::string(MpsBaseModuleName + "/mpsEnable").c_str()           ))),
    _lcls1Mode           ( IScalVal::create(    _mpsRoot->findByName( std::string(MpsBaseModuleName + "/lcls1Mode").c_str()           ))),
    _byteCount           ( IScalVal_RO::create( _mpsRoot->findByName( std::string(MpsBaseModuleName + "/byteCount").c_str()           ))),
    _digitalEn           ( IScalVal_RO::create( _mpsRoot->findByName( std::string(MpsBaseModuleName + "/digitalEn").c_str()           ))),
    _beamDestMask        ( IScalVal::create(    _mpsRoot->findByName( std::string(MpsBaseModuleName + "/beamDestMask").c_str()        ))),
    _altDestMask         ( IScalVal::create(    _mpsRoot->findByName( std::string(MpsBaseModuleName + "/altDestMask").c_str()         ))),
    _mpsMsgCnt           ( IScalVal_RO::create( _mpsRoot->findByName( std::string(MpsBaseModuleName + "/mpsMsgCount").c_str()          ))),
    _mpsLastMsgAppId     ( IScalVal_RO::create( _mpsRoot->findByName( std::string(MpsBaseModuleName + "/lastMsgAppId").c_str()         ))),
    _mpsLastMsgLcls      ( IScalVal_RO::create( _mpsRoot->findByName( std::string(MpsBaseModuleName + "/lastMsgLcls").c_str()          ))),
    _mpsLastMsgTimestamp ( IScalVal_RO::create( _mpsRoot->findByName( std::string(MpsBaseModuleName + "/lastMsgTimeStamp").c_str()     ))),
    _mpsLastMsgByte      ( IScalVal_RO::create( _mpsRoot->findByName( std::string(MpsBaseModuleName + "/lastMsgByte").c_str()          ))),
    _lastMsgByteSize     ( _mpsLastMsgByte->getNelms()                                                                                  ),

    // MPS SALT interfaces
    _mpsTxLinkUpCnt      ( IScalVal_RO::create( _mpsRoot->findByName( std::string(MpsSaltModuleName + "/MpsTxLinkUpCnt").c_str()      ))),
    _mpsRxLinkUpCnt      ( IScalVal_RO::create( _mpsRoot->findByName( std::string(MpsSaltModuleName + "/MpsRxLinkUpCnt").c_str()      ))),
    _mpsTxLinkUP         ( IScalVal_RO::create( _mpsRoot->findByName( std::string(MpsSaltModuleName + "/MpsTxLinkUP").c_str()         ))),
    _mpsRxLinkUP         ( IScalVal_RO::create( _mpsRoot->findByName( std::string(MpsSaltModuleName + "/MpsRxLinkUP").c_str()         ))),
    _mpsSlotG            ( IScalVal_RO::create( _mpsRoot->findByName( std::string(MpsSaltModuleName + "/MPS_SLOT_G").c_str()          ))),
    _appTypeG            ( IScalVal_RO::create( _mpsRoot->findByName( std::string(MpsSaltModuleName + "/APP_TYPE_G").c_str()          ))),
    _mpsPllLocked        ( IScalVal_RO::create( _mpsRoot->findByName( std::string(MpsSaltModuleName + "/MpsPllLocked").c_str()        ))),
    _rollOverEn          ( IScalVal::create(    _mpsRoot->findByName( std::string(MpsSaltModuleName + "/RollOverEn").c_str()          ))),
    _mpsTxPktSentCnt     ( IScalVal_RO::create( _mpsRoot->findByName( std::string(MpsSaltModuleName + "/MpsTxPktSentCnt").c_str()     ))),
    _mpsRxPktRcvdSentCnt ( IScalVal_RO::create( _mpsRoot->findByName( std::string(MpsSaltModuleName + "/MpsRxPktRcvdCnt").c_str()     ))),
    _rstCnt              ( ICommand::create(    _mpsRoot->findByName( std::string(MpsSaltModuleName + "/RstCnt").c_str()              ))),
    _rstPll              ( ICommand::create(    _mpsRoot->findByName( std::string(MpsSaltModuleName + "/RstPll").c_str()              )))

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