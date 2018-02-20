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


// Get application type string
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

// Get the Rx LinkUp status bit
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