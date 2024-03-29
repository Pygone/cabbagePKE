/*
 * This app starts a very simple shell and executes some simple commands.
 * The commands are stored in the hostfs_root/shellrc
 * The shell loads the file and executes the command line by line.
 */
#include <sys/_intsup.h>
#include "spike_interface/spike_utils.h"
#include "string.h"
#include "user_lib.h"
#include "util/string.h"
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
int run_help_cmd(char* cmd, char * arg){
    if (strncmp(cmd, "cd", 2) == 0){
        change_cwd(arg);
        return 1;
    }else if (strncmp(cmd, "pwd", 3) == 0){
        char path[30];
        read_cwd(path);
        printu("cwd:%s\n", path);
        return 1;
    } else if (strncmp(cmd, "ls", 2) ==0) {
        if (arg == NULL || strlen(arg) == 0){
            arg = ".";
        }
        int dir_fd = opendir_u(arg);
        struct dir dir;
        while (readdir_u(dir_fd, &dir) ==0) {
            printu("%s\n", dir.name);
        }
        closedir_u(dir_fd);
        return 1;
    }
    return 0 ;
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
        printu("%s\n",exec_cmd);
        if (run_help_cmd(exec_cmd,arg)){
            continue;
        }
        int len = strlen(exec_cmd);
        if (len == 0)continue;
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
