#pragma once

#include "variable.h"

#include <vector>
#include <sstream>

namespace regalloc {
struct IntervalRange {
    int start_ {-1};
    int end_ {-1};

    IntervalRange() = default;
    IntervalRange(int start, int end)
        : start_(start), end_(end)
    { }

    bool valid() const {
        return end_ >= start_;
    }

    int start() const {
        return start_;
    }
    int end() const {
        return end_;
    }
};

using IntervalRanges_t = std::vector<IntervalRange>;

inline std::ostream& operator <<(std::ostream& os, const IntervalRange& t)
{
    os << "[" << t.start() << ", " << t.end() << "]";
    return os;
}

struct Variable;
struct LiveInterval {
private:
    const Variable* var_ {nullptr};
    IntervalRange interval_;

public:
    LiveInterval() = default;
    LiveInterval(const Variable* var, const IntervalRange& range)
        : var_(var), interval_(range)
    { }

    const Variable* var() const {
        return var_;
    }
    const IntervalRange& interal() const {
        return interval_;
    }
};

inline std::ostream& operator <<(std::ostream& os, const LiveInterval& t)
{
    os << "var: " << t.var()->name() << ", " << t.interal();
    return os;
}

using LiveIntervalVec_t = std::vector<LiveInterval>;

inline std::ostream& operator <<(std::ostream& os, const LiveIntervalVec_t& t)
{
    for (auto&& it : t) {
        os << it << "\n";
    }
    return os;
}

}  // namespace regalloc
