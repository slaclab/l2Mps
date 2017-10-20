#include <stdio.h>
#include <iostream>
#include <string.h>
#include <iomanip>
#include <vector>
#include <inttypes.h>
#include <boost/shared_ptr.hpp>
#include <cpsw_api_user.h>
#include <stdexcept>
#include <yaml-cpp/yaml.h>
#include <getopt.h>
#include <stdexcept>
#include <arpa/inet.h>

#include "l2Mps_bcm.h"

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

    Path mpsRoot;
    try
    {
        mpsRoot = root->findByName(mpsRootName);

        std::array<MpsBcm, 2> myMpsBcm;

        for (std::size_t i {0}; i < 2; ++i)
        {
            std::cout << "BCM for AMC[" << i << "]: BCM[" << i << "]" << std::endl;
            std::cout << "==============================================" << std::endl;
            myMpsBcm[i] = MpsBcmFactory::create(mpsRoot, i);
            std::cout << "==============================================" << std::endl;

            std::cout << std::endl;

            int n;
            std::array<int, 4> ch;

            try
            {
                ch[1] = 0;  // LCLS1
                ch[2] = 0;  // Min
                ch[3] = 0;  // T0

                for (std::size_t j {0}; j < numBcmChs; ++j)
                {
                    ch[0] = j; // 0=BCM, 1=DIFF
                    std::cout << "Channel = " << j << std::endl;
                    std::cout << "==============" << std::endl;

                    n = myMpsBcm[i]->getThrCount(ch);
                    std::cout << "  Thr count     = "    << unsigned(n) << std::endl;
                    std::cout << "  Byte map      = "      << myMpsBcm[i]->getByteMap(ch) << std::endl;
                    std::cout << "  Idle Enabled  = "  << std::boolalpha << myMpsBcm[i]->getIdleEn(ch) << std::endl;
                    std::cout << "  Lcls1 Enabled = " << std::boolalpha << myMpsBcm[i]->getLcls1En(ch) << std::endl;
                    std::cout << "  Alt Enabled   = "   << std::boolalpha << myMpsBcm[i]->getAltEn(ch) << std::endl;
                    std::cout << std::endl;


                    std::cout << "  LCLS1 thresholds:" << std::endl;
                    std::cout << "  =========================" << std::endl;

                    try
                    {
                        uint32_t thrMax, thrMin;
                        bool enMax, enMin;
                        ch[1] = 0;  // LCLS1 table
                        ch[2] = 0;  // Min
                        thrMin = myMpsBcm[i]->getThreshold(ch);
                        enMin = myMpsBcm[i]->getThresholdEn(ch);
                        ch[2] = 1;  // Max
                        thrMax = myMpsBcm[i]->getThreshold(ch);
                        enMax = myMpsBcm[i]->getThresholdEn(ch);

                        std::cout << "    Min En = " << std::boolalpha << enMin << std::endl;
                        std::cout << "    Max En = " << std::boolalpha << enMax << std::endl;
                        std::cout << "    Min    = " << unsigned(thrMin) << std::endl;
                        std::cout << "    Max    = " << unsigned(thrMax) << std::endl;
                        std::cout << std::endl;

                    }
                    catch (std::exception &e)
                    {
                        printf("    Error on LCLS 1 table section: %s\n", e.what());
                        std::cout << std::endl;
                    }

                    std::cout << "  IDLE thresholds:" << std::endl;
                    std::cout << "  =========================" << std::endl;

                    try
                    {
                        uint32_t thrMax, thrMin;
                        bool enMax, enMin;
                        ch[1] = 1;  // IDLE table
                        ch[2] = 0;  // Min
                        thrMin = myMpsBcm[i]->getThreshold(ch);
                        enMin = myMpsBcm[i]->getThresholdEn(ch);
                        ch[2] = 1;  // Max
                        thrMax = myMpsBcm[i]->getThreshold(ch);
                        enMax = myMpsBcm[i]->getThresholdEn(ch);

                        std::cout << "    Min En = " << std::boolalpha << enMin << std::endl;
                        std::cout << "    Max En = " << std::boolalpha << enMax << std::endl;
                        std::cout << "    Min    = " << unsigned(thrMin) << std::endl;
                        std::cout << "    Max    = " << unsigned(thrMax) << std::endl;
                        std::cout << std::endl;

                    }
                    catch (std::exception &e)
                    {
                        printf("    Error on IDLE table section: %s\n", e.what());
                        std::cout << std::endl;
                    }

                    std::cout << "  ALT thresholds:" << std::endl;
                    std::cout << "  =========================" << std::endl;

                    try
                    {
                        uint32_t* thrMax = new uint32_t[n]; 
                        uint32_t* thrMin = new uint32_t[n];
                        bool* enMax = new bool[n];
                        bool* enMin = new bool[n];
                        ch[1] = 3;  // ALT table
                        for (std::size_t k {0}; k < n; ++k)
                        {
                            ch[3] = k;  // T(k)
                            ch[2] = 0;  // Min
                            thrMin[k] = myMpsBcm[i]->getThreshold(ch);
                            enMin[k] = myMpsBcm[i]->getThresholdEn(ch);
                            ch[2] = 1;  // Max
                            thrMax[k] = myMpsBcm[i]->getThreshold(ch);
                            enMax[k] = myMpsBcm[i]->getThresholdEn(ch);
                        }

                        std::cout << "    Min En = ";
                        for (std::size_t k {0}; k < n; ++k)
                            std::cout << std::boolalpha << enMin[k] << ", ";
                        std::cout << std::endl;
                        std::cout << "    Max En = ";
                        for (std::size_t k {0}; k < n; ++k)
                            std::cout << std::boolalpha << enMax[k] << ", ";
                        std::cout << std::endl;
                        std::cout << "    Min     = ";
                        for (std::size_t k {0}; k < n; ++k)
                            std::cout << unsigned(thrMin[k]) << ", ";
                        std::cout << std::endl;
                        std::cout << "    Max     = ";
                        for (std::size_t k {0}; k < n; ++k)
                            std::cout << unsigned(thrMax[k]) << ", ";
                        std::cout << std::endl;
                        std::cout << std::endl;
                        delete[] thrMax, thrMin, enMax, enMin;

                    }
                    catch (std::exception &e)
                    {
                        printf("    Error on ALT table section: %s\n", e.what());
                        std::cout << std::endl;
                    }

                    std::cout << "  STD thresholds:" << std::endl;
                    std::cout << "  =========================" << std::endl;

                    try
                    {
                        uint32_t* thrMax = new uint32_t[n]; 
                        uint32_t* thrMin = new uint32_t[n];
                        bool* enMax = new bool[n];
                        bool* enMin = new bool[n];
                        ch[1] = 2;  // STD table
                        for (std::size_t k {0}; k < n; ++k)
                        {
                            ch[3] = k;  // T(k)
                            ch[2] = 0;  // Min
                            thrMin[k] = myMpsBcm[i]->getThreshold(ch);
                            enMin[k] = myMpsBcm[i]->getThresholdEn(ch);
                            ch[2] = 1;  // Max
                            thrMax[k] = myMpsBcm[i]->getThreshold(ch);
                            enMax[k] = myMpsBcm[i]->getThresholdEn(ch);
                        }

                        std::cout << "    Min En = ";
                        for (std::size_t k {0}; k < n; ++k)
                            std::cout << std::boolalpha << enMin[k] << ", ";
                        std::cout << std::endl;
                        std::cout << "    Max En = ";
                        for (std::size_t k {0}; k < n; ++k)
                            std::cout << std::boolalpha << enMax[k] << ", ";
                        std::cout << std::endl;
                        std::cout << "    Min = ";
                        for (std::size_t k {0}; k < n; ++k)
                            std::cout << unsigned(thrMin[k]) << ", ";
                        std::cout << std::endl;
                        std::cout << "    Max = ";
                        for (std::size_t k {0}; k < n; ++k)
                            std::cout << unsigned(thrMax[k]) << ", ";
                        std::cout << std::endl;
                        std::cout << std::endl;
                        delete[] thrMax, thrMin, enMax, enMin;

                    }
                    catch (std::exception &e)
                    {
                        printf("    Error on STD table section: %s\n", e.what());
                        std::cout << std::endl;
                    }                    
                }

            }
            catch (std::exception &e)
            {
                printf("Error channel info section: %s\n", e.what());
            }            
        }


    }
    catch (CPSWError &e)
    {
        printf("CPSW error: %s not found!\n", e.getInfo().c_str());
        return 1;
    }

    return 0;
}   