#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  if (argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0; // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if (argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if (argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if (growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  if (argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while (ticks - ticks0 < n)
  {
    if (myproc()->killed)
    {
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  if (argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

// for mp3
uint64
sys_thrdstop(void)
{
  int delay, thrdstop_context_id;
  uint64 handler, handler_arg;
  if (argint(0, &delay) < 0)
    return -1;
  if (argint(1, &thrdstop_context_id) < 0)
    return -1;
  if (argaddr(2, &handler) < 0)
    return -1;
  if (argaddr(3, &handler_arg) < 0)
    return -1;

  struct proc *p = myproc();
  p->sys_call = 1;

  p->delay = delay;
  p->remain_time = delay;
  p->handler = handler;
  p->handler_arg = handler_arg;

  if (thrdstop_context_id == -1)
  {
    for (int i = 0; i < MAX_THRD_NUM; i++)
    {
      if (p->trapframe_used[i] == 0)
      {
        p->save_to = i;
        p->trapframe_used[i] = 1;
        break;
      }
    }
  }
  else
  {
    p->save_to = thrdstop_context_id;
  }

  // printf("thrdstop id: %d %d\n", p->save_to, p->sys_call);

  p->timer = 1;

  return p->save_to;
}

// for mp3
uint64
sys_cancelthrdstop(void)
{
  int thrdstop_context_id, is_exit;
  if (argint(0, &thrdstop_context_id) < 0)
    return -1;
  if (argint(1, &is_exit) < 0)
    return -1;

  struct proc *p = myproc();
  p->timer = 0;
  p->sys_call = 2;

  if (is_exit == 0)
  {
    if (thrdstop_context_id != -1)
    {
      // store context
      p->save_to = thrdstop_context_id;
      // memmove(p->trapframes[thrdstop_context_id], p->trapframe, sizeof(struct trapframe));
      return p->delay - p->remain_time;
    }
    else
    {
      p->save_to = -1;
      // don't save
      return p->delay - p->remain_time;
    }
  }
  else
  {
    // thread exit recycle id
    p->save_to = -1;
    if (thrdstop_context_id != -1)
    {
      p->trapframe_used[thrdstop_context_id] = 0;
      memset(p->trapframes[thrdstop_context_id], 0, sizeof(struct trapframe));
      return p->delay - p->remain_time;
    }
  }

  return 0;
}

// for mp3
uint64
sys_thrdresume(void)
{
  int thrdstop_context_id;
  if (argint(0, &thrdstop_context_id) < 0)
    return -1;

  struct proc *p = myproc();
  p->sys_call = 3;
  p->jump_to = thrdstop_context_id;
  // memmove(p->trapframe, p->trapframes[thrdstop_context_id], sizeof(struct trapframe));
  // printf("pc: %p ra: %p sp: %p\n", p->trapframe->epc, p->trapframe->ra, p->trapframe->sp);
  // p->trapframe->a0 = -1;
  // p->trapframe->a1 = 0;
  return 0;
}
