#ifndef INTERVAL_H
#define INTERVAL_H

#include <limits>

class interval {
    public:
        float min, max;

        interval() : min(+std::numeric_limits<float>::infinity()),
                     max(-std::numeric_limits<float>::infinity()) {} //Default interval is empty

        interval(float min, float max) : min(min), max(max) {}

        float size() const {
            return max - min;
        }

        bool contains(double x) const {
            return min <= x && x <= max;
        }

        bool surrounds (double x) const {
            return min < x && x < max;
        }

        float clamp(float x) const {
            if (x < min) return min;
            if (x > max) return max;
            return x;
        }

        static const interval empty, universe;
};

inline const interval interval::empty = interval(+std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity());
inline const interval interval::universe = interval(-std::numeric_limits<float>::infinity(), +std::numeric_limits<float>::infinity());

#endif