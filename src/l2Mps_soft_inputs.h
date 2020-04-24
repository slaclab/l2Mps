#ifndef L2MPS_SOFT_INPUTS_H
#define L2MPS_SOFT_INPUTS_H

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

#include <string.h>
#include <stdexcept>

#include "l2Mps_cpsw.h"

class IMpsSoftInputs;

typedef boost::shared_ptr<IMpsSoftInputs> MpsSoftInputs;

// Name of the Digital Message module
const std::string MpsDigitalMessageModuleName("AppTop/AppCore/MpsLinkNodeCore/MpsDigitalMessage/");

class IMpsSoftInputs
{
public:
    IMpsSoftInputs(Path root);

    // Factory method, which returns a smart pointer
    static MpsSoftInputs create(Path root);

    // Input values
    std::pair<bool, bool> getInput(std::size_t index)               const;
    bool                  setInput(bool val, std::size_t index)     const;

    // Input error values
    std::pair<bool, bool> getErrorInput(std::size_t index)           const;
    bool                  setErrorInput(bool val, std::size_t index) const;

private:
    CpswRegRW<uint16_t> swBitValue; // Register to set values
    CpswRegRW<uint16_t> swBitError; // Register with values used during timeouts
    std::size_t         numInputs;  // Number of software bits
};

#endif