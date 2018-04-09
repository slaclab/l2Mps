#ifndef L2MPS_COMMON_H
#define L2MPS_COMMON_H

#include <iostream>
#include <stdexcept>
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
        try
        {
            reg->getVal(&u32);
        }
        catch (CPSWError& e)
        {
            throw std::runtime_error("CPSW error found while reading register\n");
        }

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
        try
        {
            reg->getVal(u32,n);
        }
        catch (CPSWError& e)
        {
            throw std::runtime_error("CPSW error found while reading register\n");
        }

        return u32[index];
    }

    // CPSW get function wrapper, for getting an array
    template <typename T, typename U>
    const void get(const T& reg, std::vector<U>& vals)
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

        vals.resize(n);

        try
        {
            reg->getVal(&vals[0], n);
        }
        catch (CPSWError& e)
        {
            throw std::runtime_error("CPSW error found while reading the register array\n");
        }
    };

    // CPSW set function wrapper
    template <typename T>
    const void set(const T& reg, const uint32_t val)
    {
        if (!reg)
            throw std::runtime_error("Register interface not implemented\n");

        try
        {
            reg->setVal(val);
        }
        catch (CPSWError& e)
        {
            throw std::runtime_error("CPSW error found while setting register\n");
        }
    };

    // CPSW command exevute wrapper
    const void execute(const Command cmd);
}

#endif