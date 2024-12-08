#include "linear-scan.h"

#include <algorithm>
#include <cassert>
#include <iostream>

namespace regalloc {
static std::string RegName(int idx) {
    char s[10];
    snprintf(s, sizeof s, "Reg%d", idx);
    return s;
}

LinearScan::LinearScan(int R)
    : RegAllocImpl(R)
{

}

AllocResult LinearScan::Run(const LiveIntervalVec_t& live_intervals)
{
    Reset();

    AllocResult result;
    alloc_result_ = &result;

    LiveIntervalVec_t sorted(live_intervals);
    // sort intervals in order of increasing start point
    std::sort(sorted.begin(), sorted.end(), [](const LiveInterval* i1, const LiveInterval* i2) {
        return i1->interal().start() < i2->interal().start();
    });
    std::cerr << "sorted intervals:\n" << sorted << "\n";

    for (auto&& vi : sorted) {
        ExpireOldIntevals(vi);

        if (actives_.size() == R_) {
            SpillAtInterval(vi);
        } else {
            auto reg_idx = AllocReg();
            AssignReg(vi, reg_idx);
            AddToActive(vi, reg_idx);
        }
    }
    return result;
}

void LinearScan::Reset()
{
    alloc_result_ = nullptr;
    actives_.clear();
    regstates_.clear();
    stkloc_ = 0;

    std::cerr << R_ << " registered to be allocated\n";
    for (int i = 0; i < R_; i++) {
        regstates_.emplace_back(Reg{RegName(i)});
        std::cerr << "reg " << regstates_.back().reg_.name() << "\n";
    }
}

/// try to determine if there is old interval which could have determined reg assigned
void LinearScan::ExpireOldIntevals(const LiveInterval* vi)
{
    if (actives_.empty()) {
        return;
    }
    /// find the upper bound (>) interval whose end point farther than this vi's start point
    LiveInterval sp(nullptr, IntervalRange{0, vi->interal().start()});
    VarRegState lookup{&sp, -1};
    auto ub = actives_.upper_bound(lookup);
    /// 那些在这个程序点之前的变量区间就可以确定分配到之前待定的寄存器
    /// 因为它们的生命期已经结束
    /// 释放寄存器，从而其它变量可以分配使用
    for (auto it = actives_.begin(); it != ub; ++it) {
        FreeReg(it->reg_idx_);
    }
    actives_.erase(actives_.begin(), ub);
}

void LinearScan::SpillAtInterval(const LiveInterval* vi)
{
    if (!actives_.empty()) {
        auto* spill = actives_.rbegin()->interal_;
        auto reg_idx = actives_.rbegin()->reg_idx_;
        /// this spill is farther away than this `vi`
        if (spill->interal().end() > vi->interal().end()) {
            /// reigster[i] = register[spill]
            AssignReg(vi, reg_idx);
            /// location[spill] = new stackloc
            SpillVar(spill);
            /// NOTE: this var is spilled, determined, from beginning to end
            /// even though it's reg assigned before this point
            AssignReg(spill, -1);
            /// remove spill from active
            auto toremove = actives_.rbegin();
            ++toremove;
            actives_.erase(toremove.base());
            /// add i to active, sorted by increasing end point
            AddToActive(vi, reg_idx);
            return;
        }
        // fall throguh to spill current this var/interval
    }
    SpillVar(vi);
}

int LinearScan::AllocReg()
{
    for (auto i = 0; i < regstates_.size(); i++) {
        auto& regstate = regstates_[i];
        if (regstate.free_) {
            regstate.free_ = false;
            return i;
        }
    }
    return -1;
}

void LinearScan::FreeReg(int reg_idx)
{
    assert(reg_idx >= 0 && reg_idx < regstates_.size());
    regstates_[reg_idx].free_ = true;
}

void LinearScan::AssignReg(const LiveInterval* vi, int reg_idx)
{
    if (reg_idx >= 0) {
        alloc_result_->regs().emplace(vi->var(), regstates_.at(reg_idx).reg_);
    } else {
        alloc_result_->regs().erase(vi->var());
    }
}

void LinearScan::AddToActive(const LiveInterval* vi, int reg_idx)
{
    actives_.emplace(VarRegState{vi, reg_idx});
}

void LinearScan::SpillVar(const LiveInterval* vi)
{
    alloc_result_->spills().emplace(vi->var(), StackLoc{NewStackLoc()});
}

}  // namespace regalloc
