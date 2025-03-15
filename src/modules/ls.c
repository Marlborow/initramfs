//Directory Listing Program

#include "std/io.h"
#include "std/string.h"

int main(int argc, String argv[])
{
    if(argc < 1) return EXIT_FAILURE;

    if(argc == 1) //we can assume that the command is just 'ls'
    {
        Directory dir = io.getDirectory(io.getCWD());
        for(int i = 0; i < dir->size; ++i)
        {
            DirectoryObject obj = dir[i].object;
            io.print(obj->name)->endl();
        }
    }
    return EXIT_SUCCESS;
}

