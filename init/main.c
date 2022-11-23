#include "put.h"
#include "sched.h"

int start_kernel() {
  const char *msg = "ZJU OS LAB 4\n"
                    "Student1:123456 张三 Student2:123456 李四\n";
  puts(msg);

  // 设置第一次时钟中断
  asm volatile("ecall");

  task_init();
  dead_loop();

  return 0;
}
