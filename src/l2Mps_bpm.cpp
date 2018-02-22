#include "l2Mps_bpm.h"

IMpsBpm::IMpsBpm(Path mpsRoot, uint8_t amc)
    : IMpsAppBase<bpm_channel_t>(amc)
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
                        appThrMap.insert( std::make_pair( i, aThr ) );
                        break;
                    }
                }
            }
        }
        catch (CPSWError &e)
        {
        }

    }

    std::cout << "    > A BPM was created (AMC = " << unsigned(amc) << ")" << std::endl;
    printChInfo();
}

IMpsBpm::~IMpsBpm()
{
    std::cout << "    > A BPM was destroyed (AMC = " << unsigned(amc) << ")" << std::endl;
}

// Print BPM channel information
void IMpsBpm::printChInfo(void) const
{
    for (int i {0}; i < numBpmChs; ++i)
    {
        std::cout << "        Channel = " << i << ": Threshold channel = ";

        bpm_thrMap_t::const_iterator it = appThrMap.find(i);

        if (it != appThrMap.end())
            std::cout << unsigned((it->second)->getChannel()) << std::endl;
        else
            std::cout << "Not implemented" << std::endl;
    }
}