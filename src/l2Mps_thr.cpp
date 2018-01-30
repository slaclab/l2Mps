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
    
    uint8_t enabled;
    _lcls1En->getVal(&enabled);
    
    if (enabled)
    {
        // Cretae interfaces to LCLS1 Thresholds (table index 0, threshold count 1) only is table is enabled
        _thrEnMap.insert( std::make_pair( thr_channel_t{{0,0,0}}, IMpsBase::createInterface<ScalVal>( _chRoot, "lcls1Thr/minEn"  )));
        _thrEnMap.insert( std::make_pair( thr_channel_t{{0,1,0}}, IMpsBase::createInterface<ScalVal>( _chRoot, "lcls1Thr/maxEn"  )));
        _thrMap.insert(   std::make_pair( thr_channel_t{{0,0,0}}, IMpsBase::createInterface<ScalVal>( _chRoot, "lcls1Thr/min"    )));
        _thrMap.insert(   std::make_pair( thr_channel_t{{0,1,0}}, IMpsBase::createInterface<ScalVal>( _chRoot, "lcls1Thr/max"    )));
    }

    _idleEn->getVal(&enabled);
    if (enabled)
    {
        // Cretae interfaces to IDLE Thresholds (table index 1, threshold count 1) only is table is enabled
        _thrEnMap.insert( std::make_pair( thr_channel_t{{1,0,0}}, IMpsBase::createInterface<ScalVal>( _chRoot, "idleThr/minEn"  )));
        _thrEnMap.insert( std::make_pair( thr_channel_t{{1,1,0}}, IMpsBase::createInterface<ScalVal>( _chRoot, "idleThr/maxEn"  )));
        _thrMap.insert(   std::make_pair( thr_channel_t{{1,0,0}}, IMpsBase::createInterface<ScalVal>( _chRoot, "idleThr/min"    )));
        _thrMap.insert(   std::make_pair( thr_channel_t{{1,1,0}}, IMpsBase::createInterface<ScalVal>( _chRoot, "idleThr/max"    )));
    }

    uint8_t numThr;
    _thrCount->getVal(&numThr);

    std::stringstream regName;

    // Cretae interfaces to STD Thresholds (table index 2, threshold count defined in _thrCount)
    for (int i=0; i<numThr; ++i)
    {
        regName.str("");
        regName << "stdThr[" << unsigned(i) << "]/";
        _thrEnMap.insert( std::make_pair( thr_channel_t{{2,0,i}}, IMpsBase::createInterface<ScalVal>( _chRoot, regName.str() + "minEn" )));
        _thrEnMap.insert( std::make_pair( thr_channel_t{{2,1,i}}, IMpsBase::createInterface<ScalVal>( _chRoot, regName.str() + "maxEn" )));
        _thrMap.insert(   std::make_pair( thr_channel_t{{2,0,i}}, IMpsBase::createInterface<ScalVal>( _chRoot, regName.str() + "min"   )));
        _thrMap.insert(   std::make_pair( thr_channel_t{{2,1,i}}, IMpsBase::createInterface<ScalVal>( _chRoot, regName.str() + "max"   )));
    }

    // Cretae interfaces to ALT Thresholds (table index 3, threshold count defined in _thrCount) only is table is enabled
    _altEn->getVal(&enabled);
    if (enabled)
    {
        for (int i=0; i<numThr; ++i)
        {
            regName.str("");
            regName << "altThr[" << unsigned(i) << "]/";
            _thrEnMap.insert( std::make_pair( thr_channel_t{{3,0,i}}, IMpsBase::createInterface<ScalVal>( _chRoot, regName.str() + "minEn" )));
            _thrEnMap.insert( std::make_pair( thr_channel_t{{3,1,i}}, IMpsBase::createInterface<ScalVal>( _chRoot, regName.str() + "maxEn" )));
            _thrMap.insert(   std::make_pair( thr_channel_t{{3,0,i}}, IMpsBase::createInterface<ScalVal>( _chRoot, regName.str() + "min"   )));
            _thrMap.insert(   std::make_pair( thr_channel_t{{3,1,i}}, IMpsBase::createInterface<ScalVal>( _chRoot, regName.str() + "max"   )));
        }
    }
}

IThrChannel::~IThrChannel()
{
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

// const uint32_t IThrChannel::getThreshold(const uint8_t table, const uint8_t limit, const uint8_t position) const
const uint32_t IThrChannel::getThreshold(thr_channel_t ch) const
{  
    thr_channel_map_t::const_iterator it;
    it = _thrMap.find(ch);
    if (it != _thrMap.end())
    {
        uint32_t reg;
        (*it).second->getVal(&reg, 1);
        return reg;
    }
    else
        throw std::runtime_error("Threshold not defined\n");
}

// void IThrChannel::setThreshold(const uint8_t table, const uint8_t limit, const uint8_t position, const uint32_t val) const
void IThrChannel::setThreshold(thr_channel_t ch, const uint32_t val) const
{
    thr_channel_map_t::const_iterator it;
    it = _thrMap.find(ch);
    if (it != _thrMap.end())
    {
        (*it).second->setVal(val);
    }
    else
        throw std::runtime_error("Threshold not defined\n");
}

// void IThrChannel::setThresholdEn(const uint8_t table, const uint8_t limit, const uint8_t position, const bool val) const
void IThrChannel::setThresholdEn(thr_channel_t ch, const bool val) const
{
    thr_channel_map_t::const_iterator it;
    it = _thrEnMap.find(ch);
    if (it != _thrEnMap.end())
    {
        (*it).second->setVal(val);
    }
    else
        throw std::runtime_error("Threshold not defined\n");
}

// const bool IThrChannel::getThresholdEn(const uint8_t table, const uint8_t limit, const uint8_t position) const
const bool IThrChannel::getThresholdEn(thr_channel_t ch) const
{ 
    thr_channel_map_t::const_iterator it;
    it = _thrEnMap.find(ch);
    if (it != _thrEnMap.end())
    {
        uint8_t reg;
        (*it).second->getVal(&reg, 1);
        return reg?true:false;
    }
    else
        throw std::runtime_error("Threshold not defined\n");
}