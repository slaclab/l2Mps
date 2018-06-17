#include "l2Mps_mps.h"

IMpsNode::IMpsNode(Path mpsRoot)
:
    mpsRoot ( mpsRoot->clone() ),
    run     ( false )
{
    // MPS Base interfaces
    scalvals.appId               = IMpsBase::createInterface<ScalVal>(    mpsRoot, MpsBaseModuleName + "/mpsAppId");
    scalvals.version             = IMpsBase::createInterface<ScalVal>(    mpsRoot, MpsBaseModuleName + "/mpsVersion");
    scalvals.enable              = IMpsBase::createInterface<ScalVal>(    mpsRoot, MpsBaseModuleName + "/mpsEnable");
    scalvals.lcls1Mode           = IMpsBase::createInterface<ScalVal>(    mpsRoot, MpsBaseModuleName + "/lcls1Mode");
    scalvals.byteCount           = IMpsBase::createInterface<ScalVal_RO>( mpsRoot, MpsBaseModuleName + "/byteCount");
    scalvals.digitalEn           = IMpsBase::createInterface<ScalVal_RO>( mpsRoot, MpsBaseModuleName + "/digitalEn");
    scalvals.beamDestMask        = IMpsBase::createInterface<ScalVal>(    mpsRoot, MpsBaseModuleName + "/beamDestMask");
    scalvals.altDestMask         = IMpsBase::createInterface<ScalVal>(    mpsRoot, MpsBaseModuleName + "/altDestMask");
    scalvals.msgCnt              = IMpsBase::createInterface<ScalVal_RO>( mpsRoot, MpsBaseModuleName + "/mpsMsgCount");
    scalvals.lastMsgAppId        = IMpsBase::createInterface<ScalVal_RO>( mpsRoot, MpsBaseModuleName + "/lastMsgAppId");
    scalvals.lastMsgLcls         = IMpsBase::createInterface<ScalVal_RO>( mpsRoot, MpsBaseModuleName + "/lastMsgLcls");
    scalvals.lastMsgTimestamp    = IMpsBase::createInterface<ScalVal_RO>( mpsRoot, MpsBaseModuleName + "/lastMsgTimeStamp");
    scalvals.lastMsgByte         = IMpsBase::createInterface<ScalVal_RO>( mpsRoot, MpsBaseModuleName + "/lastMsgByte");

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
    scalvals.txLinkUp        = IMpsBase::createInterface<ScalVal_RO>( mpsRoot, MpsSaltModuleName + "/MpsTxLinkUP");
    scalvals.txLinkUpCnt     = IMpsBase::createInterface<ScalVal_RO>( mpsRoot, MpsSaltModuleName + "/MpsTxLinkUpCnt");
    scalvals.rxLinkUp        = IMpsBase::createInterface<ScalVal_RO>( mpsRoot, MpsSaltModuleName + "/MpsRxLinkUP");
    scalvals.rxLinkUpCnt     = IMpsBase::createInterface<ScalVal_RO>( mpsRoot, MpsSaltModuleName + "/MpsRxLinkUpCnt");
    scalvals.mpsSlot         = IMpsBase::createInterface<ScalVal_RO>( mpsRoot, MpsSaltModuleName + "/MPS_SLOT_G");
    scalvals.appType         = IMpsBase::createInterface<ScalVal_RO>( mpsRoot, MpsSaltModuleName + "/APP_TYPE_G");
    scalvals.pllLocked       = IMpsBase::createInterface<ScalVal_RO>( mpsRoot, MpsSaltModuleName + "/MpsPllLocked");
    scalvals.rollOverEn      = IMpsBase::createInterface<ScalVal>(    mpsRoot, MpsSaltModuleName + "/RollOverEn");
    scalvals.txPktSentCnt    = IMpsBase::createInterface<ScalVal_RO>( mpsRoot, MpsSaltModuleName + "/MpsTxPktSentCnt");
    scalvals.rxPktRcvdCnt    = IMpsBase::createInterface<ScalVal_RO>( mpsRoot, MpsSaltModuleName + "/MpsRxPktRcvdCnt");
    scalvals.rstCnt          = IMpsBase::createInterface<Command>(    mpsRoot, MpsSaltModuleName + "/RstCnt");
    scalvals.rstPll          = IMpsBase::createInterface<Command>(    mpsRoot, MpsSaltModuleName + "/RstPll");

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
    // Stop the thread if it was running
    if(run)
    {   
        run = false;
        scanThread.join();
    }
}

const void IMpsNode::readMpsInfo(mps_infoData_t& info) const
{
    info.appId              = IMpsBase::get( scalvals.appId             );
    info.version            = IMpsBase::get( scalvals.version           );
    info.enable             = IMpsBase::get( scalvals.enable            );
    info.lcls1Mode          = IMpsBase::get( scalvals.lcls1Mode         );
    info.byteCount          = IMpsBase::get( scalvals.byteCount         );
    info.digitalEn          = IMpsBase::get( scalvals.digitalEn         );
    info.beamDestMask       = IMpsBase::get( scalvals.beamDestMask      );
    info.altDestMask        = IMpsBase::get( scalvals.altDestMask       );
    info.msgCnt             = IMpsBase::get( scalvals.msgCnt            );
    info.lastMsgAppId       = IMpsBase::get( scalvals.lastMsgAppId      );
    info.lastMsgLcls        = IMpsBase::get( scalvals.lastMsgLcls       );
    info.lastMsgTimestamp   = IMpsBase::get( scalvals.lastMsgTimestamp  );
    info.txLinkUp           = IMpsBase::get( scalvals.txLinkUp          );
    info.txLinkUpCnt        = IMpsBase::get( scalvals.txLinkUpCnt       );
    info.mpsSlot            = IMpsBase::get( scalvals.mpsSlot           );
    info.pllLocked          = IMpsBase::get( scalvals.pllLocked         );
    info.rollOverEn         = IMpsBase::get( scalvals.rollOverEn        );
    info.txPktSentCnt       = IMpsBase::get( scalvals.txPktSentCnt      );

    uint8_t reg;
    scalvals.appType->getVal(&reg);
    std::map<int, std::string>::iterator it = appType.find(reg);
    if (it != appType.end())
        info.appType = it->second;

    IMpsBase::get(scalvals.lastMsgByte, info.lastMsgByte);
    IMpsBase::get(scalvals.rxPktRcvdCnt, info.rxPktRcvdCnt);
    IMpsBase::get(scalvals.rxLinkUpCnt, info.rxLinkUpCnt);

    // Convert the rxLnkUp status from register bit to a std::vector<bool>
    std::size_t n = 0;
    try
    {
        n = scalvals.rxLinkUp->getSizeBits();
    }
    catch (CPSWError& e)
    {
        throw std::runtime_error("CPSW error found while trying to get the bit size of the register\n");
    }

    if ((n > 0))
    {
        info.rxLinkUp.resize(n);
        uint32_t u32 = IMpsBase::get(scalvals.rxLinkUp);
        for (std::size_t i{0}; i < n; ++i)
            info.rxLinkUp.at(i) = (u32 & (1 << i));
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

    std::cout << "    Starting MPS node scan thread..." << std::endl;
    run = true;
    scanThread = std::thread( &IMpsNode::pollThread, this ); 
    if ( pthread_setname_np( scanThread.native_handle(), "mpsNodeScan" ) )
      perror( "pthread_setname_np failed for for MpsNode scanThread" );
}

void IMpsNode::pollThread()
{
    std::cout << "    MPS node scan thread created succesfully." << std::endl;

    for(;;)
    {
        if (!run)
        {
            std::cout << "    MPS node scan thread interrupted" << std::endl;
            return;
        }
        mps_infoData_t info;
        readMpsInfo(info);
        mpsCB(info);
        std::this_thread::sleep_for( std::chrono::seconds( pollCB ) );
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
