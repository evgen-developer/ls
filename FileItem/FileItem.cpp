#include "FileItem.h"

#include <array>
#include <iostream>
#include <ctime>

#include <sys/stat.h>
#include <sys/dir.h>
#include <pwd.h>
#include <grp.h>
#include <csignal>
#include <cstring>


const std::string &FileItem::getAccess() const {
    return access;
}

uint64_t FileItem::getHardLinkCount() const {
    return hardLinkCount;
}

const std::string &FileItem::getUserName() const {
    return userName;
}

const std::string &FileItem::getGroupName() const {
    return groupName;
}

const std::string &FileItem::getSizeStr() const {
    return sizeStr;
}

std::string FileItem::getAccessStr(__mode_t mode) {
    using ItemAccess = std::pair<const __mode_t, char>;
    static constexpr std::array<ItemAccess, 9> accessMap = {
            ItemAccess{S_IRUSR, 'r'},
            ItemAccess{S_IWUSR, 'w'},
            ItemAccess{S_IXUSR, 'x'},

            ItemAccess{S_IRGRP, 'r'},
            ItemAccess{S_IWGRP, 'w'},
            ItemAccess{S_IXGRP, 'x'},

            ItemAccess{S_IROTH, 'r'},
            ItemAccess{S_IWOTH, 'w'},
            ItemAccess{S_IXOTH, 'x'},
    };

    std::string ret;

    if (S_ISLNK(mode))
        ret.append(1,'l');
    else if (S_ISDIR(mode))
        ret.append(1,'d');
    else
        ret.append(1,'-');

    for (const auto& item : accessMap) {
        if (mode & item.first)
            ret.append(1,item.second);
        else
            ret.append(1,'-');
    }

    return ret;
}

void FileItem::fillDateTime(__time_t mtime) {

    // mtime
    struct tm *tmp = std::localtime(&mtime);
    if(tmp == nullptr)
        return;

    // Create a copy on the stack, because this is a pointer to an internal static buffer
    struct tm mtimeTm;
    memcpy(&mtimeTm, tmp, sizeof(mtimeTm));


    // Fill monthBuff
    char monthBuff[4];
    monthBuff[3] = '\0';
    if (strftime(monthBuff, sizeof(monthBuff), "%b", &mtimeTm) == 0) {
        std::cerr << "strftime return 0" << std::endl;
        exit(EXIT_FAILURE);
    }
    dateColumn1 = monthBuff;

    // Fill dayBuff
    char dayBuff[3];
    dayBuff[2] = '\0';
    if (strftime(dayBuff, sizeof(dayBuff), "%e", &mtimeTm) == 0) {
        std::cerr << "strftime return 0" << std::endl;
        exit(EXIT_FAILURE);
    }
    dateColumn2 = dayBuff;

    // Fill time or year
    auto currentTime = std::time(nullptr);
    std::tm* currentTm = std::localtime(&currentTime);

    if (showYear(&mtimeTm, currentTm, mtime, currentTime)) {
        // Year
        dateColumn3 = std::to_string(mtimeTm.tm_year + 1900);
    } else {
        // Time
        char timeBuff[6];
        timeBuff[5] = '\0';
        if (strftime(timeBuff, sizeof(timeBuff), "%H:%M", &mtimeTm) == 0) {
            std::cerr << "strftime return 0" << std::endl;
            exit(EXIT_FAILURE);
        }
        dateColumn3 = timeBuff;
    }

}

std::string FileItem::getUsername(__uid_t uid) {
    auto pw = getpwuid(uid);
    if(pw == nullptr)
        return {};
    return pw->pw_name;
}

std::string FileItem::getGroupname(__gid_t _gid) {
    auto gid = getgrgid(_gid);
    if(gid == nullptr)
        return {};
    return gid->gr_name;
}

const std::string &FileItem::getLinkPath() const {
    return linkPath;
}

const std::string &FileItem::getFileName() const {
    return fileName;
}

FileItem::FileItem(struct stat *st, const char *fileName, const char *fullPath, FuncSz funcSz) {

    if (S_ISLNK(st->st_mode)) {
        char buffLinkPath[PATH_MAX+1];
        const ssize_t sz = readlink(fullPath, buffLinkPath, sizeof(buffLinkPath) - sizeof(char));
        if (sz != -1) {
            buffLinkPath[sz] = '\0';
            this->linkPath.reserve(sz);
            this->linkPath.append(buffLinkPath);
        } else {
            std::cerr << "readlink return error" << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    this->fileName = fileName;
    this->access = getAccessStr(st->st_mode);
    this->userName = getUsername(st->st_uid);
    this->groupName = getGroupname(st->st_gid);
    this->sizeStr = funcSz(st->st_size);
    this->hardLinkCount = st->st_nlink;
    fillDateTime(st->st_mtime);
}

const std::string &FileItem::getDateColumn1() const {
    return dateColumn1;
}

const std::string &FileItem::getDateColumn2() const {
    return dateColumn2;
}

const std::string &FileItem::getDateColumn3() const {
    return dateColumn3;
}

bool FileItem::showYear(std::tm *inputTm, std::tm *currentTm, __time_t inputTime, __time_t currentTime) {
    uint64_t currentYear = currentTm->tm_year;
    uint64_t inputYear = inputTm->tm_year;

    if (inputYear > currentYear)
        return true;

    const auto diffYear = currentYear - inputYear;
    if (diffYear >  1)
        return true;

    else if (diffYear == 1) {
        auto diff = (11 % inputTm->tm_mon) + (currentTm->tm_mon + 1) + 1;
        if (diff > 6) { // half a year
            return true;
        } else if (currentTm->tm_mon == inputTm->tm_mon) {
            if (inputTm->tm_mday <= currentTm->tm_mday) {
                return true;
            }
        }
    }

    // diffYear == 0
    else {
        if (inputTime > currentTime) {
            // one hour ahead or more
            auto diffSec = std::difftime(inputTime, currentTime);
            if (diffSec >= (60.0 * 60.0))
                return true;
        }
    }

    return false;
}

