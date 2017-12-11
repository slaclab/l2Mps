#include "l2Mps_bpm.h"

IMpsBpm::IMpsBpm(Path mpsRoot, uint8_t amc)
{

    for (uint8_t ch = 0 ; ch < maxChannelCount ; ch++)
    {
        try
        {
            ThrChannel aThr(ThrChannelFactory::create(mpsRoot, ch));

            if (aThr->getThrCount())
            {
                for (int i = 0; i < numBpmChs; ++i)
                {
                    if (aThr->getByteMap() == bpmChByteMap[amc][i])
                    {
                        _ch[i] = ch;
                        _thr[i] = aThr;
                    }
                }
            }
        }
        catch (CPSWError &e)
        {
        }

    }

    std::cout << "    > A BPM was created" << std::endl;
    std::cout << "    > Threshold channel map:" << std::endl;

    std::cout << "    >   X:" << std::endl;
    printChInfo(_thr[0]);

    std::cout << "    >   Y:" << std::endl;
    printChInfo(_thr[1]);

    std::cout << "    >   C:" << std::endl;
    printChInfo(_thr[2]);

    std::cout << "    >   R:" << std::endl;
    printChInfo(_thr[3]);
}

IMpsBpm::~IMpsBpm()
{
    std::cout << "    > A BPM was destroyed" << std::endl;
}

uint32_t const IMpsBpm::getCh(const bpm_channel&  ch) const                      
{
    if (!_thr[ch.getBpmCh()])
        throw std::runtime_error("Channel not defined!\n");

    return  _ch[ch.getBpmCh()]; 
}

bool const IMpsBpm::getIdleEn(const bpm_channel&  ch) const                      
{
    if (!_thr[ch.getBpmCh()])
        throw std::runtime_error("Channel not defined!\n");

    return _thr[ch.getBpmCh()]->getIdleEn();   
}

bool const  IMpsBpm::getAltEn(const bpm_channel&  ch) const                      
{
    if (!_thr[ch.getBpmCh()])
        throw std::runtime_error("Channel not defined!\n");

    return _thr[ch.getBpmCh()]->getAltEn();   
}

bool const IMpsBpm::getLcls1En(const bpm_channel&  ch) const                      
{
    if (!_thr[ch.getBpmCh()])
        throw std::runtime_error("Channel not defined!\n");

    return _thr[ch.getBpmCh()]->getLcls1En();   
}

uint32_t const IMpsBpm::getByteMap(const bpm_channel&  ch) const                      
{
    if (!_thr[ch.getBpmCh()])
        throw std::runtime_error("Channel not defined!\n");

    return _thr[ch.getBpmCh()]->getByteMap(); 
}

uint32_t const IMpsBpm::getThrCount(const bpm_channel&  ch) const                     
{ 
    if (!_thr[ch.getBpmCh()])
        throw std::runtime_error("Channel not defined!\n");

    return _thr[ch.getBpmCh()]->getThrCount(); 
}

void IMpsBpm::setThreshold(const bpm_channel&  ch, const uint32_t val) const  
{
    if (!_thr[ch.getBpmCh()])
        throw std::runtime_error("Channel not defined!\n");

    _thr[ch.getBpmCh()]->setThreshold(ch.getThrCh(), val); 
}

const uint32_t IMpsBpm::getThreshold(const bpm_channel& ch) const
{
    if (!_thr[ch.getBpmCh()])
        throw std::runtime_error("Channel not defined!\n");

    return _thr[ch.getBpmCh()]->getThreshold(ch.getThrCh()); 
}

void IMpsBpm::setThresholdEn(const bpm_channel&  ch, const bool val) const     
{
    if (!_thr[ch.getBpmCh()])
        throw std::runtime_error("Channel not defined!\n");

    _thr[ch.getBpmCh()]->setThresholdEn(ch.getThrCh(), val); 

}

const bool IMpsBpm::getThresholdEn(const bpm_channel&  ch) const                     
{
    if (!_thr[ch.getBpmCh()])
        throw std::runtime_error("Channel not defined!\n");

    return _thr[ch.getBpmCh()]->getThresholdEn(ch.getThrCh()); 

}


void IMpsBpm::printChInfo(const ThrChannel thr) const
{
    // if (!thr)
        // throw std::runtime_error("Channel not defined!\n");
    if (thr)
    {
        std::cout << "          channel              = " << unsigned(thr->getChannel()) << std::endl;
        std::cout << "          Threshold count      = " << unsigned(thr->getThrCount()) << std::endl;
        std::cout << "          Idle table enabled?  = " << std::boolalpha << unsigned(thr->getIdleEn()) << std::endl;
        std::cout << "          Alt table enabled?   = " << std::boolalpha << unsigned(thr->getAltEn()) << std::endl;
        std::cout << "          Lcls1 table enabled? = " << std::boolalpha << unsigned(thr->getLcls1En()) << std::endl;
        std::cout << "          Byte map             = " << unsigned(thr->getByteMap()) << std::endl;
    }
    else
    {
        std::cout << "          Channel not defined!" << std::endl;
    }
}