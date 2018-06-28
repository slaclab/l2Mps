#include "l2Mps_thr.h"

IThrChannel::IThrChannel(Path mpsRoot, uint8_t channel)
:
    scaleFactor(1.0)
{
    if (channel > maxChannelCount)
    {
        std::cout << "        >   ERROR: channel" << unsigned(channel) << " exceeds the maximum channel number " << unsigned(maxChannelCount) << std::endl;
        std::cout << "        >   Aborting..." << std::endl;
        return;
    }

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
        std::cout << "Channel not implemented: " << e.getInfo() << std::endl;
        return;
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
    {
        thr_tableScalval_t tableScalVal;

        tableScalVal.minEn = CpswRegRW<uint8_t>(  chRoot, "lcls1Thr/minEn" );
        tableScalVal.maxEn = CpswRegRW<uint8_t>(  chRoot, "lcls1Thr/maxEn" );
        tableScalVal.min   = CpswRegRW<uint32_t>( chRoot, "lcls1Thr/min"   );
        tableScalVal.max   = CpswRegRW<uint32_t>( chRoot, "lcls1Thr/max"   );

        thrScalvals.data.insert( std::make_pair( thr_table_t{{0,0}}, tableScalVal) );
    }

    if ( thrChInfo.idleEn.first && thrChInfo.idleEn.second )
    {
        thr_tableScalval_t tableScalVal;

        tableScalVal.minEn = CpswRegRW<uint8_t>(  chRoot, "idleThr/minEn" );
        tableScalVal.maxEn = CpswRegRW<uint8_t>(  chRoot, "idleThr/maxEn" );
        tableScalVal.min   = CpswRegRW<uint32_t>( chRoot, "idleThr/min"   );
        tableScalVal.max   = CpswRegRW<uint32_t>( chRoot, "idleThr/max"   );

        thrScalvals.data.insert( std::make_pair( thr_table_t{{1,0}}, tableScalVal) );
    }

    std::stringstream regName;

    if ( thrChInfo.count.first )
    {
        for (int i=0; i<thrChInfo.count.second; ++i)
        {
            thr_tableScalval_t tableScalVal;

            regName.str("");
            regName << "stdThr[" << unsigned(i) << "]/";

            tableScalVal.minEn = CpswRegRW<uint8_t>(  chRoot, regName.str() + "minEn" );
            tableScalVal.maxEn = CpswRegRW<uint8_t>(  chRoot, regName.str() + "maxEn" );
            tableScalVal.min   = CpswRegRW<uint32_t>( chRoot, regName.str() + "min"   );
            tableScalVal.max   = CpswRegRW<uint32_t>( chRoot, regName.str() + "max"   );

            thrScalvals.data.insert( std::make_pair( thr_table_t{{2,i}}, tableScalVal) );
        }

        if ( thrChInfo.altEn.first && thrChInfo.altEn.second  )
        {
            for (int i=0; i<thrChInfo.count.second; ++i)
            {
                thr_tableScalval_t tableScalVal;

                regName.str("");
                regName << "altThr[" << unsigned(i) << "]/";

                tableScalVal.minEn = CpswRegRW<uint8_t>(  chRoot, regName.str() + "minEn" );
                tableScalVal.maxEn = CpswRegRW<uint8_t>(  chRoot, regName.str() + "maxEn" );
                tableScalVal.min   = CpswRegRW<uint32_t>( chRoot, regName.str() + "min"   );
                tableScalVal.max   = CpswRegRW<uint32_t>( chRoot, regName.str() + "max"   );

                thrScalvals.data.insert( std::make_pair( thr_table_t{{3,i}}, tableScalVal) );
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

// Read threshold register
std::pair<bool, float> IThrChannel::getThresholdMin(thr_table_t ch) const
{
    thr_chScalval_t::const_iterator it;
    it = thrScalvals.data.find(ch);
    if (it != thrScalvals.data.end())
    {
        std::pair<bool, uint32_t> tmp = (it->second).min.get();
        return std::make_pair( tmp.first, ( static_cast<int32_t>(tmp.second) ) * scaleFactor );
    }
    else
        throw std::runtime_error("Threshold not defined\n");
}

std::pair<bool, float> IThrChannel::getThresholdMax(thr_table_t ch) const
{
    thr_chScalval_t::const_iterator it;
    it = thrScalvals.data.find(ch);
    if (it != thrScalvals.data.end())
    {
        std::pair<bool, uint32_t> tmp = (it->second).max.get();
        return std::make_pair( tmp.first, ( static_cast<int32_t>(tmp.second) ) * scaleFactor );
    }
    else
        throw std::runtime_error("Threshold not defined\n");
}

// Read threshold enable register
std::pair<bool,bool> IThrChannel::getThresholdMinEn(thr_table_t ch) const
{
    thr_chScalval_t::const_iterator it;
    it = thrScalvals.data.find(ch);
    if (it != thrScalvals.data.end())
    {
        return (it->second).minEn.get();
    }
    else
        throw std::runtime_error("Threshold not defined\n");
}

std::pair<bool,bool> IThrChannel::getThresholdMaxEn(thr_table_t ch) const
{
    thr_chScalval_t::const_iterator it;
    it = thrScalvals.data.find(ch);
    if (it != thrScalvals.data.end())
    {
        return (it->second).maxEn.get();
    }
    else
        throw std::runtime_error("Threshold not defined\n");
}

// Write threshold registers
bool IThrChannel::setThresholdMin(thr_table_t ch, const float val) const
{
    thr_chScalval_t::const_iterator it;
    it = thrScalvals.data.find(ch);
    if (it != thrScalvals.data.end())
    {
        uint32_t scaledVal = (uint32_t)(val/scaleFactor);
        return (it->second).min.set(scaledVal);
    }
    else
        throw std::runtime_error("Threshold not defined\n");
}

bool IThrChannel::setThresholdMax(thr_table_t ch, const float val) const
{
    thr_chScalval_t::const_iterator it;
    it = thrScalvals.data.find(ch);
    if (it != thrScalvals.data.end())
    {
        uint32_t scaledVal = (uint32_t)(val/scaleFactor);
        return (it->second).max.set(scaledVal);
    }
    else
        throw std::runtime_error("Threshold not defined\n");
}

bool IThrChannel::setThresholdMinEn(thr_table_t ch, const bool val) const
{
    thr_chScalval_t::const_iterator it;
    it = thrScalvals.data.find(ch);
    if (it != thrScalvals.data.end())
    {
        return (it->second).minEn.set(val);
    }
    else
        throw std::runtime_error("Threshold not defined\n");
}

bool IThrChannel::setThresholdMaxEn(thr_table_t ch, const bool val) const
{
    thr_chScalval_t::const_iterator it;
    it = thrScalvals.data.find(ch);
    if (it != thrScalvals.data.end())
    {
        return (it->second).maxEn.set(val);
    }
    else
        throw std::runtime_error("Threshold not defined\n");
}

// Set the scale factor
void IThrChannel::setScaleFactor(const float sf)
{
    if (sf != 0)
        scaleFactor = sf;
}

// Get the scale factor
const float IThrChannel::getScaleFactor() const
{
    return scaleFactor;
}
