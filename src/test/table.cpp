#include <iomanip>
#include <yaml-cpp/yaml.h>
#include <arpa/inet.h>

#include "l2Mps_base.h"
#include "l2Mps_mps.h"
#include "l2Mps_thr.h"

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
    }
    catch (CPSWError &e)
    {
        printf("CPSW error: %s not found!\n", e.getInfo().c_str());
    }


    try
    {
        MpsNode aMpsNode(MpsNodeFactory::create(mpsRoot));

        std::cout << std::endl;
        std::cout << "MPS application information:"<< std::endl;
        std::cout << "============================"<< std::endl;
        
        std::cout << "Application ID =                     ";
        try
        {
            uint16_t id = aMpsNode->getAppId();
            std::cout << "0x" << std::hex << unsigned(id) << std::dec << std::endl;
        }
        catch (CPSWError &e)
        {
            std::cout << "CPSW error: " << e.getInfo() << std::endl;
        }

        std::cout << "Is MPS enabled? =                    ";
        try
        {
            bool mpsEn = aMpsNode->getEnable();
            std::cout << std::boolalpha << mpsEn << std::endl;
        }
        catch (CPSWError &e)
        {
            std::cout << "CPSW error: " << e.getInfo() << std::endl;
        }

        std::cout << "Is LCLS1 Mode enabled? =             ";
        try
        {
            bool lcls1Mode = aMpsNode->getLcls1Mode();
            std::cout << std::boolalpha << lcls1Mode << std::endl;
        }
        catch (CPSWError &e)
        {
            std::cout << "CPSW error: " << e.getInfo() << std::endl;
        }

        std::cout << "Byte Count =                         ";
        try
        {
            uint8_t byteCount = aMpsNode->getByteCount();
            std::cout << unsigned(byteCount) << std::endl;
        }
        catch (CPSWError &e)
        {
            std::cout << "CPSW error: " << e.getInfo() << std::endl;
        }

        std::cout << "Does it generate digital messages? = ";
        try
        {
            bool digital = aMpsNode->getDigitalEnable();
            std::cout << std::boolalpha << digital << std::endl;
        }
        catch (CPSWError &e)
        {
            std::cout << "CPSW error: " << e.getInfo() << std::endl;
        }

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
                ThrChannel aThr(ThrChannelFactory::create(mpsRoot, ch));

                std::cout << std::setw(2) << unsigned(aThr->getByteMap()) << std::string(8, ' ');
                std::cout << std::setw(2) << unsigned(aThr->getThrCount()) << std::string(8, ' ');
                std::cout << std::boolalpha << std::setw(5) << aThr->getLcls1En() << std::string(5, ' ');
                std::cout << std::boolalpha << std::setw(5) << aThr->getIdleEn() << std::string(5, ' ');
                std::cout << std::boolalpha << std::setw(5) << aThr->getAltEn() << std::string(5, ' ');
                std::cout << std::endl;

            }
            catch (std::runtime_error &e)
            {
                std::cout << "Error while trying to read the threhold channel info: " << e.what() << std::endl;
            }
        }
        }
    catch (CPSWError &e)
    {
        std::cout << "Error creating the MPS application: " << e.getInfo() << std::endl;
        return 1;
    }

    return 0;
}
