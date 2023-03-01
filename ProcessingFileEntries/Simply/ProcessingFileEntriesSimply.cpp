#include "ProcessingFileEntriesSimply.h"
#include "../../Parameters/Parameters.h"
#include <iostream>
#include <iterator>
#include <algorithm>


void ProcessingFileEntriesSimply::process(const char* dirPath, struct dirent *dirEntry) {
    if (dirPath == nullptr || dirEntry == nullptr)
        throw std::invalid_argument("arg is null");

    listItem.emplace_back(dirEntry->d_name);
}

void ProcessingFileEntriesSimply::process(struct stat *st, const char *filePath) {
    if (st == nullptr || filePath == nullptr)
        throw std::invalid_argument("arg is null");

    listItem.emplace_back(filePath);
}

void ProcessingFileEntriesSimply::push() {
    // Sort
    sort();

    std::for_each(listItem.begin(), listItem.end(), [](const std::string& s){std:: cout << s << "  ";});
    std::cout << std::endl;
}

void ProcessingFileEntriesSimply::clear() {
    listItem.clear();
}

void ProcessingFileEntriesSimply::sort() {
    const bool desc = flags & Parameters::descOrderFlag;
    std::function<bool(const std::string& d1, const std::string& d2)> comparator;

    if (desc) // Descending order
        comparator = [](const std::string& d1, const std::string& d2)
        {return strcasecmp(d1.c_str(), d2.c_str())>0;};
    else
        comparator = [](const std::string& d1, const std::string& d2)
        {return strcasecmp(d1.c_str(), d2.c_str())<=0;};

    listItem.sort(comparator);
}

ProcessingFileEntriesSimply::~ProcessingFileEntriesSimply() = default;
