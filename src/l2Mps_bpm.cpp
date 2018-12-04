/**
 *-----------------------------------------------------------------------------
 * Title      : BPM application class
 * ----------------------------------------------------------------------------
 * File       : l2Mps_bpm.cpp
 * Author     : Jesus Vasquez, jvasquez@slac.stanford.edu
 * Created    : 2017-10-20
 * ----------------------------------------------------------------------------
 * Description:
 * Class for interfacing the BPM's MPS application.
 * ----------------------------------------------------------------------------
 * This file is part of l2Mps. It is subject to
 * the license terms in the LICENSE.txt file found in the top-level directory
 * of this distribution and at:
    * https://confluence.slac.stanford.edu/display/ppareg/LICENSE.html.
 * No part of l2Mps, including this file, may be
 * copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE.txt file.
 * ----------------------------------------------------------------------------
**/

#include "l2Mps_bpm.h"

IMpsBpm::IMpsBpm(Path mpsRoot, uint8_t amc)
    : IMpsAppBase<bpm_channel_t>(amc)
{

    for (int ch = 0 ; ch < maxChannelCount ; ++ch)
    {
        try
        {
            ThrChannel aThr(IThrChannel::create(mpsRoot, ch));

            if (( aThr->getThrCount().first) && (aThr->getThrCount().second) > 0)
            {
                for (int i = 0; i < numBpmChs; ++i)
                {
                    if ( ( aThr->getByteMap().first ) && ( aThr->getByteMap().second == bpmChByteMap[amc][i] ) )
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

MpsBpm IMpsBpm::create(Path mpsRoot, uint8_t amc)
{
    return boost::make_shared<IMpsBpm>(mpsRoot, amc);
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
