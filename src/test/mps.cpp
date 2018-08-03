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
#include "helpers.h"

boost::atomic<bool> run(false);

void intHandler(int dummy)
{
    run = false;
}

class Tester
{
public:
    Tester(Path root);
    ~Tester();

    void printInfo();
    void startPolling();

    void setAppId(uint16_t id);
    void setEnable(bool en);

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
    mpsNode ( IMpsNode::create(mpsRoot) )
{
}

Tester::~Tester()
{
    if (polling)
        std::cout << "\033[47B";

    std::cout << "Tester object destroyed" << std::endl;
}

void Tester::mpsInfoReceiver(mps_infoData_t info)
{
    std::cout << "MPS message received!. Message # " << ++msgCnt << std::endl;
    std::cout << "=============================" << std::endl;
    printPair( "appId",             info.appId                  );
    printPair( "version",           info.version                );
    printPair( "enable",            info.enable                 );
    printPair( "lcls1Mode",         info.lcls1Mode              );
    printPair( "byteCount",         info.byteCount              );
    printPair( "digitalEn",         info.digitalEn              );
    printPair( "beamDestMask",      info.beamDestMask,  true    );
    printPair( "altDestMask",       info.altDestMask,   true    );
    printPair( "msgCnt",            info.msgCnt                 );
    printPair( "mpsSlot",           info.mpsSlot                );
    printPair( "appType",           info.appType                );

    std::cout << std::endl;

    printPair( "lastMsgAppId",      info.lastMsgAppId           );
    printPair( "lastMsgLcls",       info.lastMsgLcls            );
    printPair( "lastMsgTimestamp",  info.lastMsgTimestamp       );
    printArray( "lastMsgByte",      info.lastMsgByte,   true    );

    std::cout << std::endl;

    printPair( "pllLocked",        info.pllLocked               );
    printPair( "rollOverEn",       info.rollOverEn              );
    printPair( "txLinkUp",         info.txLinkUp                );
    printPair( "txLinkUpCnt",      info.txLinkUpCnt             );
    printPair( "txPktSentCnt",     info.txPktSentCnt            );

    std::vector<std::string> n({"rxLinkUp", "rxLinkUpCnt","rxPktRcvd"});
    std::vector< std::pair< bool, std::vector<uint32_t> > > v
    {
        make_pair(info.rxLinkUp.first, std::vector<uint32_t>(info.rxLinkUp.second.begin(), info.rxLinkUp.second.end() )),
        info.rxLinkUpCnt,
        info.rxPktRcvdCnt
    };
    printArray( n, v, false);

    std::cout << "=============================" << std::endl;

    std::cout << "\033[47A\r";
}

void Tester::printInfo()
{
    std::cout << std::endl;
    std::cout << "====================================================" << std::endl;
    std::cout << "Read all registers using the get functions" << std::endl;
    std::cout << "====================================================" << std::endl;
    printPair( "appId",             mpsNode->getAppId()             );
    printPair( "version",           mpsNode->getVersion()           );
    printPair( "enable",            mpsNode->getEnable()            );
    printPair( "lcls1Mode",         mpsNode->getLcls1Mode()         );
    printPair( "byteCount",         mpsNode->getByteCount()         );
    printPair( "digitalEn",         mpsNode->getDigitalEnable()     );
    printPair( "beamDestMask",      mpsNode->getBeamDestMask()      );
    printPair( "altDestMask",       mpsNode->getAltDestMask()       );
    printPair( "msgCnt",            mpsNode->getMsgCount()          );
    printPair( "mpsSlot",           mpsNode->getMpsSlot()           );
    printPair( "appType",           mpsNode->getAppType()           );

    std::cout << std::endl;

    printPair( "lastMsgAppId",      mpsNode->getLastMsgAppId()      );
    printPair( "lastMsgLcls",       mpsNode->getLastMsgLcls()       );
    printPair( "lastMsgTimestamp",  mpsNode->getLastMsgTimeStamp()  );

    size_t n = mpsNode->getLastMsgByteSize();
    std::cout << std::setw(20) << "lastMsgByte" << std::endl;
    for (size_t i{0}; i < n; ++i)
        printPair( std::to_string(static_cast<long long int>(i)), mpsNode->getLastMsgByte(i), true, 3, 11, 15 );

    std::cout << std::endl;

    printPair( "pllLocked",         mpsNode->getPllLocked()         );
    printPair( "rollOverEn",        mpsNode->getRollOverEn()        );
    printPair( "txLinkUp",          mpsNode->getTxLinkUp()          );
    printPair( "txLinkUpCnt",       mpsNode->getTxLinkUpCnt()       );
    printPair( "txPktSentCnt",      mpsNode->getTxPktSentCnt()      );

    n = mpsNode->getRxLinkUpCntSize();
    std::cout << std::setw(20) << "rxLinkUp";
    std::cout << std::setw(16) << "rxLinkUpCnt";
    std::cout << std::setw(16) << "rxPktRcvd";
    std::cout << std::endl;

    for (size_t i{0}; i < n; ++i)
    {
        std::cout << std::setw(4) << i;
        std::cout << std::setw(16) << std::boolalpha << mpsNode->getRxLinkUp(i).second;
        std::cout << std::setw(16) << mpsNode->getRxLinkUpCnt(i).second;
        std::cout << std::setw(16) << mpsNode->getRxPktRcvdCnt(i).second;
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

void Tester::setAppId(uint16_t id)
{
    std::cout << "Changing the app ID to " << unsigned(id) << "... ";
    if (mpsNode->setAppId(id))
        std::cout << "Done!" << std::endl;
    else
        std::cout << "Error!" << std::endl;
}

void Tester::setEnable(bool en)
{
    std::cout << "Setting the enable bit to " << std::boolalpha << en << "... ";
    if (mpsNode->setEnable(en))
        std::cout << "Done!" << std::endl;
    else
        std::cout << "Error!" << std::endl;
}

void usage(const char* name)
{
    std::cout << "Usage: " << name << "-a <IP_address> -Y <Yaml_top> [-i <App_ID>] [-e <Enable>] [-c] [-h]" << std::endl;
    std::cout << "    -h              : show this message." << std::endl;
    std::cout << "    -a <IP_address> : IP address of the target FPGA." << std::endl;
    std::cout << "    -Y <Yaml_top>   : Path to YAML top level file." << std::endl;
    std::cout << "    -i <App_ID>     : Set new MPS APP_ID to <App_ID>." << std::endl;
    std::cout << "    -e <Enable>     : Set the MPS enable bit (0: disable; 1: enabled)." << std::endl;
    std::cout << "    -c              : Enable continuous read mode (default just one shot reading)" << std::endl;
    std::cout << std::endl;
}

int main(int argc, char **argv)
{
    signal(SIGINT, intHandler);
    unsigned char buf[sizeof(struct in6_addr)];
    std::string ipAddr;
    std::string yamlDoc;
    int id     = -1;
    int en     = -1;
    bool cont  = false;
    int c;

    while((c =  getopt(argc, argv, "a:Y:i:e:c")) != -1)
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
            case 'i':
                if ((1 != sscanf(optarg, "%d", &id)) || (id < 0) || (id > 1023))
                {
                    std::cout << "Invalid ID value..." << std::endl;
                    exit(1);
                }
                break;
            case 'e':
                if ((1 != sscanf(optarg, "%d", &en)) || (en < 0) || (en > 1))
                {
                    std::cout << "Invalid Enable value..." << std::endl;
                    exit(1);
                }
                break;
            case 'c':
                cont = true;
                break;
            default:
                usage(argv[0]);
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

    std::cout << std::endl << std::endl;

    Path mpsRoot;

    try
    {
        mpsRoot = root->findByName(mpsRootName);

        Tester t( mpsRoot );

        if (-1 != id)
            t.setAppId(id);

        if (-1 != en)
            t.setEnable(en?true:false);

        if (!cont)
        {
            t.printInfo();
        }
        else
        {
            t.startPolling();

            run = true;
            while(run)
                std::this_thread::sleep_for( std::chrono::seconds( 1 ) );
        }
    }
    catch (CPSWError &e)
    {
        printf("CPSW error: %s not found!\n", e.getInfo().c_str());
        return 1;
    }

    return 0;
}
