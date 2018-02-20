#include "l2Mps_mps.h"

IMpsNode::IMpsNode(Path mpsRoot) :
    // MPS root path
    _mpsRoot             ( mpsRoot->clone())

{
    // MPS Base interfaces
    scalvals.appId               = IMpsBase::createInterface<ScalVal>(    _mpsRoot, MpsBaseModuleName + "/mpsAppId");
    scalvals.version             = IMpsBase::createInterface<ScalVal>(    _mpsRoot, MpsBaseModuleName + "/mpsVersion");
    scalvals.enable              = IMpsBase::createInterface<ScalVal>(    _mpsRoot, MpsBaseModuleName + "/mpsEnable");
    scalvals.lcls1Mode           = IMpsBase::createInterface<ScalVal>(    _mpsRoot, MpsBaseModuleName + "/lcls1Mode");
    scalvals.byteCount           = IMpsBase::createInterface<ScalVal_RO>( _mpsRoot, MpsBaseModuleName + "/byteCount");
    scalvals.digitalEn           = IMpsBase::createInterface<ScalVal_RO>( _mpsRoot, MpsBaseModuleName + "/digitalEn");
    scalvals.beamDestMask        = IMpsBase::createInterface<ScalVal>(    _mpsRoot, MpsBaseModuleName + "/beamDestMask");
    scalvals.altDestMask         = IMpsBase::createInterface<ScalVal>(    _mpsRoot, MpsBaseModuleName + "/altDestMask");
    scalvals.msgCnt              = IMpsBase::createInterface<ScalVal_RO>( _mpsRoot, MpsBaseModuleName + "/mpsMsgCount");
    scalvals.lastMsgAppId        = IMpsBase::createInterface<ScalVal_RO>( _mpsRoot, MpsBaseModuleName + "/lastMsgAppId");
    scalvals.lastMsgLcls         = IMpsBase::createInterface<ScalVal_RO>( _mpsRoot, MpsBaseModuleName + "/lastMsgLcls");
    scalvals.lastMsgTimestamp    = IMpsBase::createInterface<ScalVal_RO>( _mpsRoot, MpsBaseModuleName + "/lastMsgTimeStamp");
    scalvals.lastMsgByte         = IMpsBase::createInterface<ScalVal_RO>( _mpsRoot, MpsBaseModuleName + "/lastMsgByte");

    if (scalvals.lastMsgByte)
    {
        try
        {
            lastMsgByteSize = scalvals.lastMsgByte->getNelms();
        }
        catch (CPSWError &e)
        {
            std::cout << "Couldn't read the number of elements from the last message: " << e.getInfo() << std::endl;
        }
    }

    // MPS SALT interfaces
    scalvals.txLinkUp        = IMpsBase::createInterface<ScalVal_RO>( _mpsRoot, MpsSaltModuleName + "/MpsTxLinkUP");
    scalvals.txLinkUpCnt     = IMpsBase::createInterface<ScalVal_RO>( _mpsRoot, MpsSaltModuleName + "/MpsTxLinkUpCnt");
    scalvals.rxLinkUp        = IMpsBase::createInterface<ScalVal_RO>( _mpsRoot, MpsSaltModuleName + "/MpsRxLinkUP");
    scalvals.rxLinkUpCnt     = IMpsBase::createInterface<ScalVal_RO>( _mpsRoot, MpsSaltModuleName + "/MpsRxLinkUpCnt");
    scalvals.mpsSlot         = IMpsBase::createInterface<ScalVal_RO>( _mpsRoot, MpsSaltModuleName + "/MPS_SLOT_G");
    scalvals.appType         = IMpsBase::createInterface<ScalVal_RO>( _mpsRoot, MpsSaltModuleName + "/APP_TYPE_G");
    scalvals.pllLocked       = IMpsBase::createInterface<ScalVal_RO>( _mpsRoot, MpsSaltModuleName + "/MpsPllLocked");
    scalvals.rollOverEn      = IMpsBase::createInterface<ScalVal>(    _mpsRoot, MpsSaltModuleName + "/RollOverEn");
    scalvals.txPktSentCnt    = IMpsBase::createInterface<ScalVal_RO>( _mpsRoot, MpsSaltModuleName + "/MpsTxPktSentCnt");
    scalvals.rxPktRcvdCnt    = IMpsBase::createInterface<ScalVal_RO>( _mpsRoot, MpsSaltModuleName + "/MpsRxPktRcvdCnt");
    scalvals.rstCnt          = IMpsBase::createInterface<Command>(    _mpsRoot, MpsSaltModuleName + "/RstCnt");
    scalvals.rstPll          = IMpsBase::createInterface<Command>(    _mpsRoot, MpsSaltModuleName + "/RstPll");

    if (scalvals.rxLinkUpCnt)
    {
        try
        {
            rxLinkUpCntSize = scalvals.rxLinkUpCnt->getNelms();
        }
        catch (CPSWError &e)
        {
            std::cout << "Couldn't read the number of RX link counters: " << e.getInfo() << std::endl;
        }
    }

    if (scalvals.rxPktRcvdCnt)
    {
        try
        {
            rxPktRcvdCntSize = scalvals.rxPktRcvdCnt->getNelms();
        }
        catch (CPSWError &e)
        {
            std::cout << "Couldn't read the number of RX packet counters: " << e.getInfo() << std::endl;
        }
    }
}

IMpsNode::~IMpsNode()
{
    std::cout << "MPS node destroyed" << std::endl;
}

const void IMpsNode::readMpsInfo(mps_infoData_t& info) const
{
    scalvals.appId->getVal(            (uint16_t*)&info.appId            );
    scalvals.version->getVal(          (uint8_t*)&info.version           );
    scalvals.enable->getVal(           (uint8_t*)&info.enable            );
    scalvals.lcls1Mode->getVal(        (uint8_t*)&info.lcls1Mode         );
    scalvals.byteCount->getVal(        (uint8_t*)&info.byteCount         );
    scalvals.digitalEn->getVal(        (uint8_t*)&info.digitalEn         );
    scalvals.beamDestMask->getVal(     (uint16_t*)&info.beamDestMask     );
    scalvals.altDestMask->getVal(      (uint16_t*)&info.altDestMask      );
    scalvals.msgCnt->getVal(           (uint32_t*)&info.msgCnt           );
    scalvals.lastMsgAppId->getVal(     (uint16_t*)&info.lastMsgAppId     );
    scalvals.lastMsgLcls->getVal(      (uint8_t*)&info.lastMsgLcls       );
    scalvals.lastMsgTimestamp->getVal( (uint16_t*)&info.lastMsgTimestamp );
    scalvals.txLinkUp->getVal(         (uint8_t*)&info.txLinkUp          );
    scalvals.txLinkUpCnt->getVal(      (uint32_t*)&info.txLinkUpCnt      );
    scalvals.rxLinkUp->getVal(         (uint32_t*)&info.rxLinkUp         );
    scalvals.mpsSlot->getVal(          (uint8_t*)&info.mpsSlot           );
    scalvals.pllLocked->getVal(        (uint8_t*)&info.pllLocked         );
    scalvals.rollOverEn->getVal(       (uint16_t*)&info.rollOverEn       );
    scalvals.txPktSentCnt->getVal(     (uint32_t*)&info.txPktSentCnt     );

    uint8_t reg;
    scalvals.appType->getVal(&reg);
    std::map<int, std::string>::iterator it = appType.find(reg);
    if (it != appType.end())
        info.appType = it->second;

    if ((lastMsgByteSize > 0) && (scalvals.lastMsgByte))
    {
        info.lastMsgByte.resize(lastMsgByteSize);
        scalvals.lastMsgByte->getVal(&info.lastMsgByte[0], lastMsgByteSize);
    }

    if ((rxLinkUpCntSize > 0) && (scalvals.rxLinkUpCnt))
    {
        info.rxLinkUpCnt.resize(rxLinkUpCntSize);
        scalvals.rxLinkUpCnt->getVal(&info.rxLinkUpCnt[0], rxLinkUpCntSize);
    }

    if ((rxPktRcvdCntSize > 0) && (scalvals.rxPktRcvdCnt))
    {
        info.rxPktRcvdCnt.resize(rxPktRcvdCntSize);
        scalvals.rxPktRcvdCnt->getVal(&info.rxPktRcvdCnt[0], rxPktRcvdCntSize);
    }
}

const void IMpsNode::startPollThread(unsigned int poll, p_mpsCBFunc_t cbFunc)
{
    if (poll == 0)
    {
        std::cout << "Error creating poll thread: poll time must be greater than 0" << std::endl;
        return;
    }
    pollCB  = poll;
    mpsCB   = cbFunc;

    std::cout << "  Starting MPS scan thread..." << std::endl;
    pthread_create(&scanThread, NULL, createThread, this);
    std::cout << "  MPS scan thread created succesfully." << std::endl;
}

void IMpsNode::pollThread()
{
    while(1)
    {
        mps_infoData_t info;
        readMpsInfo(info);
        mpsCB(info);
        sleep(pollCB);
    }
}

uint16_t const IMpsNode::getAppId(void) const
{
    if (!scalvals.appId)
        throw std::runtime_error("Register interface not implemented\n");

    uint32_t reg;
    scalvals.appId->getVal(&reg);
    return reg;
}

void IMpsNode::setAppId(const uint16_t id) const
{
    if (!scalvals.appId)
        throw std::runtime_error("Register interface not implemented\n");

    scalvals.appId->setVal(id);
}

bool const IMpsNode::getEnable() const
{
    if (!scalvals.enable)
        throw std::runtime_error("Register interface not implemented\n");

    uint8_t reg;
    scalvals.enable->getVal(&reg);
    return reg ? true : false;
}

void IMpsNode::setEnable(const bool en) const
{
    if (!scalvals.enable)
        throw std::runtime_error("Register interface not implemented\n");

     scalvals.enable->setVal((uint8_t)en);
}

bool const IMpsNode::getLcls1Mode(void) const
{
    if (!scalvals.lcls1Mode)
        throw std::runtime_error("Register interface not implemented\n");

    uint8_t reg;
    scalvals.lcls1Mode->getVal(&reg);
    return reg ? true : false;
}

void IMpsNode::setLcls1Mode(const bool mode) const
{
    if (!scalvals.lcls1Mode)
        throw std::runtime_error("Register interface not implemented\n");

    scalvals.lcls1Mode->setVal((uint8_t)mode);
}

uint8_t const IMpsNode::getByteCount(void) const
{
    if (!scalvals.byteCount)
        throw std::runtime_error("Register interface not implemented\n");

    uint8_t reg;
    scalvals.byteCount->getVal(&reg);
    return reg;
}

bool const IMpsNode::getDigitalEnable(void) const
{
    if (!scalvals.digitalEn)
        throw std::runtime_error("Register interface not implemented\n");

    uint8_t reg;
    scalvals.digitalEn->getVal(&reg);
    return reg ? true : false;
}

uint16_t const IMpsNode::getBeamDestMask(void) const
{
    if (!scalvals.beamDestMask)
        throw std::runtime_error("Register interface not implemented\n");

    uint16_t reg;
    scalvals.beamDestMask->getVal(&reg);
    return reg;
}

void IMpsNode::setBeamDestMask(const uint16_t mask) const
{
    if (!scalvals.beamDestMask)
        throw std::runtime_error("Register interface not implemented\n");

    scalvals.beamDestMask->setVal(mask);
}

uint8_t const IMpsNode::getAltDestMask(void) const
{
    if (!scalvals.altDestMask)
        throw std::runtime_error("Register interface not implemented\n");

    uint8_t reg;
    scalvals.altDestMask->getVal(&reg);
    return reg;
}

void IMpsNode::setAltDestMask(const uint8_t mask) const
{
    if (!scalvals.altDestMask)
        throw std::runtime_error("Register interface not implemented\n");

    scalvals.altDestMask->setVal(mask);
}

std::string const IMpsNode::getAppType(void) const
{
    if (!scalvals.appType)
        throw std::runtime_error("Register interface not implemented\n");

    uint8_t reg;
    scalvals.appType->getVal(&reg);

    std::map<int, std::string>::iterator it = appType.find(reg);
    if (it != appType.end())
        return (*it).second;
    else
        return std::string();
}

uint32_t const IMpsNode::getTxLinkUpCnt(void) const
{
    if (!scalvals.txLinkUpCnt)
        throw std::runtime_error("Register interface not implemented\n");

    uint32_t reg;
    scalvals.txLinkUpCnt->getVal(&reg);
    return reg;
}

uint32_t const IMpsNode::getRxLinkUpCnt(const uint8_t ch) const
{
    if (!scalvals.rxLinkUpCnt)
        throw std::runtime_error("Register interface not implemented\n");

    uint8_t n = scalvals.rxLinkUpCnt->getNelms();

    if (ch > n)
        throw std::runtime_error("Register index aout of range\n");

    uint32_t reg[n];
    scalvals.rxLinkUpCnt->getVal(reg, n);
    return reg[ch];
}

uint32_t const IMpsNode::getRollOverEn(void) const
{
    if (!scalvals.rollOverEn)
        throw std::runtime_error("Register interface not implemented\n");

    uint32_t reg;
    scalvals.rollOverEn->getVal(&reg);
    return reg;
}

bool const IMpsNode::getTxLinkUp(void) const
{
    if (!scalvals.txLinkUp)
        throw std::runtime_error("Register interface not implemented\n");

    uint8_t reg;
    scalvals.txLinkUp->getVal(&reg);
    return reg ? true : false;
}

bool const IMpsNode::getRxLinkUp(const uint8_t ch) const
{
    if (!scalvals.rxLinkUp)
        throw std::runtime_error("Register interface not implemented\n");

    uint8_t n = scalvals.rxLinkUp->getSizeBits();
    uint16_t mask = (1<<ch);

    if (ch > n)
        throw std::runtime_error("Register index aout of range\n");

    uint8_t reg;
    scalvals.rxLinkUp->getVal(&reg);
    return (reg & mask) ? true : false;
}

bool const IMpsNode::getMpsSlot(void) const
{
    if (!scalvals.mpsSlot)
        throw std::runtime_error("Register interface not implemented\n");

    uint8_t reg;
    scalvals.mpsSlot->getVal(&reg);
    return reg ? true : false;
}

bool const IMpsNode::getPllLocked(void) const
{
    if (!scalvals.pllLocked)
        throw std::runtime_error("Register interface not implemented\n");

    uint8_t reg;
    scalvals.pllLocked->getVal(&reg);
    return reg ? true : false;
}

uint32_t const IMpsNode::getTxPktSentCnt(void) const
{
    if (!scalvals.txPktSentCnt)
        throw std::runtime_error("Register interface not implemented\n");

    uint32_t reg;
    scalvals.txPktSentCnt->getVal(&reg);
    return reg;
}

uint32_t const IMpsNode::getRxPktRcvdSentCnt(const uint8_t ch) const
{
      if (!scalvals.rxPktRcvdCnt)
        throw std::runtime_error("Register interface not implemented\n");

    uint8_t n = scalvals.rxPktRcvdCnt->getNelms();

    if (ch > n)
        throw std::runtime_error("Register index aout of range\n");

    uint32_t reg[n];
    scalvals.rxPktRcvdCnt->getVal(reg, n);

    return reg[ch];
}

uint32_t const IMpsNode::getMpsMsgCount(void) const
{
    if (!scalvals.msgCnt)
        throw std::runtime_error("Register interface not implemented\n");

    uint32_t reg;
    scalvals.msgCnt->getVal(&reg);
    return reg;
}

uint16_t const IMpsNode::getLastMsgAppId(void) const
{
    if (!scalvals.lastMsgAppId)
        throw std::runtime_error("Register interface not implemented\n");

    uint16_t reg;
    scalvals.lastMsgAppId->getVal(&reg);
    return reg;
}

bool const IMpsNode::getLastMsgLcls(void) const
{
    if (!scalvals.lastMsgLcls)
        throw std::runtime_error("Register interface not implemented\n");
    uint8_t reg;
    scalvals.lastMsgLcls->getVal(&reg);
    return reg ? true : false;
}

uint16_t const IMpsNode::getLastMsgTimeStamp(void) const
{
    if (!scalvals.lastMsgTimestamp)
        throw std::runtime_error("Register interface not implemented\n");
    uint16_t reg;
    scalvals.lastMsgTimestamp->getVal(&reg);
    return reg;
}

uint8_t const IMpsNode::getLastMsgByte(const uint8_t index) const
{
    if (!scalvals.lastMsgByte)
        throw std::runtime_error("Register interface not implemented\n");

    if (index > lastMsgByteSize)
        throw std::runtime_error("LastMsgByte: request index is out of range \n");

    uint8_t reg[lastMsgByteSize];
    scalvals.lastMsgByte->getVal(reg, lastMsgByteSize);
    return reg[index];
}

void const IMpsNode::resetSaltCnt(void) const
{
    if (!scalvals.rstCnt)
        throw std::runtime_error("Command interface not implemented\n");

    scalvals.rstCnt->execute();
}

void const IMpsNode::resetSaltPll(void) const
{
    if (!scalvals.rstPll)
        throw std::runtime_error("Command interface not implemented\n");

    scalvals.rstPll->execute();
}