#include "syscall_unistd.h"
#include <unistd.h>

int __syscall_uintstd_execve(const char* path, char* const argv[])
{
    return execvp(path, argv);
}

int __syscall_uintstd_fork()
{
    return fork();
}
