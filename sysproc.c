#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

int sys_shutdown(void)
{
  /* Either of the following will work. Does not harm to put them together. */
  outw(0xB004, 0x0|0x2000); // working for old qemu
  outw(0x604, 0x0|0x2000); // working for newer qemu
  
  return 0;
}

extern int sched_trace_enabled;
extern int sched_trace_counter;

int sys_enable_sched_trace(void)
{
  if (argint(0, &sched_trace_enabled) < 0)
  {
    cprintf("enable_sched_trace() failed!\n");
  }
  
  sched_trace_counter = 0;

  return 0;
}
int sys_fork_winner(void){
  int winner;
  argint(0,&winner);
  fork_winner(winner);
  return 0;
}

int sys_set_sched(void){
  int bit;
  argint(0,&bit);
  set_sched(bit);
  return 0;
}

int sys_transfer_tickets(void) {
  int recipient;
  int numTickets;
  argint(0, &recipient);
  argint(1, &numTickets);
  
  if (numTickets < 0) return -1;
  int ticket_p = k_tickets_owned(sys_getpid()); // changed
  if (numTickets > ticket_p - 1) return -2;
  if (findProc(recipient) == NULL) return -3;
  
  transfer_tickets(recipient, numTickets);
  return k_tickets_owned(sys_getpid()); // kernel imp.
}

int sys_tickets_owned(void) {
    int pid;
    if (argint(0, &pid) < 0)
        return -1;
    return k_tickets_owned(pid); // calls kernel imp. after getting arguments
}
