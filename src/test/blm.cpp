#include <iomanip>
#include <yaml-cpp/yaml.h>
#include <arpa/inet.h>

#include "l2Mps_mps.h"
#include "l2Mps_blm.h"

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

    try
    {
        Path mpsRoot = root->findByName(mpsRootName);

        {
            MpsNode mpsNode = MpsNodeFactory::create(mpsRoot);

            std::string appType(mpsNode->getAppType().second);
            std::cout << "This application type is " << appType << std::endl;
            if ( appType.compare("BLM") & appType.compare("MPS_6CH") & appType.compare("MPS_24CH") )
            {
                std::cout << "ERROR: This is not a BLM application. Aborting." << std::endl;
                return 1;
            }
        }

        std::array<MpsBlm, 2> myMpsBlm;

        for (std::size_t i {0}; i < 2; ++i)
        {
            std::cout << "BLM for AMC[" << i << "]: BLM[" << i << "]" << std::endl;
            std::cout << "====================================================" << std::endl;
            myMpsBlm[i] = MpsBlmFactory::create(mpsRoot, i);
            std::cout << "====================================================" << std::endl;

            std::cout << std::endl;

            int n;
            
            for (int j = 0; j < numBlmChs; ++j)
            {
                for (int k = 0; k <numBlmIntChs; ++k)
                {
                    try 
                    {
                        blm_channel_t blmCh = blm_channel_t{{j,k}};

                        std::cout << std::endl;
                        std::cout << "  ===============================================" << std::endl;
                        std::cout << "  Channel = (" << j << ", " << k << "):" << std::endl;
                        std::cout << "  ===============================================" << std::endl;

                        std::cout << "    Thr channel =   " << myMpsBlm[i]->getChannel(blmCh) << std::endl;
                        std::cout << "    Thr count =     " << myMpsBlm[i]->getThrCount(blmCh) << std::endl;
                        std::cout << "    Byte map =      " << myMpsBlm[i]->getByteMap(blmCh) << std::endl;
                        std::cout << "    Idle Enabled =  " << std::boolalpha << myMpsBlm[i]->getIdleEn(blmCh) << std::endl;
                        std::cout << "    Lcls1 Enabled = " << std::boolalpha << myMpsBlm[i]->getLcls1En(blmCh) << std::endl;
                        std::cout << "    Alt Enabled =   " << std::boolalpha << myMpsBlm[i]->getAltEn(blmCh) << std::endl;

                        std::cout << std::endl;
                        std::cout << "    Threshold tables:" << std::endl;
                        std::cout << "    ==========================================" << std::endl;
                        std::cout << "    Table     " << "   minEn" << "     min" << "   maxEn" << "     max" << std::endl;
                        std::cout << "    ==========================================" << std::endl;

                        blmThr_channel_t blmThrCh;
                        blmThrCh.appCh = blmCh;
                        
                        try
                        {
                            blmThrCh.thrTb = thr_table_t{{0,0}};
                            std::cout << "    LCLS1     ";
                            
                            std::cout << std::setw(8) << std::boolalpha << myMpsBlm[i]->getThresholdMinEn(blmThrCh);
                            std::cout << std::setw(8) << myMpsBlm[i]->getThresholdMin(blmThrCh);
                            std::cout << std::setw(8) << std::boolalpha << myMpsBlm[i]->getThresholdMaxEn(blmThrCh);
                            std::cout << std::setw(8) << myMpsBlm[i]->getThresholdMax(blmThrCh);
                            std::cout << std::endl;
                        }
                        catch (std::exception &e)
                        {
                            printf("   Error on LCLS 1 table section: %s", e.what());
                        }

                        try
                        {
                            blmThrCh.thrTb = thr_table_t{{1,0}};
                            std::cout << "    IDLE      ";
                            
                            std::cout << std::setw(8) << std::boolalpha << myMpsBlm[i]->getThresholdMinEn(blmThrCh);
                            std::cout << std::setw(8) << myMpsBlm[i]->getThresholdMin(blmThrCh);
                            std::cout << std::setw(8) << std::boolalpha << myMpsBlm[i]->getThresholdMaxEn(blmThrCh);
                            std::cout << std::setw(8) << myMpsBlm[i]->getThresholdMax(blmThrCh);
                            std::cout << std::endl;
                        }
                        catch (std::exception &e)
                        {
                            printf("   Error on IDLE table section: %s", e.what());
                        }
                        
                        for (int m = 0; m < maxThrCount; ++m)
                        {
                            try
                            {
                                blmThrCh.thrTb = thr_table_t{{2,m}};
                                std::cout << "    STD[" << m << "]    ";
                                
                                std::cout << std::setw(8) << std::boolalpha << myMpsBlm[i]->getThresholdMinEn(blmThrCh);
                                std::cout << std::setw(8) << myMpsBlm[i]->getThresholdMin(blmThrCh);
                                std::cout << std::setw(8) << std::boolalpha << myMpsBlm[i]->getThresholdMaxEn(blmThrCh);
                                std::cout << std::setw(8) << myMpsBlm[i]->getThresholdMax(blmThrCh);
                                std::cout << std::endl;

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
                        
                                blmThrCh.thrTb = thr_table_t{{1,0}};
                                std::cout << "    ALT[" << m << "]    ";
                                    
                                std::cout << std::setw(8) << std::boolalpha << myMpsBlm[i]->getThresholdMinEn(blmThrCh);
                                std::cout << std::setw(8) << myMpsBlm[i]->getThresholdMin(blmThrCh);
                                std::cout << std::setw(8) << std::boolalpha << myMpsBlm[i]->getThresholdMaxEn(blmThrCh);
                                std::cout << std::setw(8) << myMpsBlm[i]->getThresholdMax(blmThrCh);
                                std::cout << std::endl;
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
            }

            std::cout << "====================================================" << std::endl;
            std::cout << std::endl;
        }

    }
    catch (CPSWError &e)
    {
        printf("CPSW error: %s not found!\n", e.getInfo().c_str());
        return 1;
    }

    return 0;
}
