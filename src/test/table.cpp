/**
 *-----------------------------------------------------------------------------
 * Title      : MPS threshold channel map test program
 * ----------------------------------------------------------------------------
 * File       : table.cpp
 * Author     : Jesus Vasquez, jvasquez@slac.stanford.edu
 * Created    : 2017-10-20
 * ----------------------------------------------------------------------------
 * Description:
 * Application to read the MPS threhold channel map from the Common Platform
 * MPS module.
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

#include <iomanip>
#include <yaml-cpp/yaml.h>
#include <arpa/inet.h>

#include "l2Mps_base.h"
#include "l2Mps_mps.h"
#include "l2Mps_thr.h"
#include "helpers.h"

//template<typename T>
//void printCell(std::pair<bool, T> p)
//{
//    std::cout << std::setw(10) << std::left;
//
//    if (p.first)
//        printVal(p.second);
//    else
//        std::cout << "[Invalid]";
//
//   std::cout << std::right;
//}

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

    IYamlSetIP setIP(ipAddr);
    Path root = IPath::loadYamlFile( yamlDoc.c_str(), "NetIODev", NULL, &setIP );

    try
    {
        MpsNode aMpsNode(IMpsNode::create(root));

        std::cout << std::endl;
        std::cout << "MPS application information:"<< std::endl;
        std::cout << "============================"<< std::endl;

        printPair( "Application ID",  aMpsNode->getAppId()         );
        printPair( "Mps enabled",     aMpsNode->getEnable()        );
        printPair( "LCLS-I mode",     aMpsNode->getLcls1Mode()     );
        printPair( "Byte count",      aMpsNode->getByteCount()     );
        printPair( "Digital enabled", aMpsNode->getDigitalEnable() );

        std::cout << std::endl;
        std::cout << "Threshold channel map:"<< std::endl;
        std::cout << "============================================================" << std::endl;
        std::cout << "Channel   ByteMap   ThrCount  Lcls1En?  IdleEn?   AltEn?    " << std::endl;
        std::cout << "============================================================" << std::endl;
        for (uint8_t ch = 0 ; ch < maxChannelCount ; ch++)
        {
            std::cout << std::setw(2) << unsigned(ch) << std::string(8, ' ');
            try
            {
                Path mpsRoot = aMpsNode->getMpsRoot();
                ThrChannel aThr(IThrChannel::create(mpsRoot, ch));

                printCell(aThr->getByteMap());
                printCell(aThr->getThrCount());
                printCell(aThr->getLcls1En());
                printCell(aThr->getIdleEn());
                printCell(aThr->getAltEn());
                std::cout << std::endl;
            }
            catch (std::runtime_error &e)
            {
                std::cout << "Error while trying to read the threhold channel info: " << e.what() << std::endl;
            }
        }
    }
    catch (std::runtime_error &e)
    {
        std::cout << "Error creating the MPS application: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
