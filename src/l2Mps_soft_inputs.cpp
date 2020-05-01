/**
 *-----------------------------------------------------------------------------
 * Title      : Link Node Software Inputs class
 * ----------------------------------------------------------------------------
 * File       : l2Mps_soft_inputs.cpp
 * Author     : Jesus Vasquez, jvasquez@slac.stanford.edu
 * Created    : 2020-04-24
 * ----------------------------------------------------------------------------
 * Description:
 * Class for interfacing with the Link Node Software Inputs
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

#include "l2Mps_soft_inputs.h"

MpsSoftInputs IMpsSoftInputs::create(Path root)
{
    if(!root)
        throw std::runtime_error("The root Path is empty");

    return boost::make_shared<IMpsSoftInputs>(root);
}

IMpsSoftInputs::IMpsSoftInputs(Path root)
:
    digMesRoot ( root->findByName(MpsDigitalMessageModuleName.c_str())  ),
    swBitValue ( digMesRoot, MpsDigitalMessageModuleName + "SwBitValue" ),
    swBitError ( digMesRoot, MpsDigitalMessageModuleName + "SwBitError" ),
    numInputs  ( swBitValue.getSizeBits() )
{
    // Verify that both register has the same number of bits
    if ( numInputs != swBitError.getSizeBits() )
        throw std::runtime_error("SwBitValue and SwBitError has different sizes");
}

std::pair<bool, bool> IMpsSoftInputs::getInput(std::size_t index) const
{
    return getBit(swBitValue, index);
}

bool IMpsSoftInputs::setInput(std::size_t index, bool val) const
{
    return setBit(swBitValue, val, index);
}

std::pair<bool, bool> IMpsSoftInputs::getErrorInput(std::size_t index) const
{
    return getBit(swBitError, index);
}

bool IMpsSoftInputs::setErrorInput(std::size_t index, bool val) const
{
    return setBit(swBitError, val, index);
}

std::pair<bool, bool> IMpsSoftInputs::getBit(const CpswRegRW<uint16_t>& reg, std::size_t index) const
{
    if ( index > numInputs )
        throw std::runtime_error("Trying to access a software input out of bounds");

    // Read the whole register
    std::pair<bool, uint16_t> v { reg.get () };

    // Check if we got a valid reading
    if ( ! v.first )
        return std::make_pair(false, 0);

    // Extract the bit corresponding to the input number
    bool b { ( v.second >> index ) & 1u };

    // Return the pair
    return std::make_pair(true, b);
}

bool IMpsSoftInputs::setBit(const CpswRegRW<uint16_t>& reg, bool val, std::size_t index) const
{
    if ( index > numInputs )
        throw std::runtime_error("Trying to access a software input out of bounds");

    // Read the whole register
    std::pair<bool, uint16_t> v { reg.get () };

    // Check if we got a valid reading
    if ( ! v.first )
        return false;

    // Set the bit corresponding to the input number in the read word
    uint16_t w { v.second };
    if (val)
        w |= 1u << index;
    else
        w &=  ~(1u << index);

    // Write the updated word
    return reg.set(w);
}

si_data_t IMpsSoftInputs::getData() const
{
    si_data_t data;

    data.inputWord = getInputWord();
    data.errorWord = getErrorInputWord();

    return data;
}
