#include <yaml-cpp/yaml.h>

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

template<typename T>
void printCell(std::pair<bool, T> p)
{
    std::cout << std::setw(10) << std::left;

    if (p.first)
        printVal(p.second);
    else
        std::cout << "[Invalid]";

   std::cout << std::right;
}

template<typename T, typename U>
void printThrRow(const std::string& name, const T& app, const U& thrCh)
{
    std::cout << "    " << std::left << std::setw(10) << name << std::right;
    printCell( app->getThresholdMinEn( thrCh ) );
    printCell( app->getThresholdMin(   thrCh ) );
    printCell( app->getThresholdMaxEn( thrCh ) );
    printCell( app->getThresholdMax(   thrCh ) );
    std::cout << std::endl;
}
//void printThrRow(const std::string& name, const MpsBlm& blm, const blmThr_channel_t& blmThrCh)
