#ifndef STDIO_H
#define STDIO_H

#define __STDIO_BUFFER_SIZE 1024
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1


#define bool unsigned char
#define true 1
#define false 0

#ifndef NULL
    #define NULL ((void*)0)
#endif

#define IVAL(type,value) &(type){value}


#define LINUX_DIR_FILE 0
#define LINUX_DIR_FOLDER 1
#define LINUX_DIR_EXEC 2
#define ARGS int argc, char* argv[]

#define Directory struct linux_dirent64*
#define DirectoryObject struct linux_dir_object*

void  c_print(const char* str);
void  c_endl();
char* c_getCWD();
char* c_input();
void  c_flush();

struct linux_dir_object {
    int type;
    char* name;
};

struct linux_dirent64 {
    int size;
    struct linux_dir_object* object;
};

struct termInfo {
    int row;
    int column;
};


typedef struct IO {
    struct IO* (*print)(const char*);
    struct IO* (*endl)();

    char* (*input)();
    char* (*getCWD)();
    int   (*setDirectory)(const char*);
    struct linux_dirent64* (*getDirectory)(const char*);
    struct termInfo (*getTermInfo)();

    void  (*flush)();
    int  (*execute)(const char*);
    int  (*executevp) (const char*, char* const*);

} IO;

extern IO io;
#endif

