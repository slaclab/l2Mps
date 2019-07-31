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

#include "l2Mps_bsi.h"

MpsBsi IMpsBsi::create(Path root)
{
    if(!root)
        throw std::runtime_error("The root Path is empty");

    return boost::make_shared<IMpsBsi>(root);
}

IMpsBsi::IMpsBsi(Path root)
:
	// BSI root path
	bsiRoot    ( root->findByName(defaultBsiRootPath.c_str()) ),

	// CPSW interfaces
	crateId    ( bsiRoot, "CrateId"    ),
	slotNumber ( bsiRoot, "SlotNumber" )
{
}
