#ifndef L2MPS_BSI_H
#define L2MPS_BSI_H

/**
 *-----------------------------------------------------------------------------
 * Title      : Common Platform BSI module class.
 * ----------------------------------------------------------------------------
 * File       : l2Mps_bsi.h
 * Author     : Jesus Vasquez, jvasquez@slac.stanford.edu
 * Created    : 2019-01-08
 * ----------------------------------------------------------------------------
 * Description:
 * Class for interfacing to the BSI registers, included as part of the Common
 * Platform.
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

#include <stdexcept>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

#include "l2Mps_cpsw.h"

class IMpsBsi;

typedef boost::shared_ptr<IMpsBsi>     MpsBsi;

// Default MPS Root Path
const std::string defaultBsiRootPath("mmio/AmcCarrierCore/AmcCarrierBsi/");

class IMpsBsi
{
public:
    IMpsBsi(Path root);
    ~IMpsBsi() {};

    static MpsBsi create(Path root);

    std::pair<bool,uint16_t> getCrateId() const    { return crateId.get();    };
    std::pair<bool,uint8_t>  getSlotNumber() const { return slotNumber.get(); };

private:
    // BSI root path
    Path                bsiRoot;

    // CPSW interfaces
    CpswRegRO<uint16_t> crateId;
    CpswRegRO<uint8_t>  slotNumber;
};

#endif
