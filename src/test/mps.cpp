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

template<typename T>
inline
void printVal(const T& v)
{
    std::cout << v;
}

template<>
inline
void printVal(const bool& v)
{
    std::cout << std::boolalpha << v;
}

template<>
inline
void printVal(const uint8_t& v)
{
    std::cout << unsigned(v);
}

template<typename T>
void printPair(const std::string& name, const std::pair<bool, T>& v, bool useHex = false )
{
    size_t w = 8;

    std::cout << std::left << std::setw(17) << name; 
    std::cout << "= ";

    if (useHex)
    {
        std::cout << "0x" << std::hex;
        w -= 2;
    }
    std::cout << std::setw(w);
    printVal(v.second);

    std::cout << std::dec;
    std::cout << " [";
    std::cout << std::string(v.first?"valid":"non-valid");
    std::cout << "]";
    std::cout << std::right;
    std::cout << std::endl;
}

template<typename T>
void printArray(const std::string& regName, const std::string& varName, const T& p, bool useHex = false, size_t hexW = 2)
{
    std::cout << regName << ": [" << std::string(p.first?"valid":"non-valid") << "]" << std::endl;
    
    std::cout << std::setw(10) << "Index:";
    for (std::size_t i{0}; i < p.second.size(); ++i)
        std::cout << std::setw(12) << i;

    std::cout << std::endl << std::setw(9) << varName << ":";

    size_t w = 12;
    if (useHex)
        w -= hexW;

    for (std::size_t i{0}; i < p.second.size(); ++i)
    {
        std::cout << std::setw(w);
        if (useHex)
            std::cout << "0x" << std::setfill('0') << std::setw(hexW) << std::hex;

         printVal(p.second.at(i)); 

         std::cout << std::dec << std::setfill(' ');;
    }
    std::cout << std::endl;
} 

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
    mpsNode ( IMpsNode::create(mpsRoot) )
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
    printPair( "appId",            info.appId                    );
    printPair( "version",          info.version                  );
    printPair( "enable",           info.enable                   );
    printPair( "lcls1Mode",        info.lcls1Mode                );
    printPair( "byteCount",        info.byteCount                );
    printPair( "digitalEn",        info.digitalEn                );
    printPair( "beamDestMask",     info.beamDestMask,     true   );
    printPair( "altDestMask",      info.altDestMask,      true   );
    printPair( "msgCnt",           info.msgCnt                   );
    printPair( "lastMsgAppId",     info.lastMsgAppId             );
    printPair( "lastMsgLcls",      info.lastMsgLcls              );
    printPair( "lastMsgTimestamp", info.lastMsgTimestamp         );

    printArray( "lastMsgByte", "Status", info.lastMsgByte, true );

    printPair( "txLinkUp",            info.txLinkUp              );
    printPair( "txLinkUpCnt",         info.txLinkUpCnt           );

    printArray( "rxLinkUp",    "Status",   info.rxLinkUp    );
    printArray( "rxLinkUpCnt", "Counters", info.rxLinkUpCnt );

    printPair( "mpsSlot",            info.mpsSlot                );
    printPair( "appType",            info.appType                );
    printPair( "pllLocked",          info.pllLocked              );
    printPair( "rollOverEn",         info.rollOverEn             );
    printPair( "txPktSentCnt",       info.txPktSentCnt           );

    printArray( "rxPktRcvd", "Counters", info.rxPktRcvdCnt );

    std::cout << "=============================" << std::endl;

    std::cout << "\033[34A\r";
}

void Tester::printInfo()
{
    std::cout << std::endl;
    std::cout << "====================================================" << std::endl;
    std::cout << "Read all registers using the get functions" << std::endl;
    std::cout << "====================================================" << std::endl;
    printPair( "appId",            mpsNode->getAppId()             );
    printPair( "version",          mpsNode->getVersion()           );
    printPair( "enable",           mpsNode->getEnable()            );
    printPair( "lcls1Mode",        mpsNode->getLcls1Mode()         );
    printPair( "byteCount",        mpsNode->getByteCount()         );
    printPair( "digitalEn",        mpsNode->getDigitalEnable()     );
    printPair( "beamDestMask",     mpsNode->getBeamDestMask()      );
    printPair( "altDestMask",      mpsNode->getAltDestMask()       );
    printPair( "msgCnt",           mpsNode->getMsgCount()          );
    printPair( "lastMsgAppId",     mpsNode->getLastMsgAppId()      );
    printPair( "lastMsgLcls",      mpsNode->getLastMsgLcls()       );
    printPair( "lastMsgTimestamp", mpsNode->getLastMsgTimeStamp()  );

    
    size_t n = mpsNode->getLastMsgByteSize();
    std::cout << "lastMsgByte =" << std::endl;
    std::cout << std::setw(10) << "Index:";
    for (size_t i{0}; i < n; ++i)
        std::cout << std::setw(12) << i;
    std::cout << std::endl << std::setw(10) << "Status:";
    for (size_t i{0}; i < n; ++i)
        std::cout << std::setw(10) << "0x" << std::setfill('0') << std::setw(2) << std::hex <<  unsigned(mpsNode->getLastMsgByte(i).second) << std::setfill(' ') << std::dec;
    std::cout << std::endl;

    printPair( "txLinkUp",    mpsNode->getTxLinkUp()    );
    printPair( "txLinkUpCnt", mpsNode->getTxLinkUpCnt() );

    n = mpsNode->getRxLinkUpCntSize();
    std::cout << "rxLinkUp =" << std::endl;
    std::cout << std::setw(10) << "Index:";
    for (size_t i{0}; i < n; ++i)
        std::cout << std::setw(12) << i;
    std::cout << std::endl << std::setw(10) << "Status:";
    for (size_t i{0}; i < n; ++i)
        std::cout << std::setw(12) << std::boolalpha << mpsNode->getRxLinkUp(i).second;
    std::cout << std::endl << std::setw(10) << "Counters:";
    for (size_t i{0}; i < n; ++i)
        std::cout << std::setw(12) << mpsNode->getRxLinkUpCnt(i).second;
    std::cout << std::endl;
 
    printPair( "mpsSlot",      mpsNode->getMpsSlot()      );
    printPair( "appType",      mpsNode->getAppType()      );
    printPair( "pllLocked",    mpsNode->getPllLocked()    );
    printPair( "rollOverEn",   mpsNode->getRollOverEn()   );
    printPair( "txPktSentCnt", mpsNode->getTxPktSentCnt() );

    n = mpsNode->getRxPktRcvdCntSize();
    std::cout << "rxPktRcvdCnt =" << std::endl;
    std::cout << std::setw(10) << "Index:";
    for (std::size_t i{0}; i < n; ++i)
        std::cout << std::setw(12) << i;
    std::cout << std::endl << std::setw(10) << "Counters:";
    for (std::size_t i{0}; i < n; ++i)
        std::cout << std::setw(12) << mpsNode->getRxPktRcvdCnt(i).second;
    std::cout << std::endl;

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
