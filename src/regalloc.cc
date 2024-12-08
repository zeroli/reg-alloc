#include "regalloc.h"

#include "regalloc-impl.h"
#include "linear-scan.h"
#include "linear-scan2.h"

#include <cassert>

namespace regalloc {
namespace factory {
std::unique_ptr<RegAllocImpl> MakeRegAlloc(int R, RegAlloc::algo algo)
{
    switch (algo) {
        default:
        case RegAlloc::algo::LINEAR_SCAN: {
            fprintf(stderr, "linear scan algo chosen for reg alloc\n");
            return std::make_unique<LinearScan>(R);
        }
        case RegAlloc::algo::LINEAR_SCAN2: {
            fprintf(stderr, "linear scan2 algo chosen for reg alloc\n");
            return std::make_unique<LinearScan2>(R);
        }
        case RegAlloc::algo::GRAPH_COLORING: {
            fprintf(stderr, "graph-coloring algo chosen for reg alloc\n");
            assert(0 && "not implemented yet");
        }
    }
    return nullptr;
}
}  // namespace factory

RegAlloc::RegAlloc(int R, RegAlloc::algo algo)
    : regalloc_(factory::MakeRegAlloc(R, algo))
{
}

RegAlloc::~RegAlloc() = default;

AllocResult RegAlloc::Run(const LiveIntervalVec_t& live_intervals)
{
    return regalloc_->Run(live_intervals);
}
}  // namespace regalloc
