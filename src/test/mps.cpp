#define _GLIBCXX_USE_NANOSLEEP    // Workaround to use std::this_thread::sleep_for

#include <signal.h>
#include <iomanip>
#include <yaml-cpp/yaml.h>
#include <arpa/inet.h>
#include <thread>
#include <chrono>
#include <functional>
#include <boost/atomic.hpp>

#include "l2Mps_mps.h"

boost::atomic<bool> run(false);

void intHandler(int dummy)
{
    run = false;
}

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

class Tester
{
public: 
    Tester(Path root);
    ~Tester();

    void printInfo();
    void startPolling();

private:
    bool    polling;
    int     msgCnt;
    MpsNode mpsNode;

    void mpsInfoReceiver(mps_infoData_t info);
};

Tester::Tester(Path mpsRoot)
:
    polling ( false ),
    msgCnt  ( 0 ),
    mpsNode ( MpsNodeFactory::create(mpsRoot) )
{
    std::cout << "Tester object created" << std::endl;
}

Tester::~Tester()
{
    if (polling)
        std::cout << "\033[33B";

    std::cout << "Tester object destroyed" << std::endl;
}

void Tester::mpsInfoReceiver(mps_infoData_t info)
{
    std::cout << "MPS message received!. Message # " << ++msgCnt << std::endl;
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
        std::cout << std::setw(12) << i;
    std::cout << std::endl << std::setw(10) << "Status:";
    for (std::size_t i{0}; i < info.lastMsgByte.size(); ++i)
        //std::cout << std::setw(12) << "0x" << std::hex << unsigned(info.lastMsgByte.at(i)) << std::dec;
        std::cout << std::setw(10) << "0x" << std::setfill('0') << std::setw(2) << std::hex <<  unsigned(info.lastMsgByte.at(i)) << std::setfill(' ') << std::dec;
    std::cout << std::endl;

    std::cout << "txLinkUp          = "  << std::boolalpha <<    info.txLinkUp                   << std::endl;
    std::cout << "txLinkUpCnt       = "  <<                      info.txLinkUpCnt                << std::endl;

    std::cout << "rxLinkUp =" << std::endl;
    std::cout << std::setw(10) << "Index:";
    for (std::size_t i{0}; i < info.rxLinkUp.size(); ++i)
        std::cout << std::setw(12) << i;
    std::cout << std::endl << std::setw(10) << "Status:";
    for (std::size_t i{0}; i < info.rxLinkUp.size(); ++i)
        std::cout << std::setw(12) << std::boolalpha << info.rxLinkUp.at(i);
    std::cout << std::endl << std::setw(10) << "Counters:";
    for (std::size_t i{0}; i < info.rxLinkUpCnt.size(); ++i)
        std::cout << std::setw(12) << info.rxLinkUpCnt.at(i);
    std::cout << std::endl;

    std::cout << "mpsSlot           = "  << std::boolalpha <<    info.mpsSlot                    << std::endl;
    std::cout << "appType           = "  <<                      info.appType                    << std::endl;
    std::cout << "pllLocked         = "  << std::boolalpha <<    info.pllLocked                  << std::endl;
    std::cout << "rollOverEn        = "  <<                      info.rollOverEn                 << std::endl;
    std::cout << "txPktSentCnt      = "  <<                      info.txPktSentCnt               << std::endl;

    std::cout << "rxPktRcvd =" << std::endl;
    std::cout << std::setw(10) << "Index:";
    for (std::size_t i{0}; i < info.rxPktRcvdCnt.size(); ++i)
        std::cout << std::setw(12) << i;
    std::cout << std::endl << std::setw(10) << "Counters:";
    for (std::size_t i{0}; i < info.rxPktRcvdCnt.size(); ++i)
        std::cout << std::setw(12) << info.rxPktRcvdCnt[i];
    std::cout << std::endl;

    std::cout << "=============================" << std::endl;

    std::cout << "\033[32A\r";
}

void Tester::printInfo()
{
    std::cout << std::endl;
    std::cout << "====================================================" << std::endl;
    std::cout << "Read all registers using the get functions" << std::endl;
    std::cout << "====================================================" << std::endl;
    std::cout << "appId             = "  <<                       mpsNode->getAppId()                      << std::endl;
    std::cout << "version           = "  <<              unsigned(mpsNode->getVersion())                   << std::endl;
    std::cout << "enable            = "  << std::boolalpha <<     mpsNode->getEnable()                     << std::endl;
    std::cout << "lcls1Mode         = "  << std::boolalpha <<     mpsNode->getLcls1Mode()                  << std::endl;
    std::cout << "byteCount         = "  <<             unsigned( mpsNode->getByteCount())                 << std::endl;
    std::cout << "digitalEn         = "  << std::boolalpha <<     mpsNode->getDigitalEnable()              << std::endl;
    std::cout << "beamDestMask      = "  << "0x" << std::hex <<   mpsNode->getBeamDestMask() << std::dec   << std::endl;
    std::cout << "altDestMask       = "  << "0x" << std::hex <<   mpsNode->getAltDestMask() << std::dec    << std::endl;
    std::cout << "msgCnt            = "  <<                       mpsNode->getMsgCount()                   << std::endl;
    std::cout << "lastMsgAppId      = "  <<                       mpsNode->getLastMsgAppId()               << std::endl;
    std::cout << "lastMsgLcls       = "  << std::boolalpha <<     mpsNode->getLastMsgLcls()                << std::endl;
    std::cout << "lastMsgTimestamp  = "  <<                       mpsNode->getLastMsgTimeStamp()           << std::endl;

    {
        std::size_t n = mpsNode->getLastMsgByteSize();
        std::cout << "lastMsgByte =" << std::endl;
        std::cout << std::setw(10) << "Index:";
        for (std::size_t i{0}; i < n; ++i)
            std::cout << std::setw(12) << i;
        std::cout << std::endl << std::setw(10) << "Status:";
        for (std::size_t i{0}; i < n; ++i)
            std::cout << std::setw(10) << "0x" << std::setfill('0') << std::setw(2) << std::hex <<  unsigned(mpsNode->getLastMsgByte(i)) << std::setfill(' ') << std::dec;
        std::cout << std::endl;
    }

    std::cout << "txLinkUp          = "  << std::boolalpha <<    mpsNode->getTxLinkUp()                   << std::endl;
    std::cout << "txLinkUpCnt       = "  <<                      mpsNode->getTxLinkUpCnt()                << std::endl;

    {
        std::size_t n = mpsNode->getRxLinkUpCntSize();
        std::cout << "rxLinkUp =" << std::endl;
        std::cout << std::setw(10) << "Index:";
        for (std::size_t i{0}; i < n; ++i)
            std::cout << std::setw(12) << i;
        std::cout << std::endl << std::setw(10) << "Status:";
        for (std::size_t i{0}; i < n; ++i)
            std::cout << std::setw(12) << std::boolalpha << mpsNode->getRxLinkUp(i);
        std::cout << std::endl << std::setw(10) << "Counters:";
        for (std::size_t i{0}; i < n; ++i)
            std::cout << std::setw(12) << mpsNode->getRxLinkUpCnt(i);
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
            std::cout << std::setw(12) << i;
        std::cout << std::endl << std::setw(10) << "Counters:";
        for (std::size_t i{0}; i < n; ++i)
            std::cout << std::setw(12) << mpsNode->getRxPktRcvdCnt(i);
        std::cout << std::endl;
    }

    std::cout << "====================================================" << std::endl;
    std::cout << "Done!"<< std::endl;
    std::cout << std::endl;
}

void Tester::startPolling()
{
    std::cout << std::endl;
    std::cout << "====================================================" << std::endl;
    std::cout << "Starting MPS polling thread at 1Hz" << std::endl;
    std::cout << "====================================================" << std::endl;

    auto fp = std::bind(&Tester::mpsInfoReceiver, this, std::placeholders::_1);
    mpsNode->startPollThread(1, fp);

    polling = true;
}

int main(int argc, char **argv)
{
    signal(SIGINT, intHandler);
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

    Path mpsRoot;

    try
    {
        mpsRoot = root->findByName(mpsRootName);

        Tester t( mpsRoot );

        t.printInfo();
        t.startPolling();

        run = true;
        while(run)
            std::this_thread::sleep_for( std::chrono::seconds( 1 ) );
    }
    catch (CPSWError &e)
    {
        printf("CPSW error: %s not found!\n", e.getInfo().c_str());
        return 1;
    }

    return 0;
}
