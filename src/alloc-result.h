#pragma once

#include "variable.h"
#include "register.h"
#include "stackloc.h"

#include <map>
#include <sstream>

namespace regalloc {
struct AllocResult {
    struct VarNameCmp {
        bool operator ()(const Variable* v1, const Variable* v2) const {
            return std::string(v1->name()) < std::string(v2->name());
        }
    };
    using VarRegMap_t = std::map<const Variable*, Reg, VarNameCmp>;
    VarRegMap_t& regs() {
        return var_regs_;
    }
    const VarRegMap_t& regs() const {
        return var_regs_;
    }

    using VarStkLocMap_t = std::map<const Variable*, StackLoc, VarNameCmp>;
    VarStkLocMap_t& spills() {
        return var_spills_;
    }
    const VarStkLocMap_t& spills() const {
        return var_spills_;
    }
private:
    VarRegMap_t var_regs_;
    VarStkLocMap_t var_spills_;
};

inline std::ostream& operator <<(std::ostream& os, const AllocResult& result)
{
    os << "variable reg allocation:\n";
    os << "----------------------------\n";
    for (auto&& vr : result.regs()) {
        os << "var: " << vr.first->name() << ", ";
        os << "reg: " << vr.second.name() << "\n";
    }
    os << "variable spill to stack loc:\n";
    os << "----------------------------\n";

    for (auto&& vr : result.spills()) {
        os << "var: " << vr.first->name() << ", ";
        os << "stk: " << vr.second.offset() << "\n";
    }
    return os;
}
}  // namespace regalloc
