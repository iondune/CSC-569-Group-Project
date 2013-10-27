
#include "MappedFile.h"

#include <cstdio>
#include <cerrno>
#include <cstdlib>
#include <cstring>

#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>


unsigned int GetFileSize(std::string const & fileName)
{
    struct stat sb;
    if (stat(fileName.c_str(), & sb) != 0)
    {
        fprintf(stderr, "'stat' failed for '%s': %s.\n", fileName.c_str(), strerror(errno));
        exit(EXIT_FAILURE);
    }
    return sb.st_size;
}

MappedFile::MappedFile(std::string const & fileName)
{
    Size = GetFileSize(fileName);
    FileDescriptor = open(fileName.c_str(), O_RDONLY, 0);
    if (FileDescriptor == -1)
    {
        fprintf(stderr, "Failed to open file '%s' for reading: %s\n", fileName.c_str(), strerror(errno));
        exit(EXIT_FAILURE);
    }

    Contents = (char *) mmap(
        NULL,
        Size,
        PROT_READ | PROT_WRITE,
        MAP_PRIVATE,
        FileDescriptor,
        0);

    if (Contents == MAP_FAILED)
    {
        fprintf(stderr, "Failed to map file '%s': %s\n", fileName.c_str(), strerror(errno));
        exit(EXIT_FAILURE);
    }
}

MappedFile::~MappedFile()
{
    munmap(Contents, Size);
    close(FileDescriptor);
}

