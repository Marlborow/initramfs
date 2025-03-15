global __syscall_write
global __syscall_read
global __syscall_open
global __syscall_exit
global __syscall_close
global __syscall_fsync
global __syscall_fcntl
global __syscall_chdir

global __syscall_execve

    ;Using unistd fork until
    ;can produce the same result standalone

    ;global __syscall_fork

global __syscall_clone

global __syscall_wait
global __syscall_waitid

global __syscall_dup2
global __syscall_pipe

global __syscall_ioctl

global __syscall_getpid
global __syscall_getppid

global __syscall_mmap
global __syscall_munmap

global __syscall_getcwd
global __syscall_getdents64
global __syscall_brk

global __rawcall


global __syscall_reboot

global __syscall_mkdir
global __syscall_mknod

section .text

__syscall_write:
    mov rax, 1
    syscall
    ret

__syscall_read:
    mov rax, 0
    syscall
    ret

__syscall_open:
    mov rax, 2
    syscall
    ret

__syscall_exit:
    mov rax, 60
    syscall
    ret

__syscall_close:
    mov rax, 3
    syscall
    ret

__syscall_fsync:
    mov rax, 74
    syscall
    ret

__syscall_execve:
    mov rax, 59
    syscall
    ret

    ;__syscall_fork:
    ;    mov rax, 57
    ;    syscall
    ;    ret
__syscall_clone:
    mov rax, 56
    ret

__syscall_mknod:
    mov rax, 133

__syscall_dup2:
    mov rax, 33
    syscall
    ret

__syscall_pipe:
    mov rax, 22
    syscall
    ret

__syscall_chdir:
    mov rax, 80
    syscall
    ret

__syscall_mkdir:
    mov rax, 83
    syscall
    ret

__syscall_wait:
    mov rax, 61
    mov rdi, -1
    syscall
    ret

__syscall_waitid:
    mov rax, 247
    syscall
    ret

__syscall_ioctl:
    mov rax, 16
    mov rdx, rdi
    mov rdi, 1
    mov rsi, 0x5413
    syscall
    ret

__syscall_fcntl:
    mov rax, 72
    syscall
    ret

__syscall_getpid:
    mov rax, 39
    syscall
    ret

__syscall_getppid:
    mov rax, 110
    syscall
    ret


__syscall_mmap:
    mov rax, 9
    syscall
    ret

__syscall_munmap:
    mov rax, 11
    syscall
    ret

__syscall_getcwd:
    mov rax, 79
    syscall
    ret

__syscall_getdents64:
    mov rax, 217
    syscall
    ret

__syscall_brk:
    mov rax, 12
    syscall
    ret

__syscall_reboot:
    mov rax, 169


