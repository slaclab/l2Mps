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
    std::cout << "lastMsgByteSize   = "  <<                      info.lastMsgByteSize            << std::endl;
        
    std::cout << std::setw(15) << "lastMsgByte:";
    for (std::size_t i{0}; i < info.lastMsgByteSize; ++i)
        std::cout << std::setw(10) << i;
    std::cout << std::endl << std::setw(15) << "";
    for (std::size_t i{0}; i < info.lastMsgByteSize; ++i)
        std::cout << std::setw(10) << unsigned(info.lastMsgByte[i]);
    std::cout << std::endl;

    std::cout << "txLinkUp          = "  << std::boolalpha <<    info.txLinkUp                   << std::endl;
    std::cout << "txLinkUpCnt       = "  <<                      info.txLinkUpCnt                << std::endl;
    std::cout << "rxLinkUp          = "  << "0x" << std::hex <<  info.rxLinkUp << std::dec       << std::endl;
    std::cout << "rxLinkUpCntSize   = "  <<                      info.rxLinkUpCntSize            << std::endl;
    
    std::cout << std::setw(15) << "rxLinkUpCnt:";
    for (std::size_t i{0}; i < info.rxLinkUpCntSize; ++i)
        std::cout << std::setw(10) << i;
    std::cout << std::endl << std::setw(15) << "";
    for (std::size_t i{0}; i < info.rxLinkUpCntSize; ++i)
        std::cout << std::setw(10) << info.rxLinkUpCnt[i];
    std::cout << std::endl;

    std::cout << "mpsSlot           = "  << std::boolalpha <<    info.mpsSlot                    << std::endl;
    std::cout << "appType           = "  <<                      info.appType                    << std::endl;
    std::cout << "pllLocked         = "  << std::boolalpha <<    info.pllLocked                  << std::endl;
    std::cout << "rollOverEn        = "  <<                      info.rollOverEn                 << std::endl;
    std::cout << "txPktSentCnt      = "  <<                      info.txPktSentCnt               << std::endl;
    std::cout << "rxPktRcvdCntSize  = "  <<                      info.rxPktRcvdCntSize           << std::endl;
    
    std::cout << std::setw(15) << "rxPktRcvdCnt:";
    for (std::size_t i{0}; i < info.rxPktRcvdCntSize; ++i)
        std::cout << std::setw(10) << i;
    std::cout << std::endl << std::setw(15) << "";
    for (std::size_t i{0}; i < info.rxPktRcvdCntSize; ++i)
        std::cout << std::setw(10) << info.rxPktRcvdCnt[i];
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
