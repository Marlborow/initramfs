#include "stdio.h"
#include "../../system/syscall.h"

static IO* io_print(const char*);
static IO* io_endl();
static char* io_input_impl();
static int str_length(const char* s){ int l=0; while(s[l]) l++; return l; }

struct linux_dirent64_raw {
    unsigned long d_ino;
    long d_off;
    unsigned short d_reclen;
    unsigned char d_type;
    char d_name[];
};

static int dir_type(unsigned char t)
{
    if(t == 4) return LINUX_DIR_FOLDER;
    if(t == 8) return LINUX_DIR_FILE;
    if(t == 10) return LINUX_DIR_EXEC;
    return -1;
}

static struct linux_dirent64 entries[128];
static struct linux_dir_object objects[128];
static char namebuf[4096];

static struct linux_dirent64* c_listDirectory(const char* path)
{
    int fd = syscall.open(path, 0, 0);
    if(fd < 0) return 0;

    for(int i=0; i<128; i++){
        entries[i].size   = 0;
        entries[i].object = 0;
    }

    int readlen, count = 0, used = 0;
    static char buf[1024];

    while((readlen = syscall.getdents64(fd, buf, 1024)) > 0 && count < 127)
    {
        int pos = 0;
        while(pos < readlen && count < 127)
        {
            struct linux_dirent64_raw* raw = (struct linux_dirent64_raw*)(buf + pos);

            int n = str_length(raw->d_name);

            if((n == 1 && raw->d_name[0] == '.') ||
               (n == 2 && raw->d_name[0] == '.' && raw->d_name[1] == '.'))
            {
                pos += raw->d_reclen;
                continue;
            }

            entries[count].object = &objects[count];

            if(used + n + 1 >= (int)sizeof(namebuf)) break;
            for(int i=0; i<n; i++){
                namebuf[used + i] = raw->d_name[i];
            }
            namebuf[used + n] = 0;

            objects[count].name = &namebuf[used];
            objects[count].type = dir_type(raw->d_type);

            used += (n + 1);
            count++;
            pos += raw->d_reclen;
        }
    }
    syscall.close(fd);

    for(int i=0; i<count; i++){
        entries[i].size = count;
    }
    entries[count].size   = 0;
    entries[count].object = 0;

    return entries;
}

struct ws_termInfo
{
    unsigned short row;
    unsigned short column;
    unsigned short xpixels;
    unsigned short ypixels;
};

struct termInfo c_getTermInfo()
{
    struct ws_termInfo ws_term;
    ws_term.ypixels = 0;
    ws_term.xpixels = 0;
    ws_term.row     = 0;
    ws_term.column  = 0;

    syscall.ioctl(&ws_term);

    struct termInfo info;
    info.column = ws_term.column;
    info.row = ws_term.row;

    return info;
}

#include <sys/fcntl.h>
void c_flush()
{
    char c;
    int flags = syscall.fcntl(0, F_GETFL, 0);
    syscall.fcntl(0, F_SETFL, flags | O_NONBLOCK);

    while (syscall.read(0, &c, 1) > 0) {
        if (c == '\n') break;
    }

    syscall.fcntl(0, F_SETFL, flags);
}

int c_executevp(const char* str, char* const args[])
{
    PID pid = syscall.fork();
    if(pid < 0) return 0;

    else if(pid == 0)
    {
        if (syscall.execvp(str, args) < 0) {
            return 0;
        }
    }
    else
    {
        syscall.wait(NULL);
    }
    return 1;

}

#include "../string.h"
int c_execute(const char* str)
{
    StringList lineTokens = string.tokenize(str, ' ');
    String* args = lineTokens.data;
    return c_executevp(args[0], args);
}

int c_setDirectory(const char* path)
{
    return syscall.chdir(path);
}

IO io = {
    .print        = io_print,
    .endl         = io_endl,
    .input        = io_input_impl,
    .getCWD       = c_getCWD,
    .getDirectory = c_listDirectory,
    .setDirectory = c_setDirectory,
    .getTermInfo  = c_getTermInfo,
    .flush        = c_flush,
    .execute      = c_execute,
    .executevp    = c_executevp
};



char* c_getCWD()
{
    static char buffer[256];
    long res = syscall.getcwd(buffer, sizeof(buffer));
    if(res > 0) return buffer;
    return 0;
}

void c_print(const char* str)
{
    if(!str) return;
    unsigned long size = 0;
    while(str[size]) size++;
    syscall.write(fd.out, str, size);
}

void c_endl()
{
    c_print("\n");
}


static IO* io_print(const char* str)
{
    c_print(str);
    return &io;
}



static IO* io_endl()
{
    c_endl();
    return &io;
}

static char* io_input_impl()
{
    return c_input();
}

#include "../types.h"
#include "../memory.h"

#define INITIAL_POOL_SIZE 2
static char** inputPool = NULL;
static unsigned long poolCapacity = 0;
static unsigned long poolCount = 0;


char* c_input()
{
    if (inputPool == NULL) {
        poolCapacity = INITIAL_POOL_SIZE;
        inputPool = (char**) memory.allocate(poolCapacity * sizeof(char*));
        if (inputPool == NULL) return (char*)-1;
        poolCount = 0;
    }

    char* newBuffer = (char*) memory.allocate(__STDIO_BUFFER_SIZE);
    if (newBuffer == NULL) return (char*)-1;

    for (unsigned long i = 0; i < __STDIO_BUFFER_SIZE; i++) {
        newBuffer[i] = 0;
    }

    long bytes_read = syscall.read(fd.in, newBuffer, __STDIO_BUFFER_SIZE - 1);
    if (bytes_read <= 0) {
        memory.free(newBuffer);
        return (char*)-1;
    }

    newBuffer[bytes_read] = 0;
    if (bytes_read > 0 && newBuffer[bytes_read - 1] == '\n')
        newBuffer[bytes_read - 1] = 0;

    if (poolCount >= poolCapacity) {
        size_t newCapacity = poolCapacity * 2;
        char** newPool = (char**) memory.allocate(newCapacity * sizeof(char*));
        if (newPool == NULL) {
            memory.free(newBuffer);
            return (char*)-1;
        }
        for (size_t i = 0; i < poolCount; i++) {
            newPool[i] = inputPool[i];
        }

        memory.free(inputPool);
        inputPool = newPool;
        poolCapacity = newCapacity;
    }

    inputPool[poolCount++] = newBuffer;
    return newBuffer;
}
