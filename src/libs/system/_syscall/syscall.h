#ifndef SYSCALL_H
#define SYSCALL_H


extern void     __syscall_exit    (int code);
extern int      __syscall_fork    ();

extern int      __syscall_clone   (unsigned long flags, unsigned long newsp, void* parent_tid, void* child_tid, unsigned int tid);

extern int      __syscall_execve  (const char *path, char *const argv[], char *const envp[]);
extern int      __syscall_wait    (int *wstatus);
extern int      __syscall_waitid  (int p_type, int pid, void *info, int options);
extern void     __syscall_fsync   (int fd);
extern int      __syscall_fcntl   (int fd, int cmd, unsigned long arg);

extern int    __syscall_pipe     (int* filedes);
extern int    __syscall_chdir    (const char* path);

extern int    __syscall_dup2     (unsigned int oldfd, unsigned int newdf);

extern int      __syscall_write   (int fd, const char *buf, int len);
extern int      __syscall_read    (int fd, char *buf, int len);
extern int      __syscall_open    (const char *path, int flags, int mode);
extern int      __syscall_close   (int fd);
extern int      __syscall_brk     (void *addr);
extern void *   __syscall_mmap    (void *addr, int length, int prot, int flags, int fd, int offset);
extern int      __syscall_munmap  (void *addr, int length);
extern int      __syscall_getpid  ();
extern int      __syscall_getppid ();

//Directory SYSCALLS
extern long     __syscall_getcwd  (char *buf, unsigned long size);
extern int      __syscall_getdents64 (int fd, void *buf, int count);

extern int      __syscall_mknod      (const char* filename, unsigned short mode, unsigned dev);

//Terminal Size
extern long __syscall_ioctl(void *buf);
extern char **__environ;
int __syscall_c_execvp(const char* file, char * const argv[]);


extern int __syscall_reboot(int magic1, int magic2, unsigned int cmd, void* arg);
extern void __syscall_mkdir(const char* path, int mode);


struct __Syscall {
    void   (*exit)    (int);
    int    (*fork)    ();
    int    (*clone)   (unsigned long, unsigned long, void*, void*, unsigned int);
    int    (*execve)  (const char *, char *const[], char *const[]);
    int    (*execvp)  (const char *, char *const*);
    int    (*wait)    (int *);
    int    (*waitid)  (int, int, void *, int);
    void   (*fsync)   (int);
    int    (*fcntl)  (int, int, unsigned long);
    int    (*chdir)  (const char*);
    void   (*mkdir)  (const char*, int);

    int    (*pipe)   (int*);
    int    (*dup2)   (unsigned int, unsigned int);

    int    (*getpid)  ();
    int    (*getppid) ();

    int    (*write)   (int, const char *, int);
    int    (*read)    (int, char *, int);
    int    (*open)    (const char *, int, int);
    int    (*close)   (int);

    int    (*brk)     (void *);
    void*  (*mmap)    (void *, int, int, int, int, int);
    int    (*munmap)  (void *, int);
    long   (*getcwd)  (char*, unsigned long);
    int    (*getdents64) (int, void*, int);
    long   (*ioctl)      (void*);

    int    (*reboot)     (int, int, unsigned int, void*);
    int    (*mknod)      (const char*, unsigned short, unsigned dev);
};

/* File types.  */
#define	__S_IFDIR	0040000	/* Directory.  */
#define	__S_IFCHR	0020000	/* Character device.  */
#define	__S_IFBLK	0060000	/* Block device.  */
#define	__S_IFREG	0100000	/* Regular file.  */
#define	__S_IFIFO	0010000	/* FIFO.  */
#define	__S_IFLNK	0120000	/* Symbolic link.  */
#define	__S_IFSOCK	0140000	/* Socket.  */



struct __FD {
    const int out;
    const int err;
    const int in;
};

union __sigval {
	int sival_int;
	void *sival_ptr;
};

typedef struct {
    int             si_signo;
    int             si_code;
    int             si_pid;
    unsigned int    si_uid;
    void*           si_addr;
    int             si_status;
    union __sigval  si_value;
} __Siginfo;

typedef struct {
    unsigned short row;
    unsigned short col;
    unsigned short xpixels;
    unsigned short ypixels;
} __termWinSize;

extern struct __FD fd;
typedef int __PID;

#define PID __PID
#define SIGINFO __Siginfo


    #define WNOHANG	    1
    #define WUNTRACED	2
    #define WSTOPPED	2
    #define WEXITED		4
    #define WCONTINUED	8
    #define WNOWAIT		0x01000000
    #define __WNOTHREAD	0x20000000
    #define __WALL		0x40000000
    #define __WCLONE	0x80000000

    #define P_ALL		0
    #define P_PID		1
    #define P_PGID		2
    #define P_PIDFD		3


struct __ProcessOption {
    const int EXITED;
    const int STOPPED;
    const int CONTINUED;

};


struct __ProcessType {
    const int ALL;
    const int PID;
    const int GID;

};

struct __Process {
    int (*child) (int);
    int (*parent)(int);
    char** (*environment)(void);
    struct __ProcessType   TYPE;
    struct __ProcessOption OPTION;
};

int _PTYPE_Child(int x);
int _PTYPE_Parent(int x);

extern struct __Process __process;
#define process __process

extern struct __Syscall __syscall;
#define syscall __syscall
#endif

