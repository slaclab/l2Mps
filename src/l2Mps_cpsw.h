#ifndef L2MPS_CPSW_H
#define L2MPS_CPSW_H

#include <iostream>
#include <cpsw_api_user.h>

// Function to create CPSW interfaces
template<typename T>
T createCPSWInterface(const Path&, const std::string&);

template<>
inline
ScalVal_RO createCPSWInterface(const Path& root, const std::string& regName)
{
    return IScalVal_RO::create(root->findByName(regName.c_str()));
}

template<>
inline
ScalVal createCPSWInterface(const Path& root, const std::string& regName)
{
    return IScalVal::create(root->findByName(regName.c_str()));
}

template<>
inline
Command createCPSWInterface(const Path& root, const std::string& regName)
{
    return ICommand::create(root->findByName(regName.c_str()));
}

// This doesn't seem to be supported by the rhel6 compiler
//template<typename T> 
//using cpsw_pair_t = std::pair<bool,T>;

// CPSW base clases. Includes common functions to all registers.
template<typename T, typename U>
class CpswRegBase
{
public:
    CpswRegBase(const Path& root, const std::string& name)
    :
        root(root),
        name(name),
        nElms(0),
        nBits(0)
    {
        try
        {
            reg   = createCPSWInterface<T>(root, name);
            nElms = reg->getNelms();
            nBits = reg->getSizeBits();
        }
        catch(CPSWError& e)
        {
            //std::cout << "Failed to created " << name << std::endl;
        }
    };

    CpswRegBase() : name(), nElms(0), nBits(0) {};

    CpswRegBase& operator=(const CpswRegBase& rhs)
    {
        root  = rhs.root;
        reg   = rhs.reg;
        name  = rhs.name;
        nElms = rhs.nElms;
        nBits = rhs.nBits;  
        return *this;
    };

    virtual ~CpswRegBase() {};

    std::pair<bool, U> get() const
    {
        bool valid = false;
        U    val   = 0;
    
        if (nElms == 1)
        {
            try
            {
                reg->getVal(&val);
                valid = true;
            }
            catch(CPSWError& e)
            {
            }
        }
        return std::make_pair(valid, val);
    };
 
    std::pair<bool, U> get(const size_t& index) const
    {
        bool valid = false;
        U    val   = 0;
    
        if (nElms > index)
        {
            try
            {
                IndexRange rng(index);
                reg->getVal( &val, 1, &rng );
                valid = true;
            }
            catch(CPSWError& e)
            {
            }
        }
        return std::make_pair(valid, val);
    };
 
    std::pair< bool, std::vector<U> > getArray() const
    {
        bool           valid = false;
        std::vector<U> vals(nElms);
    
        if (nElms > 1)
        {
            try
            {
                reg->getVal( vals.data(), nElms );
                valid = true;
            }
            catch(CPSWError& e)
            {
            }
        }
        return std::make_pair(valid, vals);
    };

    std::string getName()     const { return name;  };
    size_t      getNelms()    const { return nElms; };
    size_t      getSizeBits() const { return nBits; };

protected:
    Path        root;
    T           reg;
    std::string name;
    size_t      nElms;
    size_t      nBits;   
};

// Class for RO registers
template<typename T>
class CpswRegRO : public CpswRegBase<ScalVal_RO, T>
{
public:
    CpswRegRO(const Path& root, const std::string& name) : CpswRegBase<ScalVal_RO, T>(root, name) {};
    CpswRegRO() : CpswRegBase<ScalVal_RO, T>() {};
    CpswRegRO& operator=(const CpswRegRO& rhs)
    {
        CpswRegBase<ScalVal_RO, T>::operator=(rhs);
        return *this;
    }; 
    virtual ~CpswRegRO() {};
};

// Class for RW registers
template<typename T>
class CpswRegRW : public CpswRegBase<ScalVal, T>
{
public:
    CpswRegRW(const Path& root, const std::string& name) : CpswRegBase<ScalVal, T>(root, name) {};
    CpswRegRW() : CpswRegBase<ScalVal, T>() {};
    CpswRegRW& operator=(const CpswRegRW& rhs)
    {
        CpswRegBase<ScalVal, T>::operator=(rhs);
        return *this;
    };
    virtual ~CpswRegRW() {};

    bool set(const T& val) const
    {
        bool ret = false;
    
        if (this->nElms == 1)
        {
            try
            {
                this->reg->setVal(val);
                ret = true;
            }
            catch(CPSWError& e)
            {
            }
        }
        return ret;
    };
};

// Class for commands
class CpswCmd
{
public:
    CpswCmd(const Path& root, const std::string& name)
    :
        name(name),
        created(false)
    {
        try
        {
            cmd = createCPSWInterface<Command>(root, name);
            created = true; 
        }
        catch(CPSWError &e)
        {
        }
    }
    virtual ~CpswCmd() {};

    bool exe() const
    {
        bool ret = false;

        if (created)
        {
            try
            {
                cmd->execute();
                ret = true;
            }
            catch(CPSWError &e)
            {
            }
        }

        return ret;
    };

private:
    std::string name;
    bool        created;
    Command     cmd;
};

#endif
