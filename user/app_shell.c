/*
 * This app starts a very simple shell and executes some simple commands.
 * The commands are stored in the hostfs_root/shellrc
 * The shell loads the file and executes the command line by line.                 
 */
#include "user_lib.h"
#include "string.h"
#include "util/types.h"

void get_args(char *cmd, char *args, char*exec_cmd) {
  int i = 0 , j = 0;
  while (cmd[i] != ' ' && cmd[i] != '\0') {
    exec_cmd[i] = cmd[i];
    i++;
  }
  if (cmd[i] == '\0') {
    args[0] = '\0';
    return;
  }
  i++;
  j = 0;
  while (cmd[i] != '\0') {
    args[j] = cmd[i];
    i++;
    j++;
  }
  args[j] = '\0';
}


int main(int argc, char *argv[]) {
  printu("\n======== Shell Start ========\n\n");
  int fd;
  int MAXBUF = 1024;
  char cmd[MAXBUF],arg[MAXBUF],exec_cmd[MAXBUF];
  while (1)
  {
    printu("shell> ");
    int n = gets(cmd, MAXBUF);
    if (n == 0) {
      printu("EOF\n");
      break;
    }
    get_args(cmd, arg, exec_cmd);
    printu("exec_cmd: %s\n", cmd);
    if (strncmp(cmd, "exit",4) == 0) {
      printu("exit\n");
      break;
    }
    int pid = fork();
    if(pid == 0) {
      int ret = exec(exec_cmd, arg);;
      if (ret == -1)
      printu("exec failed!\n");
    }
    else
    {
      wait(pid);
      printu("==========Command End============\n\n");
    }
  }
  exit(0);
  return 0;
}
