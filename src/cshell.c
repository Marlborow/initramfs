#include "cshell.h"
#include "system/linux/reboot.h"

int __cshell_cd(String directory)
{
    bool isFullPath = (*directory == '/')? false : true;

    if(isFullPath) {
        if(!io.getDirectory(directory)) return -1;
        io.setDirectory(directory);
        return 0;
    }

    String newDir = string.join(io.getCWD(),"/",directory,string.null);
    if(!io.getDirectory(newDir)) return -1;
    io.setDirectory(newDir);
    return 0;

}

void __cshell_run_process(int *arg)
{
    *arg = 1;
     while(true)
    {
        String input = io.print(string.fmt("(%s)$ ", io.getCWD()))->input();
        if(!input || string.compare(input, "\n") == 0) continue;
        if(string.length(input) < 2) continue;

        if(
            string.compare(input, "exit") == 0 ||
            string.compare(input, "exit\n") == 0
        ){
            *arg = 0;
            syscall.exit(0);

        }

        if(string.compare(input,"<3") == 0) {
            io.print(":) <3!\n");
            continue;
        }

        StringList tokens = string.tokenize(input, ' ');
        String* args = tokens.data;

        if(string.compare(args[0],"cd") == 0)
        {
            if(tokens.size < 2) continue;

            if(__cshell_cd(args[tokens.size - 1]) != 0)
                io.print(string.fmt("Failed to find directory '%s'\n", args[tokens.size -1]));

            continue;
        }

        if(!io.executevp(args[0],args))
        {
            syscall.exit(0);
        }
    }
}


void cls()
{
    int termHeight = io.getTermInfo().row;
    for(int i = 0; i > termHeight; ++i)
        io.print("\n");
}


#include <sys/mount.h>
#include <errno.h>

void __cshell_setupfs()
{
    if (mount("none", "/", NULL, MS_REMOUNT | MS_NOATIME, NULL) < 0)
        io.print(string.fmt("Error: Could not remount root filesystem, errno: %d\n", errno))->endl();
    else io.print("Mounted root filesystem")->endl();


    syscall.mkdir("/proc", 0555);
    if (mount("proc", "/proc", "proc", 0, NULL) < 0)
        io.print(string.fmt("Error: Could not mount /proc, errno: %d\n", errno))->endl();
    else io.print("Mounted /proc")->endl();


    syscall.mkdir("/sys", 0555);
    if (mount("sysfs", "/sys", "sysfs", 0, NULL) < 0)
        io.print(string.fmt("Error: Could not mount /sys, errno: %d\n", errno))->endl();
    else io.print("Mounted /sys")->endl();


    syscall.mkdir("/dev", 0755);
    if (mount("dev", "/dev", "tmpfs", 0, NULL) < 0)
        io.print(string.fmt("Error: Could not mount /dev, errno: %d\n", errno))->endl();
    else io.print("Mounted /dev")->endl();

    // Create necessary device nodes
    syscall.mknod("/dev/null", __S_IFCHR | 0666, (1 << 8) | 3);       // Major: 1, Minor: 3
    syscall.mknod("/dev/console", __S_IFCHR | 0622, (5 << 8) | 1);    // Major: 5, Minor: 1

    syscall.mkdir("/bin", 0755);

    static char *env[] = {
        "PATH=/bin:/usr/bin:/sbin:/usr/sbin",
        NULL
    };
    __environ = env;
}

int cshell_run()
{
    cls();
    __cshell_setupfs();

    int process_stat;
    __cshell_run_process(&process_stat);
    return process_stat;
}

__CSHELL shell = {
    .run = cshell_run
};


