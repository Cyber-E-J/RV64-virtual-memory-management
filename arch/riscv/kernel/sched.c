#include "sched.h"
#include "put.h"
#include "rand.h"

#define offset (0xffffffe000000000 - 0x80000000)

struct task_struct *current;
struct task_struct *task[NR_TASKS];
long PRIORITY_INIT_COUNTER[NR_TASKS] = {0, 1, 2, 3, 4};
long COUNTER_INIT_COUNTER[NR_TASKS] = {0, 1, 2, 3, 4};

extern void init_epc(void);
extern void __switch_to(struct task_struct *current, struct task_struct *next);

void task_init(void) { /* your code */ }

void do_timer(void) { /* your code */ }

void schedule(void) { /* your code */ }

void switch_to(struct task_struct *next) { /* your code */ }

void dead_loop(void) {
  while (1) {
    continue;
  }
}
