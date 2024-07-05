/**
 *-----------------------------------------------------------------------------
 * Title      : BLEN application test program
 * ----------------------------------------------------------------------------
 * File       : blen.cpp
 * Author     : Jesus Vasquez, jvasquez@slac.stanford.edu
 * Created    : 2017-10-20
 * ----------------------------------------------------------------------------
 * Description:
 * Application to read the MPS related infromation from a BLEN application, in
 * order to test the BLEN interface class.
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

#include "l2Mps_mps.h"
#include "l2Mps_blen.h"
#include "helpers.h"

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

    MpsNode mpsNode;
    try
    {
        mpsNode = IMpsNode::create(root);

        std::string appType(mpsNode->getAppType().second);
        std::cout << "This application type is " << appType << std::endl;
        if ( appType.compare("BLEN") )
        {
            std::cout << "ERROR: This is not a BLEN application. Aborting." << std::endl;
            return 1;
        }
    }
    catch (std::runtime_error &e)
    {
        std::cout << "Error creating the MPS application: " << e.what() << std::endl;
        return 1;
    }

    std::array<MpsBlen, 2> myMpsBlen;

    for (std::size_t i {0}; i < 2; ++i)
    {
        std::cout << "BLEN for AMC[" << i << "]: BLEN[" << i << "]" << std::endl;
        std::cout << "====================================================" << std::endl;
        myMpsBlen[i] = boost::any_cast<MpsBlen>(mpsNode->getBayApp(i));
        std::cout << "====================================================" << std::endl;

        std::cout << std::endl;

        int n;

        for (int j = 0; j < numBlenChs; ++j)
        {
                try
                {
                    blen_channel_t blenCh = j;

                    std::cout << std::endl;
                    std::cout << "  ===============================================" << std::endl;
                    std::cout << "  Channel = " << j << ":" << std::endl;
                    std::cout << "  ===============================================" << std::endl;

                    std::cout << "    Thr channel  = " << unsigned(myMpsBlen[i]->getChannel(blenCh)) << std::endl;
                    printPair( "    Thr count", myMpsBlen[i]->getThrCount(blenCh)      );
                    printPair( "    Byte map",  myMpsBlen[i]->getByteMap(blenCh)       );
                    printPair( "    Idle EN",   myMpsBlen[i]->getIdleEn(blenCh)        );
                    printPair( "    Lcls1 EN",  myMpsBlen[i]->getLcls1En(blenCh)       );
                    printPair( "    Alt EN",    myMpsBlen[i]->getAltEn(blenCh)         );
                    printPair( "    Trip Val",  myMpsBlen[i]->getTripVal(blenCh)       );
                    printPair( "    Trip PID",  myMpsBlen[i]->getTripPID(blenCh), true );

                    std::cout << std::endl;
                    std::cout << "    Threshold tables:" << std::endl;
                    std::cout << "    ================================================" << std::endl;
                    std::cout << "    Table     " << "minEn     " << "min       " << "maxEn     " << "max       " << std::endl;
                    std::cout << "    ================================================" << std::endl;

                    blenThr_channel_t blenThrCh;
                    blenThrCh.appCh = blenCh;

                    try
                    {
                        blenThrCh.thrTb = thr_table_t{{0,0}};
                        printThrRow( "LCLS1", myMpsBlen[i], blenThrCh);
                    }
                    catch (std::exception &e)
                    {
                        printf("   Error on LCLS 1 table section: %s", e.what());
                    }

                    try
                    {
                        blenThrCh.thrTb = thr_table_t{{1,0}};
                        printThrRow( "IDLE", myMpsBlen[i], blenThrCh);
                    }
                    catch (std::exception &e)
                    {
                        printf("   Error on IDLE table section: %s", e.what());
                    }

                    for (int m = 0; m < maxThrCount; ++m)
                    {
                        try
                        {
                            blenThrCh.thrTb = thr_table_t{{2,m}};
                            std::stringstream name("");
                            name << "STD[" << m << "]";
                            printThrRow( name.str(), myMpsBlen[i], blenThrCh);
                        }
                        catch (std::exception &e)
                        {
                            printf("   Error on STD table section: %s", e.what());
                        }
                    }


                    for (int m = 0; m < maxThrCount; ++m)
                    {
                        try
                        {

                            blenThrCh.thrTb = thr_table_t{{1,0}};
                            std::stringstream name("");
                            name << "ALT[" << m << "]";
                            printThrRow( name.str(), myMpsBlen[i], blenThrCh);
                        }
                        catch (std::exception &e)
                        {
                            printf("   Error on ALT table section: %s", e.what());
                        }

                    }

                    std::cout << "    ==========================================" << std::endl;
                    std::cout << std::endl;
                    std::cout << "  ===============================================" << std::endl;
                    std::cout << std::endl;
                }
                catch (std::exception &e)
                {
                    printf("  Error channel info section: %s\n", e.what());
                }
        }

        std::cout << "====================================================" << std::endl;
        std::cout << std::endl;
    }


    return 0;
}
