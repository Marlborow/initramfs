#ifndef SYSCALL_UNISTD_H
#define SYSCALL_UNISTD_H

extern int      __syscall_uintstd_execve  (const char *path, char *const argv[]);
extern int      __syscall_uintstd_fork    (void);
#endif
