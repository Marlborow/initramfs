#include "std/io.h"
#include "std/string.h"

int main(int argc, String argv[])
{
    if (argc < 1) return EXIT_FAILURE;

    if (argc == 1) {
        argv = (String[]) { "", io.getCWD() };
        argc = 2;
    }

    for (int arg = 1; arg < argc; arg++) {
        if (argc > 2) {
            io.print(argv[arg])->print(":\n");
        }

        Directory dir = io.getDirectory(argv[arg]);
        if (!dir) {
            io.print("Error: Unable to open directory ")->print(argv[arg])->endl();
            continue;
        }

        for (int i = 0; i < dir->size; ++i) {
            DirectoryObject obj = dir[i].object;
            io.print(obj->name)->endl();
        }

        if (arg < argc - 1) io.endl();
    }

    return EXIT_SUCCESS;
}

