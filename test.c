#include <linux/kernel.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#define __NR_demosystemcall 335

long demosystemcall_syscall(void)
{
 return syscall(__NR_demosystemcall, "Knox Thunderblade");
}

int main(int argc, char *argv[])
{
 long activity;
 activity = demosystemcall_syscall();
if(activity < 0)
 {
    perror("The system call has failed!\n");
 }
else
 {
    printf("Congratulations, Your system call is working!\n");
 }
return 0;
}