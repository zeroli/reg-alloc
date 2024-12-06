#pragma once

namespace regalloc {
struct Variable {
    const char* name_ {nullptr};

    explicit Variable(const char* name)
        : name_(name)
    { }

    const char* name() const {
        return name_;
    }
};
}  // namespace regalloc
