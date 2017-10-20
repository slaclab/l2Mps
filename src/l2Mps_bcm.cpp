#include "l2Mps_bcm.h"

IMpsBcm::IMpsBcm(Path mpsRoot, uint8_t amc)
{
    for (uint8_t ch = 0 ; ch < maxChannelCount ; ch++)
    {
        try
        {
            ThrChannel aThr(ThrChannelFactory::create(mpsRoot, ch));

            for (int i = 0; i < numBcmChs; ++i)
            {
                if (aThr->getByteMap() == bcmChByteMap[amc][i])
                {
                    _ch[i]  = ch;
                    _thr[i] = aThr;
                    break;
                }
            }
        }
        catch (CPSWError &e)
        {
        }
    }

    std::cout << "    > A BCM was created" << std::endl;
    std::cout << "    > Threshold channel map:" << std::endl;

    std::cout << "    >   BCM:" << std::endl;
    printChInfo(_thr[0]);

    std::cout << "    >   DIFF:" << std::endl;
    printChInfo(_thr[1]);

}

IMpsBcm::~IMpsBcm()
{
    std::cout << "    > A BCM was destroyed" << std::endl;
}

uint32_t const IMpsBcm::getCh(const bcm_channel&  ch) const                      
{
    if (!_thr[ch.getBcmCh()])
        throw std::runtime_error("Channel not defined!\n");

    return  _ch[ch.getBcmCh()]; 
}

bool const IMpsBcm::getIdleEn(const bcm_channel&  ch) const                      
{
    if (!_thr[ch.getBcmCh()])
        throw std::runtime_error("Channel not defined!\n");

    return _thr[ch.getBcmCh()]->getIdleEn();   
}

bool const  IMpsBcm::getAltEn(const bcm_channel&  ch) const                      
{
    if (!_thr[ch.getBcmCh()])
        throw std::runtime_error("Channel not defined!\n");

    return _thr[ch.getBcmCh()]->getAltEn();   
}

bool const IMpsBcm::getLcls1En(const bcm_channel&  ch) const                      
{
    if (!_thr[ch.getBcmCh()])
        throw std::runtime_error("Channel not defined!\n");

    return _thr[ch.getBcmCh()]->getLcls1En();   
}

uint32_t const IMpsBcm::getByteMap(const bcm_channel&  ch) const                      
{
    if (!_thr[ch.getBcmCh()])
        throw std::runtime_error("Channel not defined!\n");

    return _thr[ch.getBcmCh()]->getByteMap(); 
}

uint32_t const IMpsBcm::getThrCount(const bcm_channel&  ch) const                     
{ 
    if (!_thr[ch.getBcmCh()])
        throw std::runtime_error("Channel not defined!\n");

    return _thr[ch.getBcmCh()]->getThrCount(); 
}

void IMpsBcm::setThreshold(const bcm_channel&  ch, const uint32_t val) const  
{
    if (!_thr[ch.getBcmCh()])
        throw std::runtime_error("Channel not defined!\n");

    _thr[ch.getBcmCh()]->setThreshold(ch.getThrCh(), val); 
}

const uint32_t IMpsBcm::getThreshold(const bcm_channel& ch) const
{
    if (!_thr[ch.getBcmCh()])
        throw std::runtime_error("Channel not defined!\n");

    return _thr[ch.getBcmCh()]->getThreshold(ch.getThrCh()); 
}

void IMpsBcm::setThresholdEn(const bcm_channel&  ch, const bool val) const     
{
    if (!_thr[ch.getBcmCh()])
        throw std::runtime_error("Channel not defined!\n");

    _thr[ch.getBcmCh()]->setThresholdEn(ch.getThrCh(), val); 

}

const bool IMpsBcm::getThresholdEn(const bcm_channel&  ch) const                     
{
    if (!_thr[ch.getBcmCh()])
        throw std::runtime_error("Channel not defined!\n");

    return _thr[ch.getBcmCh()]->getThresholdEn(ch.getThrCh()); 

}

void IMpsBcm::printChInfo(const ThrChannel thr) const
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