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
    Path _chRoot = mpsRoot->findByName(chStr.str().c_str());
    uint8_t u8;

    // Get Channel header information register
    IScalVal_RO::create( _chRoot->findByName( "thresholdCount" ))->getVal(&_thrCount);
    // IScalVal_RO::create( _chRoot->findByName( "idleEn"         ))->getVal(&u8);
    // _idleEn = (u8 & 0x01)?true:false;
    IScalVal_RO::create( _chRoot->findByName( "altEn"          ))->getVal(&u8);
    _altEn = (u8 & 0x01)?true:false;
    IScalVal_RO::create( _chRoot->findByName( "lcls1En"        ))->getVal(&u8);
    _lcls1En = (u8 & 0x01)?true:false;
    IScalVal_RO::create( _chRoot->findByName( "byteMap"        ))->getVal(&_byteMap);

    _idleEn = IScalVal::create(_chRoot->findByName("idleEn"));
    
    // Cretae interfaces to LCLS1 Thresholds (table index 0, threshold count 1)
    _thrEnMap.insert( std::make_pair( thr_channel_t{{0,0,0}}, IScalVal::create( _chRoot->findByName( "lcls1Thr/minEn" ) ) ) );
    _thrEnMap.insert( std::make_pair( thr_channel_t{{0,1,0}}, IScalVal::create( _chRoot->findByName( "lcls1Thr/maxEn" ) ) ) );
    _thrMap.insert(   std::make_pair( thr_channel_t{{0,0,0}}, IScalVal::create( _chRoot->findByName( "lcls1Thr/min"   ) ) ) );
    _thrMap.insert(   std::make_pair( thr_channel_t{{0,1,0}}, IScalVal::create( _chRoot->findByName( "lcls1Thr/max"   ) ) ) );

    // Cretae interfaces to IDLE Thresholds (table index 1, threshold count 1)
    _thrEnMap.insert( std::make_pair( thr_channel_t{{1,0,0}}, IScalVal::create( _chRoot->findByName( "idleThr/minEn" ) ) ) );
    _thrEnMap.insert( std::make_pair( thr_channel_t{{1,1,0}}, IScalVal::create( _chRoot->findByName( "idleThr/maxEn" ) ) ) );
    _thrMap.insert(   std::make_pair( thr_channel_t{{1,0,0}}, IScalVal::create( _chRoot->findByName( "idleThr/min"   ) ) ) );
    _thrMap.insert(   std::make_pair( thr_channel_t{{1,1,0}}, IScalVal::create( _chRoot->findByName( "idleThr/max"   ) ) ) );

    // Cretae interfaces to STD Thresholds (table index 2, threshold count numThrCounts[2])
    std::stringstream regName;
    for (int i=0; i<numThrCounts[2]; ++i)
    {
        regName.str("");
        regName << "stdThr[" << unsigned(i) << "]/";
        _thrEnMap.insert( std::make_pair( thr_channel_t{{2,0,i}}, IScalVal::create( _chRoot->findByName( std::string(regName.str() + "minEn").c_str() ) ) ) );
        _thrEnMap.insert( std::make_pair( thr_channel_t{{2,1,i}}, IScalVal::create( _chRoot->findByName( std::string(regName.str() + "maxEn").c_str() ) ) ) );
        _thrMap.insert(   std::make_pair( thr_channel_t{{2,0,i}}, IScalVal::create( _chRoot->findByName( std::string(regName.str() + "min").c_str()   ) ) ) );
        _thrMap.insert(   std::make_pair( thr_channel_t{{2,1,i}}, IScalVal::create( _chRoot->findByName( std::string(regName.str() + "max").c_str()   ) ) ) );
    }

    // Cretae interfaces to ALT Thresholds (table index 3, threshold count numThrCounts[3])
    for (int i=0; i<numThrCounts[3]; ++i)
    {
        regName.str("");
        regName << "altThr[" << unsigned(i) << "]/";
        _thrEnMap.insert( std::make_pair( thr_channel_t{{3,0,i}}, IScalVal::create( _chRoot->findByName( std::string(regName.str() + "minEn").c_str() ) ) ) );
        _thrEnMap.insert( std::make_pair( thr_channel_t{{3,1,i}}, IScalVal::create( _chRoot->findByName( std::string(regName.str() + "maxEn").c_str() ) ) ) );
        _thrMap.insert(   std::make_pair( thr_channel_t{{3,0,i}}, IScalVal::create( _chRoot->findByName( std::string(regName.str() + "min").c_str()   ) ) ) );
        _thrMap.insert(   std::make_pair( thr_channel_t{{3,1,i}}, IScalVal::create( _chRoot->findByName( std::string(regName.str() + "max").c_str()   ) ) ) );
    }
}

IThrChannel::~IThrChannel()
{
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

bool IThrChannel::getIdleEn() const
{
    uint32_t u8;
    _idleEn->getVal(&u8);
    return (u8 & 0x01)?true:false;
}

void IThrChannel::setIdleEn(const bool en) const
{
    _idleEn->setVal(en);
}
