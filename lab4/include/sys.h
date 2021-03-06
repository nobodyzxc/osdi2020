#ifndef __SYS_H__
#define __SYS_H__

#define __NR_syscalls  6
#define SYSNUM_READ    0
#define SYSNUM_WRITE   1
#define SYSNUM_EXEC    2
#define SYSNUM_FORK    3
#define SYSNUM_EXIT    4
#define SYSNUM_SIGNAL  5
#define SYSNUM_MUTEX_LOCK    6
#define SYSNUM_MUTEX_UNLOCK    7

#include "task.h"

#ifndef __ASSEMBLER__
int call_sys_read();
void call_sys_write(char *);
int call_sys_exec();
int call_sys_fork();
void call_sys_exit();
void call_sys_signal(unsigned long, int);
int syscall(unsigned int code, long x0, long x1, long x2, long x3, long x4, long x5);
void call_sys_mutex_lock(Mutex *);
void call_sys_mutex_unlock(Mutex *);
#endif

#endif
