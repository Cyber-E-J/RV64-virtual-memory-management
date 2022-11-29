#include "put.h"
#include "sched.h"

int start_kernel() {
  const char *msg = "ZJU OS LAB 4\n"
                    "Student1:3200102523 张景森 Student2:3200104810 周逸杰\n";
  puts(msg);

  // 设置第一次时钟中断
  asm volatile("ecall");

  task_init();
  dead_loop();

  return 0;
}
