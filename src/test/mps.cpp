#include <stdio.h>
#include <iostream>
#include <string.h>
#include <iomanip>
#include <vector>
#include <array>
#include <inttypes.h>
#include <boost/shared_ptr.hpp>
#include <cpsw_api_user.h>
#include <yaml-cpp/yaml.h>
#include <getopt.h>
#include <stdexcept>
#include <arpa/inet.h>

#include "l2Mps_mps.h"

class IYamlSetIP : public IYamlFixup
{
    public:
        IYamlSetIP( std::string ip_addr ) : ip_addr_(ip_addr) {}

        void operator()(YAML::Node &node)
        {
            node["ipAddr"] = ip_addr_.c_str();
        }

        ~IYamlSetIP() {}

    private:
        std::string ip_addr_;
};

void mpsInfoReceiver(mps_infoData_t info)
{
    std::cout << "MPS message received!" << std::endl;
    std::cout << "=============================" << std::endl;
    std::cout << "appId             = "  <<                      info.appId                      << std::endl;
    std::cout << "version           = "  <<             unsigned(info.version)                   << std::endl;
    std::cout << "enable            = "  << std::boolalpha <<    info.enable                     << std::endl;
    std::cout << "lcls1Mode         = "  << std::boolalpha <<    info.lcls1Mode                  << std::endl;
    std::cout << "byteCount         = "  <<             unsigned(info.byteCount)                 << std::endl;
    std::cout << "digitalEn         = "  << std::boolalpha <<    info.digitalEn                  << std::endl;
    std::cout << "beamDestMask      = "  << "0x" << std::hex <<  info.beamDestMask << std::dec   << std::endl;
    std::cout << "altDestMask       = "  << "0x" << std::hex <<  info.altDestMask << std::dec    << std::endl;
    std::cout << "msgCnt            = "  <<                      info.msgCnt                     << std::endl;
    std::cout << "lastMsgAppId      = "  <<                      info.lastMsgAppId               << std::endl;
    std::cout << "lastMsgLcls       = "  << std::boolalpha <<    info.lastMsgLcls                << std::endl;
    std::cout << "lastMsgTimestamp  = "  <<                      info.lastMsgTimestamp           << std::endl;

    std::cout << "lastMsgByte =" << std::endl;
    std::cout << std::setw(10) << "Index:";
    for (std::size_t i{0}; i < info.lastMsgByte.size(); ++i)
        std::cout << std::setw(15) << i;
    std::cout << std::endl << std::setw(10) << "Status:";
    for (std::size_t i{0}; i < info.lastMsgByte.size(); ++i)
        std::cout << std::setw(13) << "0x" << std::hex << unsigned(info.lastMsgByte.at(i)) << std::dec;
    std::cout << std::endl;

    std::cout << "txLinkUp          = "  << std::boolalpha <<    info.txLinkUp                   << std::endl;
    std::cout << "txLinkUpCnt       = "  <<                      info.txLinkUpCnt                << std::endl;

    std::cout << "rxLinkUp =" << std::endl;
    std::cout << std::setw(10) << "Index:";
    for (std::size_t i{0}; i < info.rxLinkUp.size(); ++i)
        std::cout << std::setw(15) << i;
    std::cout << std::endl << std::setw(10) << "Status:";
    for (std::size_t i{0}; i < info.rxLinkUp.size(); ++i)
        std::cout << std::setw(15) << std::boolalpha << info.rxLinkUp.at(i);
    std::cout << std::endl << std::setw(10) << "Counters:";
    for (std::size_t i{0}; i < info.rxLinkUpCnt.size(); ++i)
        std::cout << std::setw(15) << info.rxLinkUpCnt.at(i);
    std::cout << std::endl;

    std::cout << "mpsSlot           = "  << std::boolalpha <<    info.mpsSlot                    << std::endl;
    std::cout << "appType           = "  <<                      info.appType                    << std::endl;
    std::cout << "pllLocked         = "  << std::boolalpha <<    info.pllLocked                  << std::endl;
    std::cout << "rollOverEn        = "  <<                      info.rollOverEn                 << std::endl;
    std::cout << "txPktSentCnt      = "  <<                      info.txPktSentCnt               << std::endl;

    std::cout << "rxPktRcvd =" << std::endl;
    std::cout << std::setw(10) << "Index:";
    for (std::size_t i{0}; i < info.rxPktRcvdCnt.size(); ++i)
        std::cout << std::setw(15) << i;
    std::cout << std::endl << std::setw(10) << "Counters:";
    for (std::size_t i{0}; i < info.rxPktRcvdCnt.size(); ++i)
        std::cout << std::setw(15) << info.rxPktRcvdCnt[i];
    std::cout << std::endl;

    std::cout << "=============================" << std::endl;
    std::cout << std::endl;
}

int main(int argc, char **argv)
{
    unsigned char buf[sizeof(struct in6_addr)];
    std::string ipAddr;
    std::string yamlDoc;
    int c;

    while((c =  getopt(argc, argv, "a:Y:")) != -1)
    {
        switch (c)
        {
            case 'a':
                if (!inet_pton(AF_INET, optarg, buf))
                {
                    std::cout << "Invalid IP address..." << std::endl;
                    exit(1);
                }
                ipAddr = std::string(optarg);
                break;
            case 'Y':
                yamlDoc = std::string(optarg);
                break;
            default:
                std::cout << "Invalid option. Use -a <IP_address> -Y <Yaml_top>" << std::endl;
                exit(1);
                break;
        }
    }

    if (ipAddr.empty())
    {
        std::cout << "Must specify an IP address." << std::endl;
        exit(1);
    }

    if (yamlDoc.empty())
    {
        std::cout << "Must specify a Yaml top path." << std::endl;
        exit(1);
    }

    const char *mpsRootName = "mmio/AmcCarrierCore/AppMps";

    IYamlSetIP setIP(ipAddr);
    Path root = IPath::loadYamlFile( yamlDoc.c_str(), "NetIODev", NULL, &setIP );

    try
    {
        Path mpsRoot = root->findByName(mpsRootName);

        std::cout << "MPS node" << std::endl;
        std::cout << "====================================================" << std::endl;
        MpsNode mpsNode = MpsNodeFactory::create(mpsRoot);
        std::cout << "====================================================" << std::endl;

        std::cout << std::endl;
        std::cout << "====================================================" << std::endl;
        std::cout << "Read all registers using the get functions" << std::endl;
        std::cout << "====================================================" << std::endl;
        std::cout << "appId             = "  <<                      mpsNode->getAppId()                      << std::endl;
        std::cout << "version           = "  <<             unsigned(mpsNode->getVersion())                   << std::endl;
        std::cout << "enable            = "  << std::boolalpha <<     mpsNode->getEnable()                     << std::endl;
        std::cout << "lcls1Mode         = "  << std::boolalpha <<     mpsNode->getLcls1Mode()                  << std::endl;
        std::cout << "byteCount         = "  <<             unsigned( mpsNode->getByteCount())                 << std::endl;
        std::cout << "digitalEn         = "  << std::boolalpha <<     mpsNode->getDigitalEnable()                  << std::endl;
        std::cout << "beamDestMask      = "  << "0x" << std::hex <<   mpsNode->getBeamDestMask() << std::dec   << std::endl;
        std::cout << "altDestMask       = "  << "0x" << std::hex <<   mpsNode->getAltDestMask() << std::dec    << std::endl;
        std::cout << "msgCnt            = "  <<                       mpsNode->getMsgCount()                     << std::endl;
        std::cout << "lastMsgAppId      = "  <<                       mpsNode->getLastMsgAppId()               << std::endl;
        std::cout << "lastMsgLcls       = "  << std::boolalpha <<     mpsNode->getLastMsgLcls()                << std::endl;
        std::cout << "lastMsgTimestamp  = "  <<                       mpsNode->getLastMsgTimeStamp()           << std::endl;

        {
            std::size_t n = mpsNode->getLastMsgByteSize();
            std::cout << "lastMsgByte =" << std::endl;
            std::cout << std::setw(10) << "Index:";
            for (std::size_t i{0}; i < n; ++i)
                std::cout << std::setw(15) << i;
            std::cout << std::endl << std::setw(10) << "Counters:";
            for (std::size_t i{0}; i < n; ++i)
                std::cout << std::setw(13) << "0x" << std::hex <<  unsigned(mpsNode->getLastMsgByte(i)) << std::dec;
            std::cout << std::endl;
        }

        std::cout << "txLinkUp          = "  << std::boolalpha <<    mpsNode->getTxLinkUp()                   << std::endl;
        std::cout << "txLinkUpCnt       = "  <<                      mpsNode->getTxLinkUpCnt()                << std::endl;

        {
            std::size_t n = mpsNode->getRxLinkUpCntSize();
            std::cout << "rxLinkUp =" << std::endl;
            std::cout << std::setw(10) << "Index:";
            for (std::size_t i{0}; i < n; ++i)
                std::cout << std::setw(15) << i;
            std::cout << std::endl << std::setw(10) << "Status:";
            for (std::size_t i{0}; i < n; ++i)
                std::cout << std::setw(15) << std::boolalpha << mpsNode->getRxLinkUp(i);
            std::cout << std::endl << std::setw(10) << "Counters:";
            for (std::size_t i{0}; i < n; ++i)
                std::cout << std::setw(15) << mpsNode->getRxLinkUpCnt(i);
            std::cout << std::endl;
        }

        std::cout << "mpsSlot           = "  << std::boolalpha <<    mpsNode->getMpsSlot()                    << std::endl;
        std::cout << "appType           = "  <<                      mpsNode->getAppType()                    << std::endl;
        std::cout << "pllLocked         = "  << std::boolalpha <<    mpsNode->getPllLocked()                  << std::endl;
        std::cout << "rollOverEn        = "  <<                      mpsNode->getRollOverEn()                 << std::endl;
        std::cout << "txPktSentCnt      = "  <<                      mpsNode->getTxPktSentCnt()               << std::endl;

        {
            std::size_t n = mpsNode->getRxPktRcvdCntSize();
             std::cout << "rxPktRcvdCnt =" << std::endl;
            std::cout << std::setw(10) << "Index:";
            for (std::size_t i{0}; i < n; ++i)
                std::cout << std::setw(15) << i;
            std::cout << std::endl << std::setw(10) << "Counters:";
            for (std::size_t i{0}; i < n; ++i)
                std::cout << std::setw(15) << mpsNode->getRxPktRcvdCnt(i);
            std::cout << std::endl;
        }


        std::cout << "====================================================" << std::endl;
        std::cout << "Done!"<< std::endl;
        std::cout << std::endl;

        std::cout << "Starting MPS polling thread at 1Hz" << std::endl;
        mpsNode->startPollThread(1, &mpsInfoReceiver);


        while(1)
            sleep(10);

        std::cout << "====================================================" << std::endl;

    }
    catch (CPSWError &e)
    {
        printf("CPSW error: %s not found!\n", e.getInfo().c_str());
        return 1;
    }

    return 0;
}
