#include "Auxiliary.h"
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace Auxiliary {

    static std::string doubleToString(long double value, int n) {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(n) << value;
        return ss.str();
    }

    std::string getSizeDefaultFormat(uint64_t sz) {
        return std::to_string(sz);
    }

    std::string getSizeHumanFormat(uint64_t sz) {
        static constexpr char unitsOfInformation[] =
                {
                        'K',
                        'M',
                        'G',
                        'T',
                        'P',
                        'E',
                };

        static constexpr uint64_t multiplier = 1024ul;
        static constexpr int unitsArrMaxIdx = (sizeof(unitsOfInformation) / sizeof(unitsOfInformation[0])) - 1;

        if (sz < multiplier)
            return getSizeDefaultFormat(sz);

        std::string ret;

        int arrIdx = -1;

        long double value = sz;
        uint64_t intValue = sz;

        while (intValue >= multiplier && arrIdx < unitsArrMaxIdx) {
            value /= (long double)multiplier;
            intValue /= multiplier;
            ++arrIdx;
        }

        long double rem = value - intValue;
        if (intValue > 10 && rem > 0.0) {
            if (rem > 0.0)
                ++intValue;
            ret.append(std::to_string(intValue));
        } else {
            int n = 0;
            if (arrIdx == 0 || rem > 0.1) // KiB
                n = 1;
            ret = doubleToString(value, n);
        }

        if (arrIdx != -1)
            ret.append(1, unitsOfInformation[arrIdx]);

        return ret;
    }
}