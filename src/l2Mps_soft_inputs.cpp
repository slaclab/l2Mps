/**
 *-----------------------------------------------------------------------------
 * Title      : Link Node Software Inputs class
 * ----------------------------------------------------------------------------
 * File       : l2Mps_blm.h
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
    swBitValue ( root, MpsDigitalMessageModuleName + "SwBitValue" ),
    swBitError ( root, MpsDigitalMessageModuleName + "SwBitError" ),
    numInputs  ( swBitValue.getSizeBits() )
{
    // Verify that both register has the same number of bits
    if ( numInputs != swBitError.getSizeBits() )
        throw std::runtime_error("SwBitValue and SwBitError has different sizes")
}

std::pair<bool, bool> IMpsSoftInputs::getInput(std::size_t index) const
{
    if ( index > numInputs )
        throw std::runtime_error("Trying to access a software input out of bounds")

}

bool IMpsSoftInputs::setInput(bool val, std::size_t index) const
{
    if ( index > numInputs )
        throw std::runtime_error("Trying to access a software input out of bounds")
}

std::pair<bool, bool> IMpsSoftInputs::getErrorInput(std::size_t index) const
{
    if ( index > numInputs )
        throw std::runtime_error("Trying to access a software input out of bounds")
}

bool IMpsSoftInputs::setErrorInput(bool val, std::size_t index) const
{
    if ( index > numInputs )
        throw std::runtime_error("Trying to access a software input out of bounds")
}
