#include "l2Mps_blen.h"

IMpsBlen::IMpsBlen(Path mpsRoot, const uint8_t amc) : _amc(amc)
{
    for (int ch = 0; ch < maxChannelCount; ++ch)
    {
        try
        {
            ThrChannel aThr(ThrChannelFactory::create(mpsRoot, ch));
            
            if (aThr->getThrCount())
            {
                for (int i = 0; i < numBlenChs; ++i)
                {
                    if (aThr->getByteMap() == blenChByteMap[amc][i])
                    {
                        _blenThrMap.insert( std::make_pair( i, aThr ) );
                        break;
                    }
                }
            }
        }
        catch (CPSWError &e)
        {
        }
    }

    std::cout << "    > A BLEN was created (AMC = " << unsigned(_amc) << ")" << std::endl;
    printChInfo();
}

IMpsBlen::~IMpsBlen()
{
    std::cout << "    > A BLEN was destroyed (AMC = " << unsigned(_amc) << ")" << std::endl;
}

// Find ThrChannel in the BLEN-ThrChannel map
ThrChannel IMpsBlen::findThrChannel(const blen_channel_t& blenCh) const
{
    blen_thrMap_t::const_iterator it = _blenThrMap.find(blenCh);

    if (it == _blenThrMap.end())
        throw std::runtime_error("Channel not defined\n");

    return it->second;
}

// Set polling thread with callback function
const void IMpsBlen::startPollThread(unsigned int poll, blen_cb_func_t callBack )
{
    if (poll == 0)
    {
        std::cout << "Error creating poll thread: poll time must be greater than 0" << std::endl;
        return;
    }
    _poll   = poll;
    _blenCB  = callBack;

    std::cout << "      Starting scan thread..." << std::endl;
    pthread_create(&_scanThread, NULL, createThread, this);
    std::cout << "      Scan thread created succesfully." << std::endl;
}

// Polling functions
void IMpsBlen::pollThread()
{
    while(1)
    {
        blen_dataMap_t dataMap;
        for (blen_thrMap_t::const_iterator it = _blenThrMap.begin() ; it != _blenThrMap.end(); ++it)
        {
            thr_ch_t data;
            (it->second)->readAll(data);

            dataMap.insert(std::make_pair(it->first, data));
        }

        _blenCB(_amc, dataMap);
        dataMap.clear();
        sleep(_poll);
    }
}

// Print BLEN channel information    
void IMpsBlen::printChInfo(void) const
{
    for (int i {0}; i < numBlenChs; ++i)
    {
        std::cout << "        Channel = " << i << ": Threshold channel = ";

        blen_thrMap_t::const_iterator it = _blenThrMap.find(i);

        if (it != _blenThrMap.end())
            std::cout << unsigned((it->second)->getChannel()) << std::endl;
        else
            std::cout << "Not implemented" << std::endl;
    }
}