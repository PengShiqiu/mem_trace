//c
#include <stdio.h>
//c++
#include <vector>
// mem_trace
#include "mem_trace.h"

int main(void) {
  mem_trace_init();

  int *p = new int;           // 泄露4字节
  std::vector<int> nums(10);  // 无泄露

  int *p1 = new int[128];
  delete p1;  // 无泄露

  int *p2 = new int[10];  // 泄漏40个字节

  return 0;
}