#include "sched.h"
#include "put.h"
#include "rand.h"

#define offset (0xffffffe000000000 - 0x80000000)

#define Kernel_Page 0x80210000
#define LOW_MEMORY 0x80211000
#define PAGE_SIZE 4096UL

int task_init_done = 0;

struct task_struct *current;
struct task_struct *task[NR_TASKS];
long PRIORITY_INIT_COUNTER[NR_TASKS] = {0, 1, 2, 3, 4};
long COUNTER_INIT_COUNTER[NR_TASKS] = {0, 1, 2, 3, 4};

extern void init_epc(void);
extern void __switch_to(struct task_struct *current, struct task_struct *next);

void task_init(void) { 
  /* your code */
  puts("task init...\n");

  // initialize task[0] and current = task[0]
  current = (struct task_struct*)Kernel_Page;
  current->state = TASK_RUNNING;
  current->counter = PRIORITY_INIT_COUNTER[0];
  current->priority = COUNTER_INIT_COUNTER[0];
  current->blocked = 0;
  current->pid = 0;
  task[0] = current;
  task[0]->thread.sp = (unsigned long long)task[0] + TASK_SIZE;
  task[0]->thread.ra = &init_epc;

  // set other 4 tasks
  // LAB_TEST_NUM 为4
  for (int i = 1; i <= LAB_TEST_NUM; ++i) {
    /* your code */
    task[i] = (struct task_struct*)(Kernel_Page + i * TASK_SIZE);
    task[i]->state = TASK_RUNNING;
    task[i]->counter = PRIORITY_INIT_COUNTER[i];
    task[i]->priority = COUNTER_INIT_COUNTER[i];
    task[i]->blocked = 0;
    task[i]->pid = i;
    task[i]->thread.sp = (unsigned long long)task[i] + TASK_SIZE;
    task[i]->thread.ra = &init_epc;

    puts("[PID =");
    puti(task[i]->pid);
    puts("] Process Create Successfully!\n");
  }
  task_init_done = 1;
}

void do_timer(void) { 
  /* your code */ 

  if (!task_init_done) return;
  puts("[*PID = ");
  puti(current->pid);
  puts("] Context Calculation: counter = ");
  puti(current->counter);
  puts(", priority = ");
  puti(current->priority);
  puts("\n");
  // current process's counter -1, judge whether to schedule or go on.
  /* your code */
  current->counter--;
  if(current->counter <= 0) schedule();
}

void schedule(void) { 
  /* your code */ 
  unsigned char next;
  // int finish = 1;
  int shortest_counter = LAB_TEST_COUNTER+100;// initial as max
  int shortest_pid = 0;
  for(int i=LAB_TEST_NUM;i>=1;i--){
    // if(task[i]->counter > 0) finish = 0;
    if(task[i]->counter > 0 && task[i]->counter < shortest_counter){
      shortest_pid = i;
      shortest_counter = task[i]->counter;
    }
  }

  if(shortest_pid == 0) {
    puts("all is done, reset all.\n");
    for(int i=LAB_TEST_COUNTER; i>=1; i--){
      task[i]->counter = rand();
      task[i]->priority = rand();
      puts("[*PID = ");
      puti(task[i]->pid);
      puts("] Reset: counter = ");
      puti(task[i]->counter);
      puts(", priority = ");
      puti(task[i]->priority);
      puts("\n");
    }
    schedule();
    next = 0;
    return;
  }
  else //next = task[shortest_pid];
    next = shortest_pid;
  
  if (current->pid != task[next]->pid) {
    puts("[ ");
    puti(current->pid);
    puts(" -> ");
    puti(task[next]->pid);
    puts(" ] Switch from task ");
    puti(current->pid);
    puts("["); putullHex(current->thread.sp); 
    puts("] to task "); 
    puti(task[next]->pid);
    puts("["); putullHex(task[next]->thread.sp); 
    puts("], prio: "); 
    puti(task[next]->priority);
    puts(", counter: ");
    puti(task[next]->counter);
    puts("\n");
  }
  switch_to(task[next]);
}

void switch_to(struct task_struct *next) { 
  /* your code */ 
  if(next->pid == current->pid) return;
  else {
    struct task_struct* tmp = current;
    current = next;
    __switch_to(tmp, next);
  }
}

void dead_loop(void) {
  while (1) {
    continue;
  }
}
