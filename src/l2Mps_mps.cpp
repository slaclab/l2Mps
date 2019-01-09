/**
 *-----------------------------------------------------------------------------
 * Title      : Common Platform MPS module class.
 * ----------------------------------------------------------------------------
 * File       : l2Mps_mps.cpp
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

#include "l2Mps_mps.h"

MpsNode IMpsNode::create(Path mpsRoot)
{
    if(!mpsRoot)
        throw std::runtime_error("The root Path is empty");

    return boost::make_shared<IMpsNode>(mpsRoot);
}

IMpsNode::IMpsNode(Path mpsRoot)
:
    run              ( false ),
    // MPS Base interfaces
    appId            ( mpsRoot, MpsBaseModuleName + "/mpsAppId"         ),
    version          ( mpsRoot, MpsBaseModuleName + "/mpsVersion"       ),
    enable           ( mpsRoot, MpsBaseModuleName + "/mpsEnable"        ),
    lcls1Mode        ( mpsRoot, MpsBaseModuleName + "/lcls1Mode"        ),
    byteCount        ( mpsRoot, MpsBaseModuleName + "/byteCount"        ),
    digitalEn        ( mpsRoot, MpsBaseModuleName + "/digitalEn"        ),
    beamDestMask     ( mpsRoot, MpsBaseModuleName + "/beamDestMask"     ),
    altDestMask      ( mpsRoot, MpsBaseModuleName + "/altDestMask"      ),
    msgCnt           ( mpsRoot, MpsBaseModuleName + "/mpsMsgCount"      ),
    lastMsgAppId     ( mpsRoot, MpsBaseModuleName + "/lastMsgAppId"     ),
    lastMsgLcls      ( mpsRoot, MpsBaseModuleName + "/lastMsgLcls"      ),
    lastMsgTimestamp ( mpsRoot, MpsBaseModuleName + "/lastMsgTimeStamp" ),
    lastMsgByte      ( mpsRoot, MpsBaseModuleName + "/lastMsgByte"      ),
    // MPS SALT interfaces
    txLinkUp         ( mpsRoot, MpsSaltModuleName + "/MpsTxLinkUP"      ),
    txLinkUpCnt      ( mpsRoot, MpsSaltModuleName + "/MpsTxLinkUpCnt"   ),
    rxLinkUp         ( mpsRoot, MpsSaltModuleName + "/MpsRxLinkUP"      ),
    rxLinkUpCnt      ( mpsRoot, MpsSaltModuleName + "/MpsRxLinkUpCnt"   ),
    mpsSlot          ( mpsRoot, MpsSaltModuleName + "/MPS_SLOT_G"       ),
    appType          ( mpsRoot, MpsSaltModuleName + "/APP_TYPE_G"       ),
    pllLocked        ( mpsRoot, MpsSaltModuleName + "/MpsPllLocked"     ),
    rollOverEn       ( mpsRoot, MpsSaltModuleName + "/RollOverEn"       ),
    txPktSentCnt     ( mpsRoot, MpsSaltModuleName + "/MpsTxPktSentCnt"  ),
    rxPktRcvdCnt     ( mpsRoot, MpsSaltModuleName + "/MpsRxPktRcvdCnt"  ),
    rstCnt           ( mpsRoot, MpsSaltModuleName + "/RstCnt"           ),
    rstPll           ( mpsRoot, MpsSaltModuleName + "/RstPll"           )
{
    // Create the application specific objects
    std::pair<bool,std::string> appTypeVal = getConvertedAppType();

    if (!appTypeVal.first)
        throw std::runtime_error("Invalid application type");

    for(std::size_t i {0}; i < numberOfBays; ++i)
    {
        if (!appTypeVal.second.compare("BPM"))
            amc[i] = IMpsBpm::create(mpsRoot, i);
        else if (!appTypeVal.second.compare("BLEN"))
            amc[i] = IMpsBlen::create(mpsRoot, i);
        else if (!appTypeVal.second.compare("BCM"))
            amc[i] = IMpsBcm::create(mpsRoot, i);
        else if ((!appTypeVal.second.compare("BLM")) | (!appTypeVal.second.compare("MPS_6CH")) | (!appTypeVal.second.compare("MPS_24CH")))
            amc[i] = IMpsBlm::create(mpsRoot, i);
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
    // Single value registers
    info.appId            = appId.get();
    info.version          = version.get();
    info.enable           = enable.get();
    info.lcls1Mode        = lcls1Mode.get();
    info.byteCount        = byteCount.get();
    info.digitalEn        = digitalEn.get();
    info.beamDestMask     = beamDestMask.get();
    info.altDestMask      = altDestMask.get();
    info.msgCnt           = msgCnt.get();
    info.lastMsgAppId     = lastMsgAppId.get();
    info.lastMsgLcls      = lastMsgLcls.get();
    info.lastMsgTimestamp = lastMsgTimestamp.get();
    info.txLinkUp         = txLinkUp.get();
    info.txLinkUpCnt      = txLinkUpCnt.get();
    info.mpsSlot          = mpsSlot.get();
    info.pllLocked        = pllLocked.get();
    info.rollOverEn       = rollOverEn.get();
    info.txPktSentCnt     = txPktSentCnt.get();

    // AppType which is converted to string
    info.appType          = getConvertedAppType();

    // Array registers
    info.lastMsgByte      = lastMsgByte.getArray();
    info.rxPktRcvdCnt     = rxPktRcvdCnt.getArray();
    info.rxLinkUpCnt      = rxLinkUpCnt.getArray();

    // Convert the rxLnkUp status from register bit to a std::vector<bool>
    bool valid = false;
    std::vector<bool> vals;

    std::pair<bool, uint8_t> rxLinkUpRaw =  rxLinkUp.get();

    if (rxLinkUpRaw.first)
    {
        size_t n = rxLinkUp.getSizeBits();
        if (n)
        {
            vals.resize(n);
            for (size_t i(0); i < n; ++i)
                vals.at(i) = (rxLinkUpRaw.second & (1 << i)) ? true : false;
            valid = true;
        }
    }
    info.rxLinkUp = std::make_pair( valid, vals );
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
    std::cout << "    MPS node scan thread created successfully." << std::endl;

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

std::pair<bool,std::string> IMpsNode::getConvertedAppType() const
{
    bool valid = false;
    std::string val;

    std::pair<bool, uint8_t> appTypeRaw = appType.get();
    if (appTypeRaw.first)
    {
        std::map<int, std::string>::iterator it = appTypeList.find( appTypeRaw.second );
        if (it != appTypeList.end())
        {
            val = it->second;
            valid = true;
        }
    }
    return std::make_pair( valid, val );
}

std::pair<bool,bool> IMpsNode::getRxLinkUp(const uint8_t ch) const
{
    bool valid = false;
    bool val   = false;

    std::pair<bool, uint8_t> rxLinkUpRaw = rxLinkUp.get();

    if (rxLinkUpRaw.first)
    {
        uint8_t mask = (1<<ch);
        val = ( rxLinkUpRaw.second & mask ) ? true : false;
        valid = true;
    }

    return std::make_pair( valid, val );
}
