#ifndef L2MPS_COMMON_H
#define L2MPS_COMMON_H

#include <stdio.h>
#include <iostream>
#include <string.h>
#include <iomanip>
#include <inttypes.h>
#include <map>
#include <stdexcept>
#include <boost/shared_ptr.hpp>
#include <cpsw_api_user.h>

namespace IMpsBase
{
	// CPSW interface constructor
    template <typename T>
    T CPSWCreate(const Path& p, const std::string& regName);

	// CPSW Interfaces constructor wrapper
    template <typename T>
    T createInterface(const Path& p, const std::string& regName)
    {
	    T reg;

	    try
	    {
	        reg = CPSWCreate<T>(p, regName);
	    }
	    catch (CPSWError &e)
	    {
	        std::cout << "Couldn't create interface to \"" << regName << "\": "<< e.getInfo() << std::endl;
	    }

	    return reg;
	}
}

#endif