#include "l2Mps_blm.h"

IMpsBlm::IMpsBlm(Path mpsRoot, const uint8_t amc) : _amc(amc)
{

    for (int ch = 0; ch < maxChannelCount; ++ch)
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
                            _blmThrMap.insert( std::make_pair( blm_channel_t{{i,j}}, aThr ) );
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
}

IMpsBlm::~IMpsBlm()
{
    std::cout << "    > A BLM was destroyed (AMC = " << unsigned(_amc) << ")" << std::endl;
}

// Set polling thread with callback function
const void IMpsBlm::startPollThread(unsigned int poll, p_func_t callBack )
{
    if (poll == 0)
    {
        std::cout << "Error creating poll thread: poll time must be greater than 0" << std::endl;
        return;
    }
    _poll   = poll;
    _blmCB  = callBack;

    std::cout << "      Starting scan thread..." << std::endl;
    pthread_create(&_scanThread, NULL, createThread, this);
    std::cout << "      Scan thread created succesfully." << std::endl;
}

// Polling functions
void IMpsBlm::scanTask()
{
    while(1)
    {
        blm_dataMap_t dataMap;
        for (blm_thrMap_t::const_iterator it = _blmThrMap.begin() ; it != _blmThrMap.end(); ++it)
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

// Threhold channel information
uint32_t const IMpsBlm::getCh(const blm_channel_t& ch) const
{
    blm_thrMap_t::const_iterator it = _blmThrMap.find(ch);

    if (it == _blmThrMap.end())
        throw std::runtime_error("Channel not defined\n");

    return (it->second)->getChannel(); 
}


bool const IMpsBlm::getIdleEn(const blm_channel_t& ch) const
{
    blm_thrMap_t::const_iterator it = _blmThrMap.find(ch);

    if (it == _blmThrMap.end())
        throw std::runtime_error("Channel not defined\n");

    return (it->second)->getIdleEn();    
}

bool const IMpsBlm::getAltEn(const blm_channel_t& ch) const
{
    blm_thrMap_t::const_iterator it = _blmThrMap.find(ch);

    if (it == _blmThrMap.end())
        throw std::runtime_error("Channel not defined\n");    

    return (it->second)->getAltEn();
}

bool const IMpsBlm::getLcls1En(const blm_channel_t& ch) const
{
    blm_thrMap_t::const_iterator it = _blmThrMap.find(ch);

    if (it == _blmThrMap.end())
        throw std::runtime_error("Channel not defined\n");    

    return (it->second)->getLcls1En();
}

uint32_t const IMpsBlm::getByteMap(const blm_channel_t& ch) const
{
    blm_thrMap_t::const_iterator it = _blmThrMap.find(ch);

    if (it == _blmThrMap.end())
        throw std::runtime_error("Channel not defined\n");    

    return (it->second)->getByteMap();
}

uint32_t const IMpsBlm::getThrCount(const blm_channel_t& ch) const
{
    blm_thrMap_t::const_iterator it = _blmThrMap.find(ch);

    if (it == _blmThrMap.end())
        throw std::runtime_error("Channel not defined\n");    

    return (it->second)->getThrCount();
}

// Threshold set enable methods
void IMpsBlm::setThresholdMinEn(const blmThr_channel_t& ch, const bool val) const
{
    blm_thrMap_t::const_iterator it = _blmThrMap.find(ch.blmCh);

    if (it == _blmThrMap.end())
        throw std::runtime_error("Channel not defined\n");

    (it->second)->setThresholdMinEn(ch.thrTb, val);
}

void IMpsBlm::setThresholdMaxEn(const blmThr_channel_t& ch, const bool val) const
{
    blm_thrMap_t::const_iterator it = _blmThrMap.find(ch.blmCh);

    if (it == _blmThrMap.end())
        throw std::runtime_error("Channel not defined\n");

    (it->second)->setThresholdMaxEn(ch.thrTb, val);
}

// Threshold get enable methods
const bool IMpsBlm::getThresholdMinEn(const blmThr_channel_t& ch) const
{
    blm_thrMap_t::const_iterator it = _blmThrMap.find(ch.blmCh);

    if (it == _blmThrMap.end())
        throw std::runtime_error("Channel not defined\n");

    return (it->second)->getThresholdMinEn(ch.thrTb);
}

const bool IMpsBlm::getThresholdMaxEn(const blmThr_channel_t& ch) const
{
    blm_thrMap_t::const_iterator it = _blmThrMap.find(ch.blmCh);

    if (it == _blmThrMap.end())
        throw std::runtime_error("Channel not defined\n");

    return (it->second)->getThresholdMaxEn(ch.thrTb);
}  

// Threshold get methods
const uint32_t IMpsBlm::getThresholdMin(const blmThr_channel_t& ch) const
{
    blm_thrMap_t::const_iterator it = _blmThrMap.find(ch.blmCh);

    if (it == _blmThrMap.end())
        throw std::runtime_error("Channel not defined\n");

    return (it->second)->getThresholdMin(ch.thrTb);
}
 
const uint32_t IMpsBlm::getThresholdMax(const blmThr_channel_t& ch) const
{
    blm_thrMap_t::const_iterator it = _blmThrMap.find(ch.blmCh);

    if (it == _blmThrMap.end())
        throw std::runtime_error("Channel not defined\n");

    return (it->second)->getThresholdMax(ch.thrTb);
}

// Threshold set methods
void IMpsBlm::setThresholdMin(const blmThr_channel_t& ch, const uint32_t val) const
{
    blm_thrMap_t::const_iterator it = _blmThrMap.find(ch.blmCh);

    if (it == _blmThrMap.end())
        throw std::runtime_error("Channel not defined\n");

     (it->second)->setThresholdMin(ch.thrTb, val);
}

void IMpsBlm::setThresholdMax(const blmThr_channel_t& ch, const uint32_t val) const
{
    blm_thrMap_t::const_iterator it = _blmThrMap.find(ch.blmCh);

    if (it == _blmThrMap.end())
        throw std::runtime_error("Channel not defined\n");

    (it->second)->setThresholdMax(ch.thrTb, val);
}

// Print BLM channel information    
void IMpsBlm::printChInfo(void) const
{
    for (int i {0}; i < numBlmChs; ++i)
    {
        std::cout << "        Channel = " << i << std::endl;
        for (int j {0}; j < numBlmIntChs; ++j)
        {
            std::cout << "            Integration channel = " << j << ": Threshold channel = ";

            blm_thrMap_t::const_iterator it = _blmThrMap.find(blm_channel_t{{i,j}});
            if (it != _blmThrMap.end())
                std::cout << unsigned((it->second)->getChannel()) << std::endl;
            else
                std::cout << "Not implemented" << std::endl;
        }
        std::cout << std::endl;
    }
}