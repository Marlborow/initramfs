#include "syscall.h"
struct __Syscall __syscall = {
    .exit       = __syscall_exit,
    .fork       = __syscall_fork,
    .execve     = __syscall_execve,
    .execvp     = __syscall_c_execvp,
    .wait       = __syscall_wait,
    .waitid     = __syscall_waitid,
    .write      = __syscall_write,
    .read       = __syscall_read,
    .open       = __syscall_open,
    .chdir      = __syscall_chdir,
    .mkdir      = __syscall_mkdir,
    .close      = __syscall_close,
    .fsync      = __syscall_fsync,
    .brk        = __syscall_brk,
    .mmap       = __syscall_mmap,
    .munmap     = __syscall_munmap,
    .getpid     = __syscall_getpid,
    .getppid    = __syscall_getppid,
    .getcwd     = __syscall_getcwd,
    .getdents64 = __syscall_getdents64,
    .ioctl      = __syscall_ioctl,
    .fcntl      = __syscall_fcntl,
    .reboot     = __syscall_reboot,
    .mknod      = __syscall_mknod,
    .access     = __syscall_access
};

char** getEnvironment()
{
    return __environ;
}

struct __Process process = {
    .child  = _PTYPE_Child,
    .parent = _PTYPE_Parent,
    .environment = getEnvironment,

    .TYPE   = (struct __ProcessType){
        P_ALL,
        P_PID,
        P_PGID
    },

    .OPTION = (struct __ProcessOption) {
        WEXITED,
        WSTOPPED,
        WCONTINUED
    }
};

struct __FD fd = {
    .out = 1,
    .err = 2,
    .in  = 0
};




int _PTYPE_Child(int x)
{
    if(x == 0) return 1;
    return 0;
}

int _PTYPE_Parent(int x)
{
    if(x > 0) return 1;
    return 0;
}

#include "../../std/string.h"
#include "../../std/memory.h"
#include "../../std/io.h"

int __CheckDirForBinary(const char* dir, const char* binary)
{
    Directory DIR = io.getDirectory(dir);
    if(DIR == NULL) return -1;

    for(int i = 0; i < DIR->size; i++)
    {
        DirectoryObject obj = DIR[i].object;
        if(string.compare(obj->name, binary) == 0)
        {
            return 1;
        }
    }
    return 0;
}




#include "syscall_unistd.h"
int __syscall_c_execvp(const char* file, char* const argv[])
{
   return __syscall_uintstd_execve(file, argv);
}

int __syscall_fork()
{
    return __syscall_uintstd_fork();
}


