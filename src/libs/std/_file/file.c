#include "file.h"
#include "../../system/syscall.h"

#define MAX_FILES 32

static __FileObject file_pool[MAX_FILES];
static int file_pool_used[MAX_FILES];
static char read_buffer[131072];

static int alloc_file_object(const char * name, int fd, File * out) {
    int i;
    for (i = 0; i < MAX_FILES; i++) {
        if (!file_pool_used[i]) {
            file_pool_used[i] = 1;
            file_pool[i].fd = fd;
            file_pool[i].name = name;* out = & file_pool[i];
            return 0;
        }
    }
    return -1;
}
static int free_file_object(File f) {
    int i;
    for (i = 0; i < MAX_FILES; i++) {
        if ( & file_pool[i] == f) {
            file_pool_used[i] = 0;
            return 0;
        }
    }
    return -1;
}
File __file_open(const char * name) {
    int fd = syscall.open(name, 2, 0);
    File f;
    if (fd < 0) fd = -1;
    if (alloc_file_object(name, fd, & f) < 0) return 0;
    return f;
}
void * __file_exist(File f) {
    if (f -> fd >= 0) return f;
    int fd = syscall.open(f -> name, 0, 0);
    if (fd < 0) return 0;
    syscall.close(fd);
    return f;
}
File __file_create(File f) {
    int fd = syscall.open(f -> name, 2 | 64, 0644);
    if (fd < 0) return 0;
    f -> fd = fd;
    return f;
}
const char * __file_read(File f, unsigned long start, unsigned long size) {
    if (f -> fd < 0) return 0;
    int newfd = syscall.open(f -> name, 0, 0);
    if (newfd < 0) return 0;
    unsigned long skipped = 0;
    char tmp;
    while (skipped < start) {
        if (syscall.read(newfd, & tmp, 1) <= 0) break;
        skipped++;
    }
    unsigned long total = 0;
    if (size == FILE_ALL) {
        int r;
        while ((r = syscall.read(newfd, & read_buffer[total], 1)) > 0) {
            total += r;
            if (total >= sizeof(read_buffer)) break;
        }
    } else {
        int r = syscall.read(newfd, read_buffer, size);
        if (r > 0) total = r;
    }
    read_buffer[total] = 0;
    syscall.close(newfd);
    return read_buffer;
}
int __file_append(File f,
                  const char * str) {
    if (f -> fd < 0) return -1;
    int fd = syscall.open(f -> name, 1 | 1024, 0);
    if (fd < 0) return -1;
    int len = 0;
    while (str[len]) len++;
    int r = syscall.write(fd, str, len);
    syscall.close(fd);
    return r;
}
int __file_clear(File f) {
  if (f -> fd < 0) return -1;
  int fd = syscall.open(f -> name, 2 | 512, 0);
  if (fd < 0) return -1;
  syscall.close(fd);
  f -> fd = syscall.open(f -> name, 2, 0);
  return 0;
}
int __file_close(File f) {
  int r = 0;
  if (f -> fd >= 0) {
      r = syscall.close(f -> fd);
      f -> fd = -1;
  }
  free_file_object(f);
  return r;
}
__FileOperations file = {
  .open = __file_open,
  .create = __file_create,
  .exist = __file_exist,
  .read = __file_read,
  .append = __file_append,
  .clear = __file_clear,
  .close = __file_close,
  .all = FILE_ALL
};
