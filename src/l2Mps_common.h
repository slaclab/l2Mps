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
    };

    // CPSW get function wrapper
    template <typename T>
    const uint32_t get(const T& reg)
    {
        if (!reg)
            throw std::runtime_error("Register interface not implemented\n");

        uint32_t u32;
        reg->getVal(&u32);
        return u32;
    };

    // CPSW get function wrapper, for getting an element of an array
    template <typename T>
    const uint32_t get(const T& reg, const std::size_t index)
    {

        if (!reg)
            throw std::runtime_error("Register interface not implemented\n");

        std::size_t n;

        try
        {
            n = reg->getNelms();
        }
        catch (CPSWError& e)
        {
            throw std::runtime_error("CPSW error found while trying to get the size of the register array\n");
        }

        if (index > n)
            throw std::runtime_error("Register index aout of range\n");

        uint32_t u32[n];
        reg->getVal(u32,n);
        return u32[index];
    }

    // CPSW set function wrapper
    template <typename T>
    const void set(const T& reg, const uint32_t val)
    {
        if (!reg)
            throw std::runtime_error("Register interface not implemented\n");

        reg->setVal(val);
    };

    // CPSW command exevute wrapper
    const void execute(const Command cmd);
}

#endif