#pragma once

#include <cstdint>

namespace regalloc {
/// @brief this class specifies stack location for variable spilled to memory
struct StackLoc {
    int32_t offset_ {-1};  // offset in bytes (or slot) of base

    StackLoc() = default;
    StackLoc(int32_t offset)
        : offset_(offset)
    { }

    int32_t offset() const {
        return offset_;
    }
    void offset(int32_t offset) {
        offset_ = offset;
    }
};
}  // namespace regalloc
