#ifndef CSHELL_H
#define CSHELL_H

#include "libs/std/io.h"
#include "libs/std/string.h"
#include "libs/system/syscall.h"

typedef struct __CSHELL {
    int (*run)(void);
} __CSHELL;

extern __CSHELL shell;

extern void __cshell_run_process(int *arg);
extern int  __cshell_cd(String directory);
extern void __cshell_setupfs(void);
#endif
