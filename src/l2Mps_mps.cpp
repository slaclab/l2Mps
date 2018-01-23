#include "l2Mps_mps.h"

IMpsNode::IMpsNode(Path mpsRoot) :
    // MPS root path
    _mpsRoot             ( mpsRoot->clone())

{
    // MPS Base interfaces
    _mpsAppId            = createInterface<ScalVal>(    MpsBaseModuleName + "/mpsAppId");
    _mpsEnable           = createInterface<ScalVal>(    MpsBaseModuleName + "/mpsEnable");
    _lcls1Mode           = createInterface<ScalVal>(    MpsBaseModuleName + "/lcls1Mode");
    _byteCount           = createInterface<ScalVal_RO>( MpsBaseModuleName + "/byteCount");
    _digitalEn           = createInterface<ScalVal_RO>( MpsBaseModuleName + "/digitalEn");
    _beamDestMask        = createInterface<ScalVal>(    MpsBaseModuleName + "/beamDestMask");
    _altDestMask         = createInterface<ScalVal>(    MpsBaseModuleName + "/altDestMask");
    _mpsMsgCnt           = createInterface<ScalVal_RO>( MpsBaseModuleName + "/mpsMsgCount");
    _mpsLastMsgAppId     = createInterface<ScalVal_RO>( MpsBaseModuleName + "/lastMsgAppId");
    _mpsLastMsgLcls      = createInterface<ScalVal_RO>( MpsBaseModuleName + "/lastMsgLcls");
    _mpsLastMsgTimestamp = createInterface<ScalVal_RO>( MpsBaseModuleName + "/lastMsgTimeStamp");
    _mpsLastMsgByte      = createInterface<ScalVal_RO>( MpsBaseModuleName + "/lastMsgByte");

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
    _mpsTxLinkUpCnt      = createInterface<ScalVal_RO>( MpsSaltModuleName + "/MpsTxLinkUpCnt");
    _mpsRxLinkUpCnt      = createInterface<ScalVal_RO>( MpsSaltModuleName + "/MpsRxLinkUpCnt");
    _mpsTxLinkUP         = createInterface<ScalVal_RO>( MpsSaltModuleName + "/MpsTxLinkUP");
    _mpsRxLinkUP         = createInterface<ScalVal_RO>( MpsSaltModuleName + "/MpsRxLinkUP");
    _mpsSlotG            = createInterface<ScalVal_RO>( MpsSaltModuleName + "/MPS_SLOT_G");
    _appTypeG            = createInterface<ScalVal_RO>( MpsSaltModuleName + "/APP_TYPE_G");
    _mpsPllLocked        = createInterface<ScalVal_RO>( MpsSaltModuleName + "/MpsPllLocked");
    _rollOverEn          = createInterface<ScalVal>(    MpsSaltModuleName + "/RollOverEn");
    _mpsTxPktSentCnt     = createInterface<ScalVal_RO>( MpsSaltModuleName + "/MpsTxPktSentCnt");
    _mpsRxPktRcvdSentCnt = createInterface<ScalVal_RO>( MpsSaltModuleName + "/MpsRxPktRcvdCnt");
    _rstCnt              = createInterface<Command>(    MpsSaltModuleName + "/RstCnt");
    _rstPll              = createInterface<Command>(    MpsSaltModuleName + "/RstPll");
    
}


IMpsNode::~IMpsNode()
{
    std::cout << "MPS node destroyed" << std::endl;
}

// CPSW Interfaces constructor wrapper
template <typename T>
T IMpsNode::createInterface(const std::string& regName)
{
    T reg;

    try
    {
        reg = CPSWCreate<T>(regName);
    }
    catch (CPSWError &e)
    {
        std::cout << "Couldn't create interface to \"" << regName << "\": "<< e.getInfo() << std::endl;
    }

    return reg;
}

// CPSW interface constructors
template <>
ScalVal IMpsNode::CPSWCreate(const std::string& regName)
{
    return IScalVal::create(_mpsRoot->findByName(regName.c_str()));
}

template <>
ScalVal_RO IMpsNode::CPSWCreate(const std::string& regName)
{
    return IScalVal_RO::create(_mpsRoot->findByName(regName.c_str()));
}

template <>
Command IMpsNode::CPSWCreate(const std::string& regName)
{
    return ICommand::create(_mpsRoot->findByName(regName.c_str()));
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