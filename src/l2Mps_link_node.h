#ifndef L2MPS_LINK_NODE_H
#define L2MPS_LINK_NODE_H

/**
 *-----------------------------------------------------------------------------
 * Title      : Link Node class
 * ----------------------------------------------------------------------------
 * File       : l2Mps_link_node.h
 * Author     : Jesus Vasquez, jvasquez@slac.stanford.edu
 * Created    : 2020-04-30
 * ----------------------------------------------------------------------------
 * Description:
 * Class for interfacing with the Link Node
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
#include "l2Mps_soft_inputs.h"

class IMpsLinkNode;

typedef boost::shared_ptr<IMpsLinkNode> MpsLinkNode;

// Default Path to the Link Node module
const std::string defaultLinkNodePath("mmio/AppTop/AppCore/MpsLinkNodeCore/");

// Link node data
struct ln_data_t
{
    si_data_t softInputData;
};

class IMpsLinkNode
{
public:
    IMpsLinkNode(Path root);

    // Factory method, which returns a smart pointer
    static MpsLinkNode create(Path root);

    // Get the soft input object
    MpsSoftInputs getMpsSoftInputs() const { return mpsSoftInputs; };

    // Get data
    ln_data_t getData() const;

private:
    Path          linkNodeRoot;     // Link Node path
    MpsSoftInputs mpsSoftInputs;    // Soft inputs object
};

#endif