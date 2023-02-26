#include "ProcessingFileEntriesCreator.h"
#include "../../Parameters/Parameters.h"
#include "../Long/ProcessingFileEntriesLong.h"
#include "../Simply/ProcessingFileEntriesSimply.h"

std::shared_ptr<ProcessingFileEntries> ProcessingFileEntriesCreator::create(uint8_t flags) {
    ProcessingFileEntries* ptr;
    if (flags & Parameters::longFlag)
        ptr = new ProcessingFileEntriesLong(flags);
    else
        ptr = new ProcessingFileEntriesSimply(flags);

    return std::shared_ptr<ProcessingFileEntries>(ptr);
}
