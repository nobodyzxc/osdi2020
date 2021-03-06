#include "string.h"
#include "sprintf.h"
#include "syscall.h"

#define SIGKILL 0x1

#define EQS(xs, ys) (!strcmp(xs, ys))
int exec_cmd(char *cmd, int el) {
  if (EQS("hello", cmd)) {
    println("Hello World!");
  }
  else if (EQS("help", cmd)) {
    println("hello : print Hello World!");
    println("clear : clean the screen");
    println("timestamp : show timestamp");
    println("reboot : reboot the device");
    println("help : print all available commands");
  }
  else if (EQS("clear", cmd)) {
    print("\e[1;1H\e[2J");
  }
  else if (EQS("pid", cmd)) {
    printf("shell pid is [%d]" NEWLINE, call_sys_task_id());
  }
  else if (strbeg(cmd, "kill")) {
    int pid = 0;
    char *p = cmd + 4;
    while(*p == ' ') p++;
    while(*p) pid = pid * 10 + (*p - '0'), p++;
    printf("kill pid %d" NEWLINE, pid);
    call_sys_signal(pid, SIGKILL);
  }
  else if (strbeg(cmd, "pages")) {
    int pid = 0;
    char *p = cmd + 5;
    while(*p == ' ') p++;
    while(*p) pid = pid * 10 + (*p - '0'), p++;
    if(!pid) pid = call_sys_task_id();
    printf("pages of [%d]" NEWLINE, pid);
    call_sys_pages(pid);
  }
  else if (strlen(cmd)) {
    printf("command not found: %s" NEWLINE, cmd);
    return 1;
  }
  return 0;
}

char *read_cmd(char *ptr, char *buffer) {
  printf("# ");
  char *beg = ptr--;
  do {
    *(++ptr) = call_sys_read();
    switch (*ptr) {
      case 4:
        ptr = buffer + 1;
        *ptr = '\r';
        *buffer = 4;
        break;
      case 8:
      case 127:
        ptr--;
        if (ptr >= buffer) {
          ptr--;
          printf("\b \b");
        }
        break;
      case 12:
        *ptr = 0;
        ptr--;
        printf("\e[1;1H\e[2J");
        printf("# ", beg);
        break;
      case 21:
        ptr--;
        while (ptr >= buffer) {
          if (*ptr == '\t')
            printf("\b\b\b\b\b\b");
          else
            printf("\b \b");
          ptr--;
        }
        break;
      default:
        printf("%c", *ptr);
    }

  } while (ptr < buffer || (!strchr("\r", *ptr)));
  while (ptr >= buffer && strchr(" \r\t", *ptr))
    ptr--;
  *(++ptr) = 0;
  printf(NEWLINE);
  while (beg < ptr && strchr(" \r\t\n", *beg))
    beg++;
  return beg;
}

void shloop(){
  char buffer[128];
  while(1) exec_cmd(read_cmd(buffer, buffer), 0);
}
