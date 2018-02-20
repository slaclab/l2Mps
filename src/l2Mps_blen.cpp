#include "l2Mps_blen.h"

IMpsBlen::IMpsBlen(Path mpsRoot, const uint8_t amc)
    : IMpsAppBase<blen_channel_t>(amc)
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
                        _appThrMap.insert( std::make_pair( i, aThr ) );
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

// Print BLEN channel information
void IMpsBlen::printChInfo(void) const
{
    for (int i {0}; i < numBlenChs; ++i)
    {
        std::cout << "        Channel = " << i << ": Threshold channel = ";

        blen_thrMap_t::const_iterator it = _appThrMap.find(i);

        if (it != _appThrMap.end())
            std::cout << unsigned((it->second)->getChannel()) << std::endl;
        else
            std::cout << "Not implemented" << std::endl;
    }
}