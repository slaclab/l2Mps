#include "l2Mps_blm.h"

IMpsBlm::IMpsBlm(Path mpsRoot, const uint8_t amc, p_func_t blmCB) : _amc(amc), _poll(1), _blmCB(blmCB)
{

    for (std::size_t ch {0}; ch < maxChannelCount; ++ch)
    {
        try
        {
            ThrChannel aThr(ThrChannelFactory::create(mpsRoot, ch));

            if (aThr->getThrCount())
            {
                for (int i = 0 ; i < numBlmChs ; ++i)
                {
                    for (int j = 0 ; j < numBlmIntChs; ++j)
                    {
                        if (aThr->getByteMap() == blmChByteMap[_amc][i][j])        
                        {
                            _ch.insert( std::make_pair( std::make_pair(i, j), ch ) );
                            _blmThrMap.insert( std::make_pair( _blm_channel_t{{i,j}}, aThr ) );
                        }

                    }
                }
            }
        }
        catch (CPSWError &e)
        {
        }
    }
    std::cout << "    > A BLM was created (AMC = " << unsigned(_amc) << ")" << std::endl;
    printChInfo();

    std::cout << "      Starting scan thread..." << std::endl;

    pthread_create(&_scanThread, NULL, createThread, this);

    std::cout << "      Scan thread created succesfully." << std::endl;
}

IMpsBlm::~IMpsBlm()
{
    std::cout << "    > A BLM was destroyed (AMC = " << unsigned(_amc) << ")" << std::endl;
}

void IMpsBlm::scanTask()
{
    sleep(10);   
    while(1)
    {
        _blm_dataMap_t dataMap;
        for (_blm_thrMap_t::const_iterator it = _blmThrMap.begin() ; it != _blmThrMap.end(); ++it)
        {
            thr_ch_t data;
            (it->second)->readAll(data);

            dataMap.insert(std::make_pair(it->first, data));
        }

        _blmCB(_amc, dataMap);
        dataMap.clear();
        sleep(_poll);
    }
}

uint32_t const IMpsBlm::getCh(const blm_channel_t& ch) const
{
    _blm_thrMap_t::const_iterator it = _blmThrMap.find(ch.getBlenCh());

    if (it == _blmThrMap.end())
        throw std::runtime_error("Channel not defined\n");

    return (it->second)->getChannel(); 
}


bool const IMpsBlm::getIdleEn (const blm_channel& ch) const
{
    _blm_thrMap_t::const_iterator it = _blmThrMap.find(ch.getBlenCh());

    if (it == _blmThrMap.end())
        throw std::runtime_error("Channel not defined\n");

    return (it->second)->getIdleEn();    
}

bool const IMpsBlm::getAltEn (const blm_channel& ch) const
{
    _blm_thrMap_t::const_iterator it = _blmThrMap.find(ch.getBlenCh());

    if (it == _blmThrMap.end())
        throw std::runtime_error("Channel not defined\n");    

    return (it->second)->getAltEn();
}

bool const IMpsBlm::getLcls1En (const blm_channel& ch) const
{
    _blm_thrMap_t::const_iterator it = _blmThrMap.find(ch.getBlenCh());

    if (it == _blmThrMap.end())
        throw std::runtime_error("Channel not defined\n");    

    return (it->second)->getLcls1En();
}

uint32_t const IMpsBlm::getByteMap (const blm_channel& ch) const
{
    _blm_thrMap_t::const_iterator it = _blmThrMap.find(ch.getBlenCh());

    if (it == _blmThrMap.end())
        throw std::runtime_error("Channel not defined\n");    

    return (it->second)->getByteMap();
}

uint32_t const IMpsBlm::getThrCount (const blm_channel& ch) const
{
    _blm_thrMap_t::const_iterator it = _blmThrMap.find(ch.getBlenCh());

    if (it == _blmThrMap.end())
        throw std::runtime_error("Channel not defined\n");    

    return (it->second)->getThrCount();
}

void IMpsBlm::setThreshold(const blm_channel& ch, const uint32_t val) const
{
    _blm_thrMap_t::const_iterator it = _blmThrMap.find(ch.getBlenCh());

    if (it == _blmThrMap.end())
        throw std::runtime_error("Channel not defined\n");

    (it->second)->setThreshold(ch.getThrCh(), val);
}

const uint32_t IMpsBlm::getThreshold(const blm_channel& ch) const
{
    _blm_thrMap_t::const_iterator it = _blmThrMap.find(ch.getBlenCh());

    if (it == _blmThrMap.end())
        throw std::runtime_error("Channel not defined\n");

    return (it->second)->getThreshold(ch.getThrCh());
}
    
void IMpsBlm::setThresholdEn(const blm_channel& ch, const bool val) const
{
    _blm_thrMap_t::const_iterator it = _blmThrMap.find(ch.getBlenCh());

    if (it == _blmThrMap.end())
        throw std::runtime_error("Channel not defined\n");

    (it->second)->setThresholdEn(ch.getThrCh(), val);
}

const bool IMpsBlm::getThresholdEn(const blm_channel& ch) const
{
    _blm_thrMap_t::const_iterator it = _blmThrMap.find(ch.getBlenCh());

    if (it == _blmThrMap.end())
        throw std::runtime_error("Channel not defined\n");

    return (it->second)->getThresholdEn(ch.getThrCh());
}
    

void IMpsBlm::printChInfo(void) const
{
    for (std::size_t i {0}; i < numBlmChs; ++i)
    {
        std::cout << "        Channel = " << i << std::endl;
        for (std::size_t j {0}; j < numBlmIntChs; ++j)
        {
            std::cout << "            Integration channel = " << j << ": Threshold channel = ";

            std::map<std::pair<int, int>, int>::const_iterator it = _ch.find(std::make_pair(i,j));
            if (it != _ch.end())
                std::cout << it->second << std::endl;
            else
                std::cout << "Not implemented" << std::endl;
        }
        std::cout << std::endl;
    }
}
