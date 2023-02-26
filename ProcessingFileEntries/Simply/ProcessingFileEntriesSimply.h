#ifndef LS_PROCESSINGFILEENTRIESSIMPLY_H
#define LS_PROCESSINGFILEENTRIESSIMPLY_H

#include "../ProcessingFileEntries.h"
#include <list>
#include <string>

class ProcessingFileEntriesSimply: public ProcessingFileEntries {

private:
    void sort();
    std::list<std::string> listItem;

public:
    using ProcessingFileEntries::ProcessingFileEntries;

    void process(const char* dirPath, struct dirent *dirEntry) override;
    void process(struct stat *st, const char *filename) override;
    void push() override;
    void clear() override;


    ~ProcessingFileEntriesSimply() override;
};

#endif //LS_PROCESSINGFILEENTRIESSIMPLY_H
