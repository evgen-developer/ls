#include <iostream>
#include <iomanip>
#include <algorithm>

#include <sys/stat.h>
#include <cstring>

#include "ProcessingFileEntriesLong.h"
#include "../../Parameters/Parameters.h"
#include "../../Auxiliary/Auxiliary.h"

void ProcessingFileEntriesLong::process(const char* dirPath, struct dirent *dirEntry) {

    if (dirPath == nullptr || dirEntry == nullptr)
        throw std::invalid_argument("pointer is null");

    const auto& fullPath = buildFullPath(dirPath, dirEntry->d_name);
    onlyFilepath = false;

    // Get info about file
    struct stat st;
    if (lstat(fullPath.c_str(), &st) != 0) {
        std::cerr << "stat return error " << std::endl;
        exit(EXIT_FAILURE);
    }

    // Create new file item
    auto* newItem = new FileItem(&st, dirEntry->d_name, fullPath.c_str(), funcSz);

    // Calculate blocks count
    totalBlockCount += st.st_blocks;

    // Insert to list
    listItem.push_back(newItem);
}

void ProcessingFileEntriesLong::process(struct stat *st, const char* filePath) {
    if (st == nullptr || filePath == nullptr)
        throw std::invalid_argument("pointer is null");

    // Create new file item
    auto* newItem = new FileItem(st, filePath, nullptr, funcSz);

    // Insert to list
    listItem.push_back(newItem);
}

void ProcessingFileEntriesLong::push() {

    // Sort
    sortItems();

    // ProcessingFileEntries total block count
    std::cout << "total ";
    if (listItem.empty())
        std::cout << 0 << std::endl;

    if (!onlyFilepath) {

        // Output as in the original utility ls
        totalBlockCount *= defaultBlockSize;
        totalBlockCount /= defaultBlockSizeLsTool;

        if (flags & Parameters::humanFormatFlag)
            totalBlockCount *= defaultBlockSizeLsTool;

        std::cout << funcSz(totalBlockCount);
        std::cout << std::endl;
    }

    // Calculate column widths for formatted output
    fillIdentsInColumns();

    // ProcessingFileEntries items
    for (const auto& item : listItem) {
        std::cout
                << item->getAccess()
                << " "
                << std::setw(identsInColumns.hardLink)
                << item->getHardLinkCount()
                << " "
                << std::setw(identsInColumns.groupname)
                << item->getUserName()
                << " "
                << std::setw(identsInColumns.username)
                << item->getGroupName()
                << " "
                << std::setw(identsInColumns.size)
                << item->getSizeStr()
                << " "
                << std::setw(identsInColumns.date1)
                << item->getDateColumn1()
                << " "
                << std::setw(identsInColumns.date2)
                << item->getDateColumn2()
                << " "
                << std::setw(identsInColumns.date3)
                << item->getDateColumn3()
                << " "
                << item->getFileName();

        if (!item->getLinkPath().empty())
            std::cout << " -> " << item->getLinkPath();

        std::cout << std::endl;
    }
}

void ProcessingFileEntriesLong::sortItems() {
    const bool desc = flags & Parameters::descOrderFlag;
    std::function<bool(const FileItem* d1, const FileItem* d2)> comparator;

    if (desc) // Descending order
        comparator = [](const FileItem* d1, const FileItem* d2)
                {return strcasecmp(d1->getFileName().c_str(), d2->getFileName().c_str())>0;};
    else
        comparator = [](const FileItem* d1, const FileItem* d2)
                {return strcasecmp(d1->getFileName().c_str(), d2->getFileName().c_str())<=0;};

    listItem.sort(comparator);
}

void ProcessingFileEntriesLong::fillIdentsInColumns() {

    for (const auto& item : listItem) {
        // Hardlink
        uint64_t tmp = item->getHardLinkCount();
        uint64_t cnt = 0;
        while (tmp){
            tmp /= 10;
            ++cnt;
        }
        if (cnt > identsInColumns.hardLink)
            identsInColumns.hardLink = cnt;

        // Groupname
        cnt = item->getGroupName().length();
        if (cnt > identsInColumns.groupname)
            identsInColumns.groupname = cnt;

        // Username
        cnt = item->getUserName().length();
        if (cnt > identsInColumns.username)
            identsInColumns.username = cnt;

        // Size
        cnt = item->getSizeStr().length();
        if (cnt > identsInColumns.size)
            identsInColumns.size = cnt;

        // Date column 1
        cnt = item->getDateColumn1().length();
        if (cnt > identsInColumns.date1)
            identsInColumns.date1 = cnt;

        // Date column 2
        cnt = item->getDateColumn2().length();
        if (cnt > identsInColumns.date2)
            identsInColumns.date2 = cnt;

        // Date column 3
        cnt = item->getDateColumn3().length();
        if (cnt > identsInColumns.date3)
            identsInColumns.date3 = cnt;
    }

}

ProcessingFileEntriesLong::ProcessingFileEntriesLong(uint8_t flags)
        : ProcessingFileEntries(flags) {

    if (flags & Parameters::humanFormatFlag)
        funcSz = Auxiliary::getSizeHumanFormat;
    else
        funcSz = Auxiliary::getSizeDefaultFormat;
}

void ProcessingFileEntriesLong::clear() {
    totalBlockCount = 0;
    freeList();
    listItem.clear();
    memset(&identsInColumns, 0, sizeof(identsInColumns));
}

ProcessingFileEntriesLong::~ProcessingFileEntriesLong() {
    freeList();
}

void ProcessingFileEntriesLong::freeList() {
    std::for_each(listItem.begin(), listItem.end(), [](FileItem* p){delete p;});
}
