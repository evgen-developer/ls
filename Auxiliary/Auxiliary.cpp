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
                        'Z',
                        'Y',
                        'R',
                        'Q'
                };

        static constexpr uint64_t divider = 1024ul;
        static constexpr int unitsArrMaxIdx = (sizeof(unitsOfInformation) / sizeof(unitsOfInformation[0])) - 1;

        if (sz < divider)
            return getSizeDefaultFormat(sz);

        std::string ret;

        int arrIdx = -1;
        long double value = sz;

        while (value >= divider && arrIdx < unitsArrMaxIdx) {
            value /= (long double)divider;
            ++arrIdx;
        }

        ret = doubleToString(value, 1);
        if (arrIdx != -1)
            ret.append(1, unitsOfInformation[arrIdx]);

        return ret;
    }
}