#include "l2Mps_thr.h"

IThrChannel::IThrChannel(Path mpsRoot, uint8_t channel)
{
    if (channel > maxChannelCount)
    {
        std::cout << "        >   ERROR: channel" << unsigned(channel) << " exceeds the maximum channel number " << unsigned(maxChannelCount) << std::endl;
        std::cout << "        >   Using 0 instead." << std::endl;
        _ch = 0;
    }
    else
    {
        _ch = channel;
    }

    // Find MPS application root
    std::stringstream chStr;
    chStr.str("");
    chStr << MpsThrModuleName << "/channel[" << unsigned(_ch) << "]";

    try
    {
        _chRoot = mpsRoot->findByName(chStr.str().c_str());
    }
    catch (CPSWError &e)
    {
        std::cout << "Channel not implemented: " << e.getInfo() << std::endl;
        return;
    }

    // Get Channel header information register
    _thrCount = IMpsBase::createInterface<ScalVal_RO>( _chRoot, "thresholdCount" );
    _idleEn   = IMpsBase::createInterface<ScalVal>(    _chRoot, "idleEn"         );
    _altEn    = IMpsBase::createInterface<ScalVal_RO>( _chRoot, "altEn"          );
    _lcls1En  = IMpsBase::createInterface<ScalVal_RO>( _chRoot, "lcls1En"        );
    _byteMap  = IMpsBase::createInterface<ScalVal_RO>( _chRoot, "byteMap"        );

    thr_chInfo_t thrChInfo;
    readThrChInfo(thrChInfo);
    
    if (thrChInfo.lcls1En)
    {
        _thrScalvalMap.insert( std::make_pair( thr_table_t{{0,0,0}}, std::make_pair(IMpsBase::createInterface<ScalVal>( _chRoot, "lcls1Thr/min" ), IMpsBase::createInterface<ScalVal>( _chRoot, "lcls1Thr/minEn" ))));
        _thrScalvalMap.insert( std::make_pair( thr_table_t{{0,1,0}}, std::make_pair(IMpsBase::createInterface<ScalVal>( _chRoot, "lcls1Thr/max" ), IMpsBase::createInterface<ScalVal>( _chRoot, "lcls1Thr/maxEn" ))));   
    }

    if (thrChInfo.idleEn)
    {
        _thrScalvalMap.insert( std::make_pair( thr_table_t{{1,0,0}}, std::make_pair(IMpsBase::createInterface<ScalVal>( _chRoot, "idleThr/min" ), IMpsBase::createInterface<ScalVal>( _chRoot, "idleThr/minEn" ))));
        _thrScalvalMap.insert( std::make_pair( thr_table_t{{1,1,0}}, std::make_pair(IMpsBase::createInterface<ScalVal>( _chRoot, "idleThr/max" ), IMpsBase::createInterface<ScalVal>( _chRoot, "idleThr/maxEn" ))));
    }

    std::stringstream regName;

    for (int i=0; i<thrChInfo.count; ++i)
    {
        regName.str("");
        regName << "stdThr[" << unsigned(i) << "]/";
        _thrScalvalMap.insert( std::make_pair( thr_table_t{{2,0,i}}, std::make_pair(IMpsBase::createInterface<ScalVal>( _chRoot, regName.str() + "min" ), IMpsBase::createInterface<ScalVal>( _chRoot, regName.str() + "minEn" ))));
        _thrScalvalMap.insert( std::make_pair( thr_table_t{{2,1,i}}, std::make_pair(IMpsBase::createInterface<ScalVal>( _chRoot, regName.str() + "max" ), IMpsBase::createInterface<ScalVal>( _chRoot, regName.str() + "maxEn" ))));
    }

    if (thrChInfo.altEn)
    {
        for (int i=0; i<thrChInfo.count; ++i)
        {
            regName.str("");
            regName << "altThr[" << unsigned(i) << "]/";
            _thrScalvalMap.insert( std::make_pair( thr_table_t{{3,0,i}}, std::make_pair(IMpsBase::createInterface<ScalVal>( _chRoot, regName.str() + "min" ), IMpsBase::createInterface<ScalVal>( _chRoot, regName.str() + "minEn" ))));
            _thrScalvalMap.insert( std::make_pair( thr_table_t{{3,1,i}}, std::make_pair(IMpsBase::createInterface<ScalVal>( _chRoot, regName.str() + "max" ), IMpsBase::createInterface<ScalVal>( _chRoot, regName.str() + "maxEn" ))));
        }
    }

}

IThrChannel::~IThrChannel()
{
}


void IThrChannel::readThrChInfo(thr_chInfo_t& info) const
{
    uint32_t u32;
    
    info.ch = _ch;

    _thrCount->getVal(&u32);
    info.count = u32;
    
    _byteMap->getVal(&u32);
    info.byteMap = u32;

    _idleEn->getVal(&u32);
    info.idleEn = (u32?true:false);

    _lcls1En->getVal(&u32);
    info.lcls1En = (u32?true:false);

    _altEn->getVal(&u32);
    info.altEn = (u32?true:false);
}

void IThrChannel::readThrChData(thr_chData_t& data) const
{
    for (thr_chScalval_t::const_iterator it = _thrScalvalMap.begin(); it != _thrScalvalMap.end(); ++it)
    {
        uint32_t thrVal;
        uint8_t  thrEn;
        std::get<0>(it->second)->getVal(&thrVal);
        std::get<1>(it->second)->getVal(&thrEn);
        thr_tableData_t tableData = std::make_pair( thrVal, thrEn?true:false );
        data.insert(std::make_pair( it->first, tableData ));
    }
}

void IThrChannel::readAll(thr_ch_t& data) const
{
    readThrChInfo(data.info);
    readThrChData(data.data);
}

uint8_t IThrChannel::getThrCount() const
{
    if (!_thrCount)
        throw std::runtime_error("Register interface not implemented\n");

    uint8_t reg;
    _thrCount->getVal(&reg);
    return reg;
}

bool IThrChannel::getIdleEn() const
{
    if (!_idleEn)
        throw std::runtime_error("Register interface not implemented\n");

    uint8_t reg;
    _idleEn->getVal(&reg);
    return (reg & 0x01)?true:false;
}

void IThrChannel::setIdleEn(const bool en) const
{
    if (!_idleEn)
        throw std::runtime_error("Register interface not implemented\n");

    _idleEn->setVal(en);
}

bool IThrChannel::getAltEn() const
{ 
    if (!_altEn)
        throw std::runtime_error("Register interface not implemented\n");

    uint8_t reg;
    _altEn->getVal(&reg);
    return (reg & 0x01)?true:false;

}

bool IThrChannel::getLcls1En() const
{
    if (!_lcls1En)
        throw std::runtime_error("Register interface not implemented\n");

    uint8_t reg;
    _lcls1En->getVal(&reg);
    return (reg & 0x01)?true:false;
}

uint8_t IThrChannel::getByteMap() const
{
    if (!_byteMap)
        throw std::runtime_error("Register interface not implemented\n");

    uint8_t reg;
    _byteMap->getVal(&reg);
    return reg;
}

const uint32_t IThrChannel::getThreshold(thr_table_t ch) const
{  
    thr_chScalval_t::const_iterator it;
    it = _thrScalvalMap.find(ch);
    if (it != _thrScalvalMap.end())
    {
        uint32_t reg;
        std::get<0>((*it).second)->getVal(&reg, 1);
        return reg;
    }
    else
        throw std::runtime_error("Threshold not defined\n");
}

void IThrChannel::setThreshold(thr_table_t ch, const uint32_t val) const
{
    thr_chScalval_t::const_iterator it;
    it = _thrScalvalMap.find(ch);
    if (it != _thrScalvalMap.end())
    {
        std::get<0>((*it).second)->setVal(val);
    }
    else
        throw std::runtime_error("Threshold not defined\n");
}

void IThrChannel::setThresholdEn(thr_table_t ch, const bool val) const
{
    thr_chScalval_t::const_iterator it;
    it = _thrScalvalMap.find(ch);
    if (it != _thrScalvalMap.end())
    {
        std::get<1>((*it).second)->setVal(val);
    }
    else
        throw std::runtime_error("Threshold not defined\n");
}

const bool IThrChannel::getThresholdEn(thr_table_t ch) const
{ 
    thr_chScalval_t::const_iterator it;
    it = _thrScalvalMap.find(ch);
    if (it != _thrScalvalMap.end())
    {
        uint8_t reg;
        std::get<1>((*it).second)->getVal(&reg, 1);
        return reg?true:false;
    }
    else
        throw std::runtime_error("Threshold not defined\n");
}
