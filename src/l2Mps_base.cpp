#include "l2Mps_base.h"

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
}