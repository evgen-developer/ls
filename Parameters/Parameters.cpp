#include <iostream>

#include <getopt.h>
#include <sys/stat.h>

#include "Parameters.h"

Parameters::Input& Parameters::parse(int argc, char **argv) {
    opterr = 1;

    const char* short_options = "lhr";
    const struct option long_options[] = {
            { "help",     0, nullptr, 0 }
    };

    int option;
    while((option = getopt_long(argc, argv, short_options, long_options, nullptr)) != -1) {
        switch(option) {
            case 0:
                printUsage();
                exit(EXIT_SUCCESS);
                //break;
            case 'l':
                input.flags |= longFlag;
                break;
            case 'h':
                input.flags |= humanFormatFlag;
                break;
            case 'r':
                input.flags |= descOrderFlag;
                break;
            default:
                std::cerr
                << "This tool supports -l, -r, -h options"
                << std::endl;
                exit(2); // According to MAN original utility
        }
    }

    // Get directories and files
    for (int i=1; i < argc; ++i) {
        if (argv[i][0] != '-') {

            auto st = new struct stat;
            if (lstat(argv[i], st) != 0) {
                std::cerr << "stat return error" << std::endl;
                exit(EXIT_FAILURE);
            }

            std::shared_ptr<struct stat> stPtr(st);
            if (S_ISDIR(st->st_mode))
                input.dirs.push_back(argv[i]);
            else {
                // Let's save the stat so as not to request twice
                input.files.emplace_back(argv[i], stPtr);
            }
        }
    }

    // If directory/files are not specified, use the current one
    if (input.dirs.empty() && input.files.empty())
        input.dirs.push_back("./");

    return input;
}

Parameters::Parameters() = default;

Parameters &Parameters::getInstance() {
    static Parameters parameters;
    return parameters;
}

const Parameters::Input &Parameters::getInput() const {
    return input;
}

void Parameters::printUsage() {
    std::cout
        << "A simplified analogue of the UNIX utility ls."
        << std::endl
        << "Usage: ls [OPTION]... [FILE]..."
        << std::endl
        << "List information about the FILEs (the current directory by default)."
        << std::endl
        << "Sort entries alphabetically"
        << std::endl
        << "Supported options:"
        << std::endl
        << " -h\twith -l, print sizes like 1K 234M 2G etc."
        << std::endl
        << " -l\tuse a long listing format"
        << std::endl
        << " -r\treverse order while sorting"
        << std::endl
        << " --help\tdisplay this help and exit"
        << std::endl
        << "Exit status:"
        << std::endl
        << " 0  if OK,"
        << std::endl
        << " 1  if minor problems (e.g., cannot access subdirectory),"
        << std::endl
        << " 2  if serious trouble (e.g., cannot access command-line argument)."
        << std::endl;
}
