#ifndef FILE_H
#define FILE_H

#include "syscall.h"
#define FILE_ALL ((unsigned long)-1)

typedef struct __FileObject {
    int fd;
    const char* name;
} __FileObject;

typedef __FileObject* File;

typedef struct __FileOperations {
    File (*open)(const char*);
    File (*create)(File);
    void* (*exist)(File);
    const char* (*read)(File, unsigned long, unsigned long);
    int (*append)(File, const char*);
    int (*clear)(File);
    int (*close)(File);
    unsigned long all;
} __FileOperations;


File __file_open(const char*);
File __file_create(File);
void* __file_exist(File);
const char* __file_read(File, unsigned long, unsigned long);
int __file_append(File, const char*);
int __file_clear(File);
int __file_close(File);


extern __FileOperations file;


#endif
