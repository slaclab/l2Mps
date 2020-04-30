/**
 *-----------------------------------------------------------------------------
 * Title      : Common Platform MPS module test program
 * ----------------------------------------------------------------------------
 * File       : mps.cpp
 * Author     : Jesus Vasquez, jvasquez@slac.stanford.edu
 * Created    : 2017-10-20
 * ----------------------------------------------------------------------------
 * Description:
 * Application to read the common MPS related infromation from any application,
 * in order to test the Common Platform MPS module interface class.
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

Tester::Tester(Path root)
:
    polling ( false ),
    msgCnt  ( 0 ),
    mpsNode ( IMpsNode::create(root) )
{
}

Tester::~Tester()
{
    if (polling)
        std::cout << "\033[47B";
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


    // Print the soft input status, which is only available on Link Node applications
    if (mpsNode->getMpsSoftInputs())
    {
        std::cout << std::endl;
        std::cout << "Soft input status: " << std::endl;
        std::cout << "-------------------" std::endl;
        printPair( "  Input Values", info.softInputs.inputWord, true );
        printPair( "  Error Values", info.softInputs.errorWord, true );
        std::cout << "-------------------" std::endl;
    }
    std::cout << "=============================" << std::endl;

    std::cout << "\033[47A\r";
}

void Tester::printInfo()
{
    std::cout << std::endl;
    std::cout << "====================================================" << std::endl;
    std::cout << "Read all registers using the get functions" << std::endl;
    std::cout << "====================================================" << std::endl;
    printPair( "appId",             mpsNode->getAppId()                     );
    printPair( "version",           mpsNode->getVersion()                   );
    printPair( "enable",            mpsNode->getEnable()                    );
    printPair( "lcls1Mode",         mpsNode->getLcls1Mode()                 );
    printPair( "byteCount",         mpsNode->getByteCount()                 );
    printPair( "digitalEn",         mpsNode->getDigitalEnable()             );
    printPair( "beamDestMask",      mpsNode->getBeamDestMask(),     true    );
    printPair( "altDestMask",       mpsNode->getAltDestMask(),      true    );
    printPair( "msgCnt",            mpsNode->getMsgCount()                  );
    printPair( "mpsSlot",           mpsNode->getMpsSlot()                   );
    printPair( "appType",           mpsNode->getAppType()                   );

    std::cout << std::endl;

    printPair( "lastMsgAppId",      mpsNode->getLastMsgAppId()              );
    printPair( "lastMsgLcls",       mpsNode->getLastMsgLcls()               );
    printPair( "lastMsgTimestamp",  mpsNode->getLastMsgTimeStamp()          );


    {
        std::pair< bool, std::vector<uint32_t> > p;
        bool allValid = true;
        for (std::size_t i(0); i < mpsNode->getLastMsgByteSize() ; ++i)
        {
            std::pair<bool, uint32_t> v = mpsNode->getLastMsgByte(i);

            allValid &= v.first;

            p.second.push_back(v.second);
        }
        p.first = allValid;

        printArray("lastMsgByte", p, true);
    }

    std::cout << std::endl;

    printPair( "pllLocked",         mpsNode->getPllLocked()         );
    printPair( "rollOverEn",        mpsNode->getRollOverEn()        );
    printPair( "txLinkUp",          mpsNode->getTxLinkUp()          );
    printPair( "txLinkUpCnt",       mpsNode->getTxLinkUpCnt()       );
    printPair( "txPktSentCnt",      mpsNode->getTxPktSentCnt()      );

    {
        std::vector< std::pair< bool, std::vector<uint32_t> > > ps(3);
        std::vector<std::string> ns({"rxLinkUp", "rxLinkUpCnt", "rxPktRcvd"});

        ps.at(0).first = true;
        ps.at(1).first = true;
        ps.at(2).first = true;

        for (std::size_t i(0); i < mpsNode->getRxLinkUpCntSize(); ++i)
        {
            std::pair<bool, bool> v1 = mpsNode->getRxLinkUp(i);
            ps.at(0).first &= v1.first;
            ps.at(0).second.push_back(static_cast<uint32_t>(v1.second));

            std::pair<bool, uint32_t> v2 = mpsNode->getRxLinkUpCnt(i);
            ps.at(1).first &= v2.first;
            ps.at(1).second.push_back(v2.second);

            std::pair<bool, uint32_t> v3 = mpsNode->getRxPktRcvdCnt(i);
            ps.at(2).first &= v3.first;
            ps.at(2).second.push_back(v3.second);
        }

        printArray(ns, ps);
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

    IYamlSetIP setIP(ipAddr);
    Path root = IPath::loadYamlFile( yamlDoc.c_str(), "NetIODev", NULL, &setIP );

    std::cout << std::endl << std::endl;

    try
    {
        Tester t( root );

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
