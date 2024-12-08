#include "linear-scan2.h"

#include <algorithm>
#include <cassert>
#include <iostream>

namespace regalloc {
static std::string RegName(int idx) {
    char s[10];
    snprintf(s, sizeof s, "Reg%d", idx);
    return s;
}

LinearScan2::LinearScan2(int R)
    : RegAllocImpl(R)
{

}

AllocResult LinearScan2::Run(const LiveIntervalVec_t& live_intervals)
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
        DetermineIntervalReg(vi);

        // 没有可用的寄存器了
        if (free_regs_.empty()) {
            SpillAtInterval(vi);
        } else {
            /// 待定区间寄存器分配
            actives_.emplace(vi, AllocReg());
        }
    }

    DetermineIntervalReg(nullptr);
    // 最后的所有的寄存器应该都free了
    assert(free_regs_.size() == R_);
    return result;
}

void LinearScan2::Reset()
{
    alloc_result_ = nullptr;
    actives_.clear();
    stkloc_ = 0;

    for (auto& reg : free_regs_) {
        delete reg;
    }
    free_regs_.clear();

    std::cerr << R_ << " registered to be allocated\n";
    for (int i = 0; i < R_; i++) {
        free_regs_.emplace_back(new Reg{RegName(i)});
        std::cerr << "reg " << free_regs_.back()->name() << "\n";
    }
}

/// try to determine if there is old interval which could have determined reg assigned
void LinearScan2::DetermineIntervalReg(const LiveInterval* vi)
{
    if (actives_.empty()) {
        return;
    }
    auto ub = actives_.end();
    /// vi为null，确定所有的待定都可以分配到寄存器
    if (vi) {
        /// find the upper bound (>) interval whose end point farther than this vi's start point
        LiveInterval lookup(nullptr, IntervalRange{0, vi->interal().start()});
        ub = actives_.upper_bound(&lookup);
    }
    /// 那些在这个程序点之前的变量区间就可以确定分配到之前待定的寄存器
    /// 因为它们的生命期已经结束
    /// 释放寄存器，从而其它变量可以分配使用
    for (auto it = actives_.begin(); it != ub; ++it) {
        free_regs_.emplace_back(it->second);
        alloc_result_->regs().emplace(it->first->var(), *(it->second));
    }
    actives_.erase(actives_.begin(), ub);
}

void LinearScan2::SpillAtInterval(const LiveInterval* vi)
{
    if (!actives_.empty()) {
        auto* spill = actives_.rbegin()->first;
        auto* reg = actives_.rbegin()->second;
        /// this spill is farther away than this `vi`s
        if (spill->interal().end() > vi->interal().end()) {
            // 转移那个寄存器给这个新的变量使用
            actives_.emplace(vi, reg);
            SpillInterval(spill);
            actives_.erase(spill);
            return;
        }
        // fall throguh to spill current this var/interval
    }
    SpillInterval(vi);
}

void LinearScan2::SpillInterval(const LiveInterval* vi)
{
    alloc_result_->spills().emplace(vi->var(), StackLoc{NewStackLoc()});
}

Reg* LinearScan2::AllocReg()
{
    auto* reg = free_regs_.front();
    free_regs_.pop_front();
    return reg;
}

void LinearScan2::FreeReg(Reg* reg)
{
    free_regs_.push_front(reg);
}
}  // namespace regalloc
