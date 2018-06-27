#include "l2Mps_bcm.h"

IMpsBcm::IMpsBcm(Path mpsRoot, const uint8_t amc)
    : IMpsAppBase<bcm_channel_t>(amc)
{
    for (int ch = 0; ch < maxChannelCount; ++ch)
    {
        try
        {
            ThrChannel aThr(IThrChannel::create(mpsRoot, ch));

            if (aThr->getThrCount())
            {
                for (int i = 0; i < numBcmChs; ++i)
                {
                    if (aThr->getByteMap() == bcmChByteMap[amc][i])
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

    std::cout << "    > A BCM was created (AMC = " << unsigned(amc) << ")" << std::endl;
    printChInfo();
}

MpsBcm IMpsBcm::create(Path mpsRoot, uint8_t amc)
{
    return boost::make_shared<IMpsBcm>(mpsRoot, amc);
}

// Print BCM channel information
void IMpsBcm::printChInfo(void) const
{
    for (int i {0}; i < numBcmChs; ++i)
    {
        std::cout << "        Channel = " << i << ": Threshold channel = ";

        bcm_thrMap_t::const_iterator it = appThrMap.find(i);

        if (it != appThrMap.end())
            std::cout << unsigned((it->second)->getChannel()) << std::endl;
        else
            std::cout << "Not implemented" << std::endl;
    }
}
