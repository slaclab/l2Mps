#include "l2Mps_bcm.h"

IMpsBcm::IMpsBcm(Path mpsRoot, const uint8_t amc) : _amc(amc)
{
    for (int ch = 0; ch < maxChannelCount; ++ch)
    {
        try
        {
            ThrChannel aThr(ThrChannelFactory::create(mpsRoot, ch));
            
            if (aThr->getThrCount())
            {
                for (int i = 0; i < numBcmChs; ++i)
                {
                    if (aThr->getByteMap() == bcmChByteMap[amc][i])
                    {
                        _bcmThrMap.insert( std::make_pair( i, aThr ) );
                        break;
                    }
                }
            }
        }
        catch (CPSWError &e)
        {
        }
    }

    std::cout << "    > A BCM was created (AMC = " << unsigned(_amc) << ")" << std::endl;
    printChInfo();
}

IMpsBcm::~IMpsBcm()
{
    std::cout << "    > A BCM was destroyed (AMC = " << unsigned(_amc) << ")" << std::endl;
}

// Find ThrChannel in the BCM-ThrChannel map
ThrChannel IMpsBcm::findThrChannel(const bcm_channel_t& bcmCh) const
{
    bcm_thrMap_t::const_iterator it = _bcmThrMap.find(bcmCh);

    if (it == _bcmThrMap.end())
        throw std::runtime_error("Channel not defined\n");

    return it->second;
}

// Set polling thread with callback function
const void IMpsBcm::startPollThread(unsigned int poll, bcm_cb_func_t callBack )
{
    if (poll == 0)
    {
        std::cout << "Error creating poll thread: poll time must be greater than 0" << std::endl;
        return;
    }
    _poll   = poll;
    _bcmCB  = callBack;

    std::cout << "      Starting scan thread..." << std::endl;
    pthread_create(&_scanThread, NULL, createThread, this);
    std::cout << "      Scan thread created succesfully." << std::endl;
}

// Polling functions
void IMpsBcm::pollThread()
{
    while(1)
    {
        bcm_dataMap_t dataMap;
        for (bcm_thrMap_t::const_iterator it = _bcmThrMap.begin() ; it != _bcmThrMap.end(); ++it)
        {
            thr_ch_t data;
            (it->second)->readAll(data);

            dataMap.insert(std::make_pair(it->first, data));
        }

        _bcmCB(_amc, dataMap);
        dataMap.clear();
        sleep(_poll);
    }
}

// Print BCM channel information    
void IMpsBcm::printChInfo(void) const
{
    for (int i {0}; i < numBcmChs; ++i)
    {
        std::cout << "        Channel = " << i << ": Threshold channel = ";

        bcm_thrMap_t::const_iterator it = _bcmThrMap.find(i);

        if (it != _bcmThrMap.end())
            std::cout << unsigned((it->second)->getChannel()) << std::endl;
        else
            std::cout << "Not implemented" << std::endl;
    }
}