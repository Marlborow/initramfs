#include "std/file.h"
#include "std/io.h"
#include "std/string.h"

int main(int argc, char* argv[])
{
    if (argc < 2) return -1;
    File x = file.open(argv[1]);
    if (!x) {
        io.print("Error: Could not open file.\n");
        return -1;
    }

    const String content = file.read(x, 0, file.all);
    if (content) {
        io.print(content)->endl();
    } else {
        io.print("Error: Could not read file.\n");
    }

    file.close(x);
    return 0;
}

