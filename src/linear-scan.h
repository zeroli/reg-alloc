#pragma once

#include "regalloc-impl.h"

#include <set>

namespace regalloc {
class LiveInterval;

struct LinearScan : RegAllocImpl {
public:
    explicit LinearScan(int R);

    AllocResult Run(const LiveIntervalVec_t& live_intervals) override;

private:
    void Reset();
    void ExpireOldIntevals(const LiveInterval& vi);
    void SpillAtInterval(const LiveInterval& vi);
    int AllocReg();
    void FreeReg(int idx);
    void AssignReg(const LiveInterval& vi, int reg_idx);
    void SpillVar(const LiveInterval& vi);
    void AddToActive(const LiveInterval& vi, int reg_idx);

    int32_t NewStackLoc() {
        return stkloc_++;
    }
private:
    /// fill it in Run
    AllocResult* alloc_result_{nullptr};

    struct RegState {
        Reg reg_;
        bool free_ {true};

        RegState(const Reg& reg)
            : reg_(reg)
        {}
    };
    std::vector<RegState> regstates_;

    // stack loc offset for spilled vars
    int32_t stkloc_{0};

    // sorted list in order of increasing end point
    struct VarRegState {
        const LiveInterval* interal_;
        int reg_idx_;  // register index
    };
    struct IntervalEndLess {
        bool operator ()(const VarRegState& v1, const VarRegState& v2) const
        {
            return v1.interal_->interal().end() < v2.interal_->interal().end();
        }
    };

    using ActiveIntervalSet_t = std::set<VarRegState, IntervalEndLess>;
    ActiveIntervalSet_t actives_;
};
}  // namespace regalloc
