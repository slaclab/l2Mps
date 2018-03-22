#include "l2Mps_thr.h"

IThrChannel::IThrChannel(Path mpsRoot, uint8_t channel) : scaleFactor(1.0)
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
    thrScalvals.info.count   = IMpsBase::createInterface<ScalVal_RO>( chRoot, "thresholdCount" );
    thrScalvals.info.idleEn  = IMpsBase::createInterface<ScalVal>(    chRoot, "idleEn"         );
    thrScalvals.info.altEn   = IMpsBase::createInterface<ScalVal_RO>( chRoot, "altEn"          );
    thrScalvals.info.lcls1En = IMpsBase::createInterface<ScalVal_RO>( chRoot, "lcls1En"        );
    thrScalvals.info.byteMap = IMpsBase::createInterface<ScalVal_RO>( chRoot, "byteMap"        );

    thr_chInfoData_t thrChInfo;
    readThrChInfo(thrChInfo);

    if (thrChInfo.lcls1En)
    {
        thr_tableScalval_t tableScalVal;

        tableScalVal.minEn = IMpsBase::createInterface<ScalVal>( chRoot, "lcls1Thr/minEn" );
        tableScalVal.maxEn = IMpsBase::createInterface<ScalVal>( chRoot, "lcls1Thr/maxEn" );
        tableScalVal.min   = IMpsBase::createInterface<ScalVal>( chRoot, "lcls1Thr/min" );
        tableScalVal.max   = IMpsBase::createInterface<ScalVal>( chRoot, "lcls1Thr/max" );

        thrScalvals.data.insert( std::make_pair( thr_table_t{{0,0}}, tableScalVal) );
    }

    if (thrChInfo.idleEn)
    {
        thr_tableScalval_t tableScalVal;

        tableScalVal.minEn = IMpsBase::createInterface<ScalVal>( chRoot, "idleThr/minEn" );
        tableScalVal.maxEn = IMpsBase::createInterface<ScalVal>( chRoot, "idleThr/maxEn" );
        tableScalVal.min   = IMpsBase::createInterface<ScalVal>( chRoot, "idleThr/min" );
        tableScalVal.max   = IMpsBase::createInterface<ScalVal>( chRoot, "idleThr/max" );

        thrScalvals.data.insert( std::make_pair( thr_table_t{{1,0}}, tableScalVal) );
    }

    std::stringstream regName;

    for (int i=0; i<thrChInfo.count; ++i)
    {
        thr_tableScalval_t tableScalVal;

        regName.str("");
        regName << "stdThr[" << unsigned(i) << "]/";

        tableScalVal.minEn = IMpsBase::createInterface<ScalVal>( chRoot, regName.str() + "minEn" );
        tableScalVal.maxEn = IMpsBase::createInterface<ScalVal>( chRoot, regName.str() + "maxEn" );
        tableScalVal.min   = IMpsBase::createInterface<ScalVal>( chRoot, regName.str() + "min" );
        tableScalVal.max   = IMpsBase::createInterface<ScalVal>( chRoot, regName.str() + "max" );

        thrScalvals.data.insert( std::make_pair( thr_table_t{{2,i}}, tableScalVal) );
    }

    if (thrChInfo.altEn)
    {
        for (int i=0; i<thrChInfo.count; ++i)
        {
            thr_tableScalval_t tableScalVal;

            regName.str("");
            regName << "altThr[" << unsigned(i) << "]/";

            tableScalVal.minEn = IMpsBase::createInterface<ScalVal>( chRoot, regName.str() + "minEn" );
            tableScalVal.maxEn = IMpsBase::createInterface<ScalVal>( chRoot, regName.str() + "maxEn" );
            tableScalVal.min   = IMpsBase::createInterface<ScalVal>( chRoot, regName.str() + "min" );
            tableScalVal.max   = IMpsBase::createInterface<ScalVal>( chRoot, regName.str() + "max" );

            thrScalvals.data.insert( std::make_pair( thr_table_t{{3,i}}, tableScalVal) );
        }
    }

}

IThrChannel::~IThrChannel()
{
}

void IThrChannel::readThrChInfo(thr_chInfoData_t& info) const
{
    uint32_t u32;

    thrScalvals.info.count->getVal(&u32);
    info.count = u32;

    thrScalvals.info.byteMap->getVal(&u32);
    info.byteMap = u32;

    thrScalvals.info.idleEn->getVal(&u32);
    info.idleEn = (u32?true:false);

    thrScalvals.info.lcls1En->getVal(&u32);
    info.lcls1En = (u32?true:false);

    thrScalvals.info.altEn->getVal(&u32);
    info.altEn = (u32?true:false);

    info.ch = ch;
    info.scaleFactor = scaleFactor;
}

void IThrChannel::readThrChData(thr_chData_t& data) const
{
    for (thr_chScalval_t::const_iterator it = thrScalvals.data.begin(); it != thrScalvals.data.end(); ++it)
    {
        thr_tableData_t tableData;

        uint32_t reg;

        (it->second).minEn->getVal(&reg);
        tableData.minEn = reg?true:false;

        (it->second).maxEn->getVal(&reg);
        tableData.maxEn = reg?true:false;

        (it->second).min->getVal(&reg);
        tableData.min = ((int32_t)reg)*scaleFactor;

        (it->second).max->getVal(&reg);
        tableData.max = ((int32_t)reg)*scaleFactor;

        data.insert( std::make_pair(it->first, tableData) );
    }
}

void IThrChannel::readAll(thr_ch_t& data) const
{
    readThrChInfo(data.info);
    readThrChData(data.data);
}

uint8_t IThrChannel::getThrCount() const
{
    if (!thrScalvals.info.count)
        throw std::runtime_error("Register interface not implemented\n");

    uint8_t reg;
    thrScalvals.info.count->getVal(&reg);
    return reg;
}

bool IThrChannel::getIdleEn() const
{
    if (!thrScalvals.info.idleEn)
        throw std::runtime_error("Register interface not implemented\n");

    uint8_t reg;
    thrScalvals.info.idleEn->getVal(&reg);
    return (reg & 0x01)?true:false;
}

void IThrChannel::setIdleEn(const bool en) const
{
    if (!thrScalvals.info.idleEn)
        throw std::runtime_error("Register interface not implemented\n");

    thrScalvals.info.idleEn->setVal(en);
}

bool IThrChannel::getAltEn() const
{
    if (!thrScalvals.info.altEn)
        throw std::runtime_error("Register interface not implemented\n");

    uint8_t reg;
    thrScalvals.info.altEn->getVal(&reg);
    return (reg & 0x01)?true:false;

}

bool IThrChannel::getLcls1En() const
{
    if (!thrScalvals.info.lcls1En)
        throw std::runtime_error("Register interface not implemented\n");

    uint8_t reg;
    thrScalvals.info.lcls1En->getVal(&reg);
    return (reg & 0x01)?true:false;
}

uint8_t IThrChannel::getByteMap() const
{
    if (!thrScalvals.info.byteMap)
        throw std::runtime_error("Register interface not implemented\n");

    uint8_t reg;
    thrScalvals.info.byteMap->getVal(&reg);
    return reg;
}

// Read threshold register
const float IThrChannel::getThresholdMin(thr_table_t ch) const
{
    thr_chScalval_t::const_iterator it;
    it = thrScalvals.data.find(ch);
    if (it != thrScalvals.data.end())
    {
        uint32_t reg;
        (it->second).min->getVal(&reg, 1);
        return ((int32_t)reg) * scaleFactor;
    }
    else
        throw std::runtime_error("Threshold not defined\n");
}

const float IThrChannel::getThresholdMax(thr_table_t ch) const
{
    thr_chScalval_t::const_iterator it;
    it = thrScalvals.data.find(ch);
    if (it != thrScalvals.data.end())
    {
        uint32_t reg;
        (it->second).max->getVal(&reg, 1);
        return ((int32_t)reg) * scaleFactor;
    }
    else
        throw std::runtime_error("Threshold not defined\n");
}

// Read threshold enable register
const bool IThrChannel::getThresholdMinEn(thr_table_t ch) const
{
    thr_chScalval_t::const_iterator it;
    it = thrScalvals.data.find(ch);
    if (it != thrScalvals.data.end())
    {
        uint32_t reg;
        (it->second).minEn->getVal(&reg, 1);
        return reg?true:false;
    }
    else
        throw std::runtime_error("Threshold not defined\n");
}

const bool IThrChannel::getThresholdMaxEn(thr_table_t ch) const
{
    thr_chScalval_t::const_iterator it;
    it = thrScalvals.data.find(ch);
    if (it != thrScalvals.data.end())
    {
        uint32_t reg;
        (it->second).maxEn->getVal(&reg, 1);
        return reg?true:false;
    }
    else
        throw std::runtime_error("Threshold not defined\n");
}

// Write threshold registers
void IThrChannel::setThresholdMin(thr_table_t ch, const float val) const
{
    thr_chScalval_t::const_iterator it;
    it = thrScalvals.data.find(ch);
    if (it != thrScalvals.data.end())
    {
        uint32_t scaledVal = (uint32_t)(val/scaleFactor);
        (it->second).min->setVal(scaledVal);
    }
    else
        throw std::runtime_error("Threshold not defined\n");
}

void IThrChannel::setThresholdMax(thr_table_t ch, const float val) const
{
    thr_chScalval_t::const_iterator it;
    it = thrScalvals.data.find(ch);
    if (it != thrScalvals.data.end())
    {
        uint32_t scaledVal = (uint32_t)(val/scaleFactor);
        (it->second).max->setVal(scaledVal);
    }
    else
        throw std::runtime_error("Threshold not defined\n");
}

void IThrChannel::setThresholdMinEn(thr_table_t ch, const bool val) const
{
    thr_chScalval_t::const_iterator it;
    it = thrScalvals.data.find(ch);
    if (it != thrScalvals.data.end())
    {
        (it->second).minEn->setVal(val);
    }
    else
        throw std::runtime_error("Threshold not defined\n");
}

void IThrChannel::setThresholdMaxEn(thr_table_t ch, const bool val) const
{
    thr_chScalval_t::const_iterator it;
    it = thrScalvals.data.find(ch);
    if (it != thrScalvals.data.end())
    {
        (it->second).maxEn->setVal(val);
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
