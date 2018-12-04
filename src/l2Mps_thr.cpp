/**
 *-----------------------------------------------------------------------------
 * Title      : MPS threshold class
 * ----------------------------------------------------------------------------
 * File       : l2Mps_thr.cpp
 * Author     : Jesus Vasquez, jvasquez@slac.stanford.edu
 * Created    : 2017-10-20
 * ----------------------------------------------------------------------------
 * Description:
 * Class for interfacing the MPS thershold registers.
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

#include "l2Mps_thr.h"

IThrChannel::IThrChannel(Path mpsRoot, uint8_t channel)
:
    scaleFactor(1.0)
{
    if (channel >= maxChannelCount)
        throw std::runtime_error("Channel exceeds the maximum channel number");

    ch = channel;

    // Find MPS application root
    std::stringstream chStr;
    chStr.str("");
    chStr << MpsThrModuleName << "/channel[" << unsigned(ch) << "]";

    try
    {
        chRoot = mpsRoot->findByName(chStr.str().c_str());
    }
    catch (CPSWError &e)
    {
        throw std::runtime_error("Channel not implemented");
    }

    // Get Channel header information register
    thrScalvals.info.count   = CpswRegRO<uint8_t>( chRoot, "thresholdCount" );
    thrScalvals.info.idleEn  = CpswRegRW<uint8_t>( chRoot, "idleEn"         );
    thrScalvals.info.altEn   = CpswRegRO<uint8_t>( chRoot, "altEn"          );
    thrScalvals.info.lcls1En = CpswRegRO<uint8_t>( chRoot, "lcls1En"        );
    thrScalvals.info.byteMap = CpswRegRO<uint8_t>( chRoot, "byteMap"        );

    thr_chInfoData_t thrChInfo;
    readThrChInfo(thrChInfo);

    if (thrChInfo.lcls1En.first && thrChInfo.lcls1En.second)
        thrScalvals.data.insert( std::make_pair( thr_table_t{{0,0}}, createTableScalVal("lcls1Thr/") ) );

    if ( thrChInfo.idleEn.first && thrChInfo.idleEn.second )
        thrScalvals.data.insert( std::make_pair( thr_table_t{{1,0}}, createTableScalVal("idleThr/") ) );

    if ( thrChInfo.count.first )
    {
        std::stringstream tablePrefix;

        for (int i=0; i<thrChInfo.count.second; ++i)
        {
            tablePrefix.str("");
            tablePrefix << "stdThr[" << unsigned(i) << "]/";
            thrScalvals.data.insert( std::make_pair( thr_table_t{{2,i}}, createTableScalVal(tablePrefix.str()) ) );
        }

        if ( thrChInfo.altEn.first && thrChInfo.altEn.second  )
        {
            for (int i=0; i<thrChInfo.count.second; ++i)
            {
                tablePrefix.str("");
                tablePrefix << "altThr[" << unsigned(i) << "]/";
                thrScalvals.data.insert( std::make_pair( thr_table_t{{3,i}}, createTableScalVal(tablePrefix.str()) ) );
            }
        }
    }
}

ThrChannel IThrChannel::create(Path mpsRoot, uint8_t channel)
{
    return boost::make_shared<IThrChannel>(mpsRoot, channel);
}

void IThrChannel::readThrChInfo(thr_chInfoData_t& info) const
{
    info.count       = thrScalvals.info.count.get();
    info.byteMap     = thrScalvals.info.byteMap.get();
    info.idleEn      = thrScalvals.info.idleEn.get();
    info.lcls1En     = thrScalvals.info.lcls1En.get();
    info.altEn       = thrScalvals.info.altEn.get();
    info.ch          = ch;
    info.scaleFactor = scaleFactor;
}

void IThrChannel::readThrChData(thr_chData_t& data) const
{
    for (thr_chScalval_t::const_iterator it = thrScalvals.data.begin(); it != thrScalvals.data.end(); ++it)
    {
        thr_tableData_t tableData;

        tableData.minEn = (it->second).minEn.get();
        tableData.maxEn = (it->second).maxEn.get();

        std::pair<bool,uint32_t> tmp ( (it->second).min.get() );
        tableData.min   = std::make_pair( tmp.first, static_cast<int32_t>(tmp.second) * scaleFactor );

	tmp = (it->second).max.get();
        tableData.max   = std::make_pair( tmp.first, static_cast<int32_t>(tmp.second) * scaleFactor );

        data.insert( std::make_pair(it->first, tableData) );
    }
}

void IThrChannel::readAll(thr_ch_t& data) const
{
    readThrChInfo(data.info);
    readThrChData(data.data);
}

// Read threshold registers
std::pair<bool, float> IThrChannel::getThresholdMin(thr_table_t ch)
{
    std::pair<bool, uint32_t> tmp = findDataTableScalval(ch)->min.get();
    return std::make_pair( tmp.first, ( static_cast<int32_t>(tmp.second) ) * scaleFactor );
}

std::pair<bool, float> IThrChannel::getThresholdMax(thr_table_t ch)
{
    std::pair<bool, uint32_t> tmp = findDataTableScalval(ch)->max.get();
    return std::make_pair( tmp.first, ( static_cast<int32_t>(tmp.second) ) * scaleFactor );
}

// Read threshold enable registers
std::pair<bool,bool> IThrChannel::getThresholdMinEn(thr_table_t ch)
{
    return findDataTableScalval(ch)->minEn.get();
}

std::pair<bool,bool> IThrChannel::getThresholdMaxEn(thr_table_t ch)
{
    return findDataTableScalval(ch)->maxEn.get();
}

// Write threshold registers
bool IThrChannel::setThresholdMin(thr_table_t ch, const float val)
{
    uint32_t scaledVal = static_cast<uint32_t>(val/scaleFactor);
    return findDataTableScalval(ch)->min.set(scaledVal);
}

bool IThrChannel::setThresholdMax(thr_table_t ch, const float val)
{
    uint32_t scaledVal = static_cast<uint32_t>(val/scaleFactor);
    return findDataTableScalval(ch)->max.set(scaledVal);
}

// Write threshold enable registers
bool IThrChannel::setThresholdMinEn(thr_table_t ch, const bool val)
{
    return findDataTableScalval(ch)->minEn.set(val);
}

bool IThrChannel::setThresholdMaxEn(thr_table_t ch, const bool val)
{
    return findDataTableScalval(ch)->maxEn.set(val);
}

// Set the scale factor
bool IThrChannel::setScaleFactor(const float sf)
{
    if (sf == 0)
        return false;

    scaleFactor = sf;
    return true;
}

// Get the scale factor
const float IThrChannel::getScaleFactor() const
{
    return scaleFactor;
}

// Helper functions definitions
thr_tableScalval_t IThrChannel::createTableScalVal(const std::string& prefix) const
{
    thr_tableScalval_t tableScalVal;

    tableScalVal.minEn = CpswRegRW<uint8_t>(  chRoot, prefix + "minEn" );
    tableScalVal.maxEn = CpswRegRW<uint8_t>(  chRoot, prefix + "maxEn" );
    tableScalVal.min   = CpswRegRW<uint32_t>( chRoot, prefix + "min"   );
    tableScalVal.max   = CpswRegRW<uint32_t>( chRoot, prefix + "max"   );

    return tableScalVal;
}

thr_tableScalval_t* IThrChannel::findDataTableScalval(const thr_table_t& ch)
{
    thr_chScalval_t::iterator it;
    it = thrScalvals.data.find(ch);
    if (it != thrScalvals.data.end())
    {
        return &(it->second);
    }
    else
        throw std::runtime_error("Threshold not defined\n");
}

