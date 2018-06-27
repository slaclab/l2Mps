#include <iomanip>
#include <yaml-cpp/yaml.h>
#include <arpa/inet.h>

#include "l2Mps_mps.h"
#include "l2Mps_bpm.h"

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
            if ( appType.compare("BPM") )
            {
                std::cout << "ERROR: This is not a BPM application. Aborting." << std::endl;
                return 1;
            }
        }

        std::array<MpsBpm, 2> myMpsBpm;

        for (std::size_t i {0}; i < 2; ++i)
        {
            std::cout << "BPM for AMC[" << i << "]: BPM[" << i << "]" << std::endl;
            std::cout << "====================================================" << std::endl;
            myMpsBpm[i] = MpsBpmFactory::create(mpsRoot, i);
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

                    std::cout << "    Thr channel =   " << myMpsBpm[i]->getChannel(bpmCh) << std::endl;
                    std::cout << "    Thr count =     " << myMpsBpm[i]->getThrCount(bpmCh) << std::endl;
                    std::cout << "    Byte map =      " << myMpsBpm[i]->getByteMap(bpmCh) << std::endl;
                    std::cout << "    Idle Enabled =  " << std::boolalpha << myMpsBpm[i]->getIdleEn(bpmCh) << std::endl;
                    std::cout << "    Lcls1 Enabled = " << std::boolalpha << myMpsBpm[i]->getLcls1En(bpmCh) << std::endl;
                    std::cout << "    Alt Enabled =   " << std::boolalpha << myMpsBpm[i]->getAltEn(bpmCh) << std::endl;

                    std::cout << std::endl;
                    std::cout << "    Threshold tables:" << std::endl;
                    std::cout << "    ==========================================" << std::endl;
                    std::cout << "    Table     " << "   minEn" << "     min" << "   maxEn" << "     max" << std::endl;
                    std::cout << "    ==========================================" << std::endl;

                    bpmThr_channel_t bpmThrCh;
                    bpmThrCh.appCh = bpmCh;
                    
                    try
                    {
                        bpmThrCh.thrTb = thr_table_t{{0,0}};
                        std::cout << "    LCLS1     ";
                        
                        std::cout << std::setw(8) << std::boolalpha << myMpsBpm[i]->getThresholdMinEn(bpmThrCh);
                        std::cout << std::setw(8) << myMpsBpm[i]->getThresholdMin(bpmThrCh);
                        std::cout << std::setw(8) << std::boolalpha << myMpsBpm[i]->getThresholdMaxEn(bpmThrCh);
                        std::cout << std::setw(8) << myMpsBpm[i]->getThresholdMax(bpmThrCh);
                        std::cout << std::endl;
                    }
                    catch (std::exception &e)
                    {
                        printf("   Error on LCLS 1 table section: %s", e.what());
                    }

                    try
                    {
                        bpmThrCh.thrTb = thr_table_t{{1,0}};
                        std::cout << "    IDLE      ";
                        
                        std::cout << std::setw(8) << std::boolalpha << myMpsBpm[i]->getThresholdMinEn(bpmThrCh);
                        std::cout << std::setw(8) << myMpsBpm[i]->getThresholdMin(bpmThrCh);
                        std::cout << std::setw(8) << std::boolalpha << myMpsBpm[i]->getThresholdMaxEn(bpmThrCh);
                        std::cout << std::setw(8) << myMpsBpm[i]->getThresholdMax(bpmThrCh);
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
                            bpmThrCh.thrTb = thr_table_t{{2,m}};
                            std::cout << "    STD[" << m << "]    ";
                            
                            std::cout << std::setw(8) << std::boolalpha << myMpsBpm[i]->getThresholdMinEn(bpmThrCh);
                            std::cout << std::setw(8) << myMpsBpm[i]->getThresholdMin(bpmThrCh);
                            std::cout << std::setw(8) << std::boolalpha << myMpsBpm[i]->getThresholdMaxEn(bpmThrCh);
                            std::cout << std::setw(8) << myMpsBpm[i]->getThresholdMax(bpmThrCh);
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
                    
                            bpmThrCh.thrTb = thr_table_t{{1,0}};
                            std::cout << "    ALT[" << m << "]    ";
                                
                            std::cout << std::setw(8) << std::boolalpha << myMpsBpm[i]->getThresholdMinEn(bpmThrCh);
                            std::cout << std::setw(8) << myMpsBpm[i]->getThresholdMin(bpmThrCh);
                            std::cout << std::setw(8) << std::boolalpha << myMpsBpm[i]->getThresholdMaxEn(bpmThrCh);
                            std::cout << std::setw(8) << myMpsBpm[i]->getThresholdMax(bpmThrCh);
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
