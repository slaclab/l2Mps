#include "l2Mps_blm.h"

IMpsBlm::IMpsBlm(Path mpsRoot, const uint8_t amc)
    : IMpsAppBase<blm_channel_t>(amc)
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
                        if (aThr->getByteMap() == blmChByteMap[amc][i][j])
                        {
                            appThrMap.insert( std::make_pair( blm_channel_t{{i,j}}, aThr ) );
                        }

                    }
                }
            }
        }
        catch (CPSWError &e)
        {
        }
    }
    std::cout << "    > A BLM was created (AMC = " << unsigned(amc) << ")" << std::endl;
    printChInfo();
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

            blm_thrMap_t::const_iterator it = appThrMap.find(blm_channel_t{{i,j}});
            if (it != appThrMap.end())
                std::cout << unsigned((it->second)->getChannel()) << std::endl;
            else
                std::cout << "Not implemented" << std::endl;
        }
        std::cout << std::endl;
    }
}