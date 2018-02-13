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

// Find ThrChannel in the Blm-ThrChannel map
ThrChannel IMpsBlm::findThrChannel(const blm_channel_t& blmCh) const
{
    blm_thrMap_t::const_iterator it = _blmThrMap.find(blmCh);

    if (it == _blmThrMap.end())
        throw std::runtime_error("Channel not defined\n");

    return it->second;
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
void IMpsBlm::pollThread()
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