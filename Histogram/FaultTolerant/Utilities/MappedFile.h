
#pragma once

#include <string>


class MappedFile
{

    int FileDescriptor;

public:

    MappedFile(std::string const & fileName);
    ~MappedFile();

    char * Contents;
    unsigned int Size;
};

