/*
 * This app starts a very simple shell and executes some simple commands.
 * The commands are stored in the hostfs_root/shellrc
 * The shell loads the file and executes the command line by line.
 */
#include "string.h"
#include "user_lib.h"
#include "util/types.h"
char env[] = "/:/bin:";
void get_args(char *cmd, char *args, char *exec_cmd)
{
    int i = 0, j = 0;
    while (cmd[i] != ' ' && cmd[i] != '\0' && cmd[i] != '\n')
    {
        exec_cmd[i] = cmd[i];
        i++;
    }
    exec_cmd[i] = '\0';
    if (cmd[i] == '\0' || cmd[i] == '\n')
    {
        args[0] = '\0';
        return;
    }
    i++;
    j = 0;
    while (cmd[i] != '\0' && cmd[i] != '\n')
    {
        args[j] = cmd[i];
        i++;
        j++;
    }
    args[j] = '\0';
}


int main(int argc, char *argv[])
{
    printu("\n======== Shell Start ========\n\n");
    int fd;
    int MAXBUF = 1024;
    // char cmd[MAXBUF],arg[MAXBUF],exec_cmd[MAXBUF];
    char *cmd = naive_malloc();
    char *arg = naive_malloc();
    char *exec_cmd = naive_malloc();
    while (1)
    {
        printu("shell> ");
        int n = gets(cmd, MAXBUF);
        if (n == 0)
        {
            printu("EOF\n");
            break;
        }
        get_args(cmd, arg, exec_cmd);
        if (strncmp(cmd, "exit", 4) == 0)
        {
            printu("exit\n");
            break;
        }
        memset(cmd, 0, MAXBUF);
        int pid = fork();
        if (pid == 0)
        {
            int ret = exec(exec_cmd, arg);
            if (ret == -1)
                printu("exec failed!\n");
        }
        else
        {
            wait(pid);
        }
    }
    exit(0);
    return 0;
}
