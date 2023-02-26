#include <iostream>
#include <algorithm>

#include <dirent.h>

#include "Parameters/Parameters.h"
#include "ProcessingFileEntries/ProcessingFileEntries.h"
#include "ProcessingFileEntries/Creator/ProcessingFileEntriesCreator.h"

static bool filter(struct dirent *d) {
    return d->d_name[0] == '.';
}

int main(int argc, char *argv[]) {

    // Parse parameters
    auto &params = Parameters::getInstance();
    auto &inputParams = params.parse(argc, argv);

    const auto &processingDirEntries = ProcessingFileEntriesCreator::create(inputParams.flags);

    // Processing files
    if (!inputParams.files.empty()) {
        std::cout << std::endl;

        for (const auto &item: inputParams.files)
            processingDirEntries->process(item.second.get(), item.first);

        // Push data to stdout
        processingDirEntries->push();

        // Free resource and clear items
        processingDirEntries->clear();

        // Separate output
        std::cout << std::endl;
    }

    for (const auto &pathItem: inputParams.dirs) {

        // Output fileName
        if (inputParams.dirs.size() > 1 || !inputParams.files.empty())
            std::cout << pathItem << ": " << std::endl;

        // Open directory
        DIR *dir = opendir(pathItem);
        if (!dir) {
            std::cerr << "Cannot open " << pathItem << std::endl;
            exit(EXIT_FAILURE);
        }

        // Get items
        dirent *dirEntry;
        while ((dirEntry = readdir(dir)) != nullptr) {

            if (filter(dirEntry))
                continue;

            processingDirEntries->process(pathItem, dirEntry);
        }

        if (closedir(dir)) {
            std::cerr << "'closedir' return error" << std::endl;
            exit(EXIT_FAILURE);
        }

        // Push data to stdout
        processingDirEntries->push();

        // Free resource and clear items
        processingDirEntries->clear();

        // Separate directory output
        std::cout << std::endl;
    }

    return 0;
}

