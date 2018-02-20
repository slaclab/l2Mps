#include "l2Mps_common.h"

namespace IMpsBase
{
    // CPSW interface constructors
    template <>
    ScalVal CPSWCreate(const Path& p, const std::string& regName)
    {
        return IScalVal::create(p->findByName(regName.c_str()));
    }

    template <>
    ScalVal_RO CPSWCreate(const Path& p, const std::string& regName)
    {
        return IScalVal_RO::create(p->findByName(regName.c_str()));
    }

    template <>
    Command CPSWCreate(const Path& p, const std::string& regName)
    {
        return ICommand::create(p->findByName(regName.c_str()));
    }

    // CPSW command exevute wrapper
    const void execute(const Command cmd)
    {
        if (!cmd)
            throw std::runtime_error("Command interface not implemented\n");

        cmd->execute();
    };
}