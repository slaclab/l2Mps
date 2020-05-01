#ifndef L2MPS_SOFT_INPUTS_H
#define L2MPS_SOFT_INPUTS_H

/**
 *-----------------------------------------------------------------------------
 * Title      : Link Node Software Inputs class
 * ----------------------------------------------------------------------------
 * File       : l2Mps_soft_inputs.h
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
#include <boost/make_shared.hpp>

#include "l2Mps_cpsw.h"

class IMpsSoftInputs;

typedef boost::shared_ptr<IMpsSoftInputs> MpsSoftInputs;

// Name of the digital message module (relative to the Link Node module)
const std::string MpsDigitalMessageModuleName("MpsDigitalMessage/");

// Soft input word status
struct si_data_t
{
    std::pair<bool, uint16_t> inputWord;
    std::pair<bool, uint16_t> errorWord;
};

class IMpsSoftInputs
{
public:
    IMpsSoftInputs(Path root);

    // Factory method, which returns a smart pointer
    static MpsSoftInputs create(Path root);

    // Get number of inputs
    std::size_t getNumInputs() const { return numInputs; };

    // Input values
    std::pair<bool, bool> getInput(std::size_t index) const;
    bool setInput(std::size_t index, bool val) const;

    // Input error values
    std::pair<bool, bool> getErrorInput(std::size_t index) const;
    bool setErrorInput(std::size_t index, bool val) const;

    // Get the full words
    std::pair<bool, uint16_t> getInputWord() const      { return swBitValue.get(); };
    std::pair<bool, uint16_t> getErrorInputWord() const { return swBitError.get(); };

    // Get Data
    si_data_t getData() const;

private:
    Path                digMesRoot; // Root to the MpsDigitalMessage module
    CpswRegRW<uint16_t> swBitValue; // Register to set values
    CpswRegRW<uint16_t> swBitError; // Register with values used during timeouts
    std::size_t         numInputs;  // Number of software bits

    // Helper methods
    std::pair<bool, bool> getBit(const CpswRegRW<uint16_t>& reg, std::size_t index) const;
    bool setBit(const CpswRegRW<uint16_t>& reg, bool val, std::size_t index) const;
};

#endif
