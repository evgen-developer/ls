#ifndef LS_PROCESSINGFILEENTRIES_H
#define LS_PROCESSINGFILEENTRIES_H

#include <functional>
#include <cstdint>
#include <memory>
#include <list>
#include <string>

#include <dirent.h>

#include "../FileItem//FileItem.h"

class ProcessingFileEntries
{
protected:
    const uint8_t flags;

public:
    explicit ProcessingFileEntries(uint8_t flags);
    virtual void process(const char* dirPath, struct dirent* dirEntry) = 0;
    virtual void process(struct stat* st, const char* filename) = 0;
    virtual void push();
    virtual void clear() = 0;

    virtual ~ProcessingFileEntries();
};

#endif //LS_PROCESSINGFILEENTRIES_H
