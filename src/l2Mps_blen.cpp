#include "l2Mps_blen.h"

IMpsBlen::IMpsBlen(Path mpsRoot, uint8_t amc)
{
    for (uint8_t ch = 0 ; ch < maxChannelCount ; ch++)
    {
        try
        {
            ThrChannel aThr(ThrChannelFactory::create(mpsRoot, ch));

            if (aThr->getThrCount())
            {
                if (aThr->getByteMap() == blenChByteMap[amc])
                {
                    _ch  = ch;
                    _thr = aThr;
                    break;
                }
            }
        }
        catch (CPSWError &e)
        {
        }
    }

    std::cout << "    > A BLEN was created" << std::endl;
    std::cout << "    > Threshold channel map:" << std::endl;
    printChInfo(_thr);
}

IMpsBlen::~IMpsBlen()
{
    std::cout << "    > A BLEN was destroyed" << std::endl;
}

uint32_t const  IMpsBlen::getCh(const blen_channel_t ch) const
{
	if (!_thr)
		throw std::runtime_error("Channel not defined!\n");

	return _ch;
}

bool const  IMpsBlen::getIdleEn(const blen_channel_t ch) const
{
	if (!_thr)
		throw std::runtime_error("Channel not defined!\n");

	return _thr->getIdleEn();
}

bool const  IMpsBlen::getAltEn(const blen_channel_t ch) const
{
	if (!_thr)
		throw std::runtime_error("Channel not defined!\n");

	return _thr->getAltEn();
}

bool const  IMpsBlen::getLcls1En(const blen_channel_t ch) const
{
	if (!_thr)
		throw std::runtime_error("Channel not defined!\n");

	return _thr->getLcls1En();
}

uint32_t const IMpsBlen::getByteMap(const blen_channel_t ch) const
{
	if (!_thr)
		throw std::runtime_error("Channel not defined!\n");

	return _thr->getByteMap();
}

uint32_t const IMpsBlen::getThrCount(const blen_channel_t ch) const
{
	if (!_thr)
		throw std::runtime_error("Channel not defined!\n");

	return _thr->getThrCount();
}

void IMpsBlen::setThreshold(const blen_channel_t ch, const uint32_t val) const
{
	if (!_thr)
		throw std::runtime_error("Channel not defined!\n");

	_thr->setThreshold(ch, val);
}

const uint32_t IMpsBlen::getThreshold(const blen_channel_t ch) const
{
	if (!_thr)
		throw std::runtime_error("Channel not defined!\n");

	return _thr->getThreshold(ch);
}

void IMpsBlen::setThresholdEn(const blen_channel_t ch, const bool val) const
{
	if (!_thr)
		throw std::runtime_error("Channel not defined!\n");

	_thr->setThresholdEn(ch, val);
}

const bool IMpsBlen::getThresholdEn(const blen_channel_t ch) const
{
	if (!_thr)
		throw std::runtime_error("Channel not defined!\n");

	return _thr->getThresholdEn(ch);
}


void IMpsBlen::printChInfo(const ThrChannel thr) const
{
    // if (!thr)
        // throw std::runtime_error("Channel not defined!\n");
    if (thr)
    {
        std::cout << "      channel              = " << unsigned(thr->getChannel()) << std::endl;
        std::cout << "      Threshold count      = " << unsigned(thr->getThrCount()) << std::endl;
        std::cout << "      Idle table enabled?  = " << std::boolalpha << unsigned(thr->getIdleEn()) << std::endl;
        std::cout << "      Alt table enabled?   = " << std::boolalpha << unsigned(thr->getAltEn()) << std::endl;
        std::cout << "      Lcls1 table enabled? = " << std::boolalpha << unsigned(thr->getLcls1En()) << std::endl;
        std::cout << "      Byte map             = " << unsigned(thr->getByteMap()) << std::endl;
    }
    else
    {
        std::cout << "      Channel not defined!" << std::endl;
    }
}