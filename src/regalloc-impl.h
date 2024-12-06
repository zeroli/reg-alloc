#pragma once

#include "alloc-result.h"
#include "live-interval.h"

namespace regalloc {
class RegAllocImpl {
public:
    explicit RegAllocImpl(int R)
        : R_(R)
    { }
    virtual ~RegAllocImpl() = default;

    virtual AllocResult Run(const LiveIntervalVec_t& live_intervals) = 0;

protected:
    int R_;
};
}  // namespace regalloc
