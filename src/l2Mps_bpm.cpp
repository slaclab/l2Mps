#include "l2Mps_bpm.h"

IMpsBpm::IMpsBpm(Path mpsRoot, uint8_t amc) : _amc(amc)
{

    for (int ch = 0 ; ch < maxChannelCount ; ++ch)
    {
        try
        {
            ThrChannel aThr(ThrChannelFactory::create(mpsRoot, ch));

            if (aThr->getThrCount())
            {
                for (int i = 0; i < numBpmChs; ++i)
                {
                    if (aThr->getByteMap() == bpmChByteMap[amc][i])
                    {
                        _bpmThrMap.insert( std::make_pair( i, aThr ) );
                        break;
                    }
                }
            }
        }
        catch (CPSWError &e)
        {
        }

    }

    std::cout << "    > A BPM was created (AMC = " << unsigned(_amc) << ")" << std::endl;
    printChInfo();
}

IMpsBpm::~IMpsBpm()
{
    std::cout << "    > A BPM was destroyed (AMC = " << unsigned(_amc) << ")" << std::endl;
}

// Find ThrChannel in the BPM-ThrChannel map
ThrChannel IMpsBpm::findThrChannel(const bpm_channel_t& bpmCh) const
{
    bpm_thrMap_t::const_iterator it = _bpmThrMap.find(bpmCh);

    if (it == _bpmThrMap.end())
        throw std::runtime_error("Channel not defined\n");

    return it->second;
}

// Set polling thread with callback function
const void IMpsBpm::startPollThread(unsigned int poll, bpm_cb_func_t callBack )
{
    if (poll == 0)
    {
        std::cout << "Error creating poll thread: poll time must be greater than 0" << std::endl;
        return;
    }
    _poll   = poll;
    _bpmCB  = callBack;

    std::cout << "      Starting scan thread..." << std::endl;
    pthread_create(&_scanThread, NULL, createThread, this);
    std::cout << "      Scan thread created succesfully." << std::endl;
}

// Polling functions
void IMpsBpm::pollThread()
{
    while(1)
    {
        bpm_dataMap_t dataMap;
        for (bpm_thrMap_t::const_iterator it = _bpmThrMap.begin() ; it != _bpmThrMap.end(); ++it)
        {
            thr_ch_t data;
            (it->second)->readAll(data);

            dataMap.insert(std::make_pair(it->first, data));
        }

        _bpmCB(_amc, dataMap);
        dataMap.clear();
        sleep(_poll);
    }
}

// Print BPM channel information    
void IMpsBpm::printChInfo(void) const
{
    for (int i {0}; i < numBpmChs; ++i)
    {
        std::cout << "        Channel = " << i << ": Threshold channel = ";

        bpm_thrMap_t::const_iterator it = _bpmThrMap.find(i);

        if (it != _bpmThrMap.end())
            std::cout << unsigned((it->second)->getChannel()) << std::endl;
        else
            std::cout << "Not implemented" << std::endl;
    }
}