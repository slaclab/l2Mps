/**
 *-----------------------------------------------------------------------------
 * Title      : Link Node class
 * ----------------------------------------------------------------------------
 * File       : l2Mps_link_node.cpp
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

#include "l2Mps_link_node.h"

MpsLinkNode IMpsLinkNode::create(Path root)
{
    if(!root)
        throw std::runtime_error("The root Path is empty");

    return boost::make_shared<IMpsLinkNode>(root);
}

IMpsLinkNode::IMpsLinkNode(Path root)
:
    linkNodeRoot( root->findByName(defaultLinkNodePath.c_str()) ),
    mpsSoftInputs(IMpsSoftInputs::create(linkNodeRoot))
{
}

ln_data_t IMpsLinkNode::getData() const
{
    ln_data_t data;

    data.softInputData = mpsSoftInputs->getData();

    return data;
}
