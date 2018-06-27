#include <iomanip>
#include <yaml-cpp/yaml.h>
#include <arpa/inet.h>

#include "l2Mps_mps.h"
#include "l2Mps_blen.h"

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
            MpsNode mpsNode = IMpsNode::create(mpsRoot);

            std::string appType(mpsNode->getAppType().second);
            std::cout << "This application type is " << appType << std::endl;
            if ( appType.compare("BLEN") )
            {
                std::cout << "ERROR: This is not a BLEN application. Aborting." << std::endl;
                return 1;
            }
        }

        std::array<MpsBlen, 2> myMpsBlen;

        for (std::size_t i {0}; i < 2; ++i)
        {
            std::cout << "BLEN for AMC[" << i << "]: BLEN[" << i << "]" << std::endl;
            std::cout << "====================================================" << std::endl;
            myMpsBlen[i] = IMpsBlen::create(mpsRoot, i);
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

                    std::cout << "    Thr channel =   " << myMpsBlen[i]->getChannel(blenCh) << std::endl;
                    std::cout << "    Thr count =     " << myMpsBlen[i]->getThrCount(blenCh) << std::endl;
                    std::cout << "    Byte map =      " << myMpsBlen[i]->getByteMap(blenCh) << std::endl;
                    std::cout << "    Idle Enabled =  " << std::boolalpha << myMpsBlen[i]->getIdleEn(blenCh) << std::endl;
                    std::cout << "    Lcls1 Enabled = " << std::boolalpha << myMpsBlen[i]->getLcls1En(blenCh) << std::endl;
                    std::cout << "    Alt Enabled =   " << std::boolalpha << myMpsBlen[i]->getAltEn(blenCh) << std::endl;

                    std::cout << std::endl;
                    std::cout << "    Threshold tables:" << std::endl;
                    std::cout << "    ==========================================" << std::endl;
                    std::cout << "    Table     " << "   minEn" << "     min" << "   maxEn" << "     max" << std::endl;
                    std::cout << "    ==========================================" << std::endl;

                    blenThr_channel_t blenThrCh;
                    blenThrCh.appCh = blenCh;
                    
                    try
                    {
                        blenThrCh.thrTb = thr_table_t{{0,0}};
                        std::cout << "    LCLS1     ";
                        
                        std::cout << std::setw(8) << std::boolalpha << myMpsBlen[i]->getThresholdMinEn(blenThrCh);
                        std::cout << std::setw(8) << myMpsBlen[i]->getThresholdMin(blenThrCh);
                        std::cout << std::setw(8) << std::boolalpha << myMpsBlen[i]->getThresholdMaxEn(blenThrCh);
                        std::cout << std::setw(8) << myMpsBlen[i]->getThresholdMax(blenThrCh);
                        std::cout << std::endl;
                    }
                    catch (std::exception &e)
                    {
                        printf("   Error on LCLS 1 table section: %s", e.what());
                    }

                    try
                    {
                        blenThrCh.thrTb = thr_table_t{{1,0}};
                        std::cout << "    IDLE      ";
                        
                        std::cout << std::setw(8) << std::boolalpha << myMpsBlen[i]->getThresholdMinEn(blenThrCh);
                        std::cout << std::setw(8) << myMpsBlen[i]->getThresholdMin(blenThrCh);
                        std::cout << std::setw(8) << std::boolalpha << myMpsBlen[i]->getThresholdMaxEn(blenThrCh);
                        std::cout << std::setw(8) << myMpsBlen[i]->getThresholdMax(blenThrCh);
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
                            blenThrCh.thrTb = thr_table_t{{2,m}};
                            std::cout << "    STD[" << m << "]    ";
                            
                            std::cout << std::setw(8) << std::boolalpha << myMpsBlen[i]->getThresholdMinEn(blenThrCh);
                            std::cout << std::setw(8) << myMpsBlen[i]->getThresholdMin(blenThrCh);
                            std::cout << std::setw(8) << std::boolalpha << myMpsBlen[i]->getThresholdMaxEn(blenThrCh);
                            std::cout << std::setw(8) << myMpsBlen[i]->getThresholdMax(blenThrCh);
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
                    
                            blenThrCh.thrTb = thr_table_t{{1,0}};
                            std::cout << "    ALT[" << m << "]    ";
                                
                            std::cout << std::setw(8) << std::boolalpha << myMpsBlen[i]->getThresholdMinEn(blenThrCh);
                            std::cout << std::setw(8) << myMpsBlen[i]->getThresholdMin(blenThrCh);
                            std::cout << std::setw(8) << std::boolalpha << myMpsBlen[i]->getThresholdMaxEn(blenThrCh);
                            std::cout << std::setw(8) << myMpsBlen[i]->getThresholdMax(blenThrCh);
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
