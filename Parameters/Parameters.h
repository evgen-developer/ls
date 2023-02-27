#ifndef LS_PARAMETERS_H
#define LS_PARAMETERS_H

#include <string>
#include <list>
#include <memory>

#include <sys/stat.h>

class Parameters {

public:
    using PairFile = std::pair<const char*, std::shared_ptr<struct stat>>;

    struct Input {
        uint8_t flags = noFlags;
        std::list<const char*> dirs;
        std::list<PairFile> files;
    };

private:
    Parameters();
    Input input;

    static void printUsage();

public:
    static constexpr uint8_t noFlags = 0;

    static constexpr uint8_t longFlag = 1;
    static constexpr uint8_t descOrderFlag = 2;
    static constexpr uint8_t humanFormatFlag = 4;

    Input& parse(int argc, char** argv);

    const Input &getInput() const;

    static Parameters& getInstance();

};

#endif //LS_PARAMETERS_H
