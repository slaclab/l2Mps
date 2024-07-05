/**
 *-----------------------------------------------------------------------------
 * Title      : BCM application test program
 * ----------------------------------------------------------------------------
 * File       : bcm.cpp
 * Author     : Jesus Vasquez, jvasquez@slac.stanford.edu
 * Created    : 2017-10-20
 * ----------------------------------------------------------------------------
 * Description:
 * Application to read the MPS related infromation from a BCM application, in
 * order to test the BCM interface class.
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
#include "l2Mps_bpm.h"
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
        if ( appType.compare("BPM") )
        {
            std::cout << "ERROR: This is not a BPM application. Aborting." << std::endl;
            return 1;
        }
    }
    catch (std::runtime_error &e)
    {
        std::cout << "Error creating the MPS application: " << e.what() << std::endl;
        return 1;
    }

    std::array<MpsBpm, 2> myMpsBpm;

    for (std::size_t i {0}; i < 2; ++i)
    {
        std::cout << "BPM for AMC[" << i << "]: BPM[" << i << "]" << std::endl;
        std::cout << "====================================================" << std::endl;
        myMpsBpm[i] = boost::any_cast<MpsBpm>(mpsNode->getBayApp(i));
        std::cout << "====================================================" << std::endl;

        std::cout << std::endl;

        int n;

        for (int j = 0; j < numBpmChs; ++j)
        {
                try
                {
                    bpm_channel_t bpmCh = j;

                    std::cout << std::endl;
                    std::cout << "  ===============================================" << std::endl;
                    std::cout << "  Channel = " << j << ":" << std::endl;
                    std::cout << "  ===============================================" << std::endl;

                    std::cout << "    Thr channel  = " << unsigned(myMpsBpm[i]->getChannel(bpmCh)) << std::endl;
                    printPair( "    Thr count", myMpsBpm[i]->getThrCount(bpmCh)      );
                    printPair( "    Byte map",  myMpsBpm[i]->getByteMap(bpmCh)       );
                    printPair( "    Idle EN",   myMpsBpm[i]->getIdleEn(bpmCh)        );
                    printPair( "    Lcls1 EN",  myMpsBpm[i]->getLcls1En(bpmCh)       );
                    printPair( "    Alt EN",    myMpsBpm[i]->getAltEn(bpmCh)         );
                    printPair( "    Trip Val",  myMpsBpm[i]->getTripVal(bpmCh)       );
                    printPair( "    Trip PID",  myMpsBpm[i]->getTripPID(bpmCh), true );

                    std::cout << std::endl;
                    std::cout << "    Threshold tables:" << std::endl;
                    std::cout << "    ================================================" << std::endl;
                    std::cout << "    Table     " << "minEn     " << "min       " << "maxEn     " << "max       " << std::endl;
                    std::cout << "    ================================================" << std::endl;

                    bpmThr_channel_t bpmThrCh;
                    bpmThrCh.appCh = bpmCh;

                    try
                    {
                        bpmThrCh.thrTb = thr_table_t{{0,0}};
                        printThrRow( "LCLS1", myMpsBpm[i], bpmThrCh);
                    }
                    catch (std::exception &e)
                    {
                        printf("   Error on LCLS 1 table section: %s", e.what());
                    }

                    try
                    {
                        bpmThrCh.thrTb = thr_table_t{{1,0}};
                        printThrRow( "IDLE", myMpsBpm[i], bpmThrCh);
                    }
                    catch (std::exception &e)
                    {
                        printf("   Error on IDLE table section: %s", e.what());
                    }

                    for (int m = 0; m < maxThrCount; ++m)
                    {
                        try
                        {
                            bpmThrCh.thrTb = thr_table_t{{2,m}};
                            std::stringstream name("");
                            name << "STD[" << m << "]";
                            printThrRow( name.str(), myMpsBpm[i], bpmThrCh);
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

                            bpmThrCh.thrTb = thr_table_t{{1,0}};
                            std::stringstream name("");
                            name << "ALT[" << m << "]";
                            printThrRow( name.str(), myMpsBpm[i], bpmThrCh);
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
