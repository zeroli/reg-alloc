#pragma once

#include "live-interval.h"
#include "alloc-result.h"

#include <map>
#include <string>
#include <memory>

namespace regalloc {
class RegAllocImpl;

class RegAlloc {
public:
    enum class algo {
        LINEAR_SCAN = 0,
        LINEAR_SCAN2 = 1,
        GRAPH_COLORING = 2,
    };
    /// R: the number of registers
    RegAlloc(int R, algo algo = algo::LINEAR_SCAN);
    ~RegAlloc();

    AllocResult Run(const LiveIntervalVec_t& live_intervals);

private:
    std::unique_ptr<RegAllocImpl> regalloc_;
};
}  // namespace regalloc
