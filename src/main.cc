#include "regalloc.h"

#include <vector>
#include <string>
#include <iostream>
#include <cstdio>
#include <cstdlib>

using namespace regalloc;

static LiveIntervalVec_t MakeLiveIntevals1()
{
    LiveIntervalVec_t intervals;
    /// no free at all for simplity
    intervals.emplace_back(new Variable("A"), IntervalRange{0, 3});
    intervals.emplace_back(new Variable("B"), IntervalRange{1, 5});
    intervals.emplace_back(new Variable("C"), IntervalRange{2, 10});
    intervals.emplace_back(new Variable("D"), IntervalRange{4, 9});
    intervals.emplace_back(new Variable("E"), IntervalRange{5, 8});

    std::cerr << "var intervals:\n" << intervals << "\n";

    return intervals;
}

static LiveIntervalVec_t MakeLiveIntevals2()
{
    LiveIntervalVec_t intervals;
    /// no free at all for simplity
    intervals.emplace_back(new Variable("A"), IntervalRange{0, 3});
    intervals.emplace_back(new Variable("B"), IntervalRange{1, 5});
    intervals.emplace_back(new Variable("C"), IntervalRange{2, 4});
    intervals.emplace_back(new Variable("D"), IntervalRange{4, 9});
    intervals.emplace_back(new Variable("E"), IntervalRange{5, 8});

    std::cerr << "var intervals:\n" << intervals << "\n";

    return intervals;
}

int main(int argc, char** argv)
{
    RegAlloc::algo algo(RegAlloc::algo::LINEAR_SCAN);
    if (argc > 1) {
        algo = static_cast<RegAlloc::algo>(atoi(argv[1]));
    }
    RegAlloc regalloc(2, algo);
    {
        std::cerr << "============\n";
        auto live_intervals = MakeLiveIntevals1();
        auto result = regalloc.Run(live_intervals);
        std::cerr << ">>>>>>>>>>>>\n";
        std::cerr << "reg allocation result1\n" << result;
    }
    {
        std::cerr << "============\n";
        auto live_intervals = MakeLiveIntevals2();
        auto result = regalloc.Run(live_intervals);
        std::cerr << ">>>>>>>>>>>>\n";
        std::cerr << "reg allocation result2\n" << result;
    }
}
