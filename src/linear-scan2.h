#pragma once

#include "regalloc-impl.h"

#include <map>
#include <list>

namespace regalloc {
class LiveInterval;

struct LinearScan2 : RegAllocImpl {
public:
    explicit LinearScan2(int R);
    ~LinearScan2();

    AllocResult Run(const LiveIntervalVec_t& live_intervals) override;

private:
    void Reset();
    int32_t NewStackLoc() {
        return stkloc_++;
    }
    void DetermineIntervalReg(const LiveInterval* vi);
    void SpillAtInterval(const LiveInterval* vi);
    void SpillInterval(const LiveInterval* vi);
    Reg* AllocReg();
    void FreeReg(Reg* reg);

    void CreateFreeRegs();
    void DeleteFreeRegs();
private:
    /// fill it in Run
    AllocResult* alloc_result_{nullptr};

    // stack loc offset for spilled vars
    int32_t stkloc_{0};

    // sorted list in order of increasing end point
    struct IntervalEndLess {
        bool operator ()(const LiveInterval* v1, const LiveInterval* v2) const
        {
            return v1->interal().end() < v2->interal().end();
        }
    };

    using ActiveIntervals = std::map<const LiveInterval*, Reg*, IntervalEndLess>;
    ActiveIntervals actives_;

    /// 可用的寄存器列表
    std::list<Reg*> free_regs_;
};
}  // namespace regalloc
