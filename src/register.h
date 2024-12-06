#pragma once

#include <string>

namespace regalloc {
struct Reg {
    std::string name_;

    Reg() = default;
    explicit Reg(const std::string& name)
        : name_(name)
    { }

    const char* name() const { return name_.c_str(); }
};

}  // namespace regalloc
