#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <dlfcn.h>
#include <execinfo.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/prctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

extern void* __libc_malloc(long unsigned int size);
extern void __libc_free(void* p);

#define PRINT_MACRO_HELPER(x) #x
#define PRINT_MACRO(x) #x "=" PRINT_MACRO_HELPER(x)  

#ifndef MEM_FILE_NAME_LEN
#define MEM_FILE_NAME_LEN 40
#else
#pragma message(PRINT_MACRO(MEM_FILE_NAME_LEN))
#endif

#ifndef MEM_BACK_TRACE_DEPTH
#define MEM_BACK_TRACE_DEPTH 10
#else
#pragma message(PRINT_MACRO(MEM_BACK_TRACE_DEPTH))
#endif

#ifndef MEM_TRACE_PATH
#define MEM_TRACE_PATH "/tmp/app_mem/"
#else
#pragma message("trace save path defined:" PRINT_MACRO(MEM_TRACE_PATH))
#endif

#ifdef MEM_PRINT_ENABLE
#define MEM_PRINT(a, ...) printf(a, __VA_ARGS__)
#else
#define MEM_PRINT(a, ...)
#endif

#define AUTO_LOCK_ASSIGN(mutex, left_operands, right_operands) \
  do {                                                         \
    pthread_mutex_lock(&mutex);                                \
    (left_operands) = (right_operands);                        \
    pthread_mutex_unlock(&mutex);                              \
  } while (0)

static int sg_enable_malloc_hook = 1;
static int sg_enable_free_hook = 1;
static int sg_init = 0;
static pthread_mutex_t sg_mutex = PTHREAD_MUTEX_INITIALIZER;

void mem_trace_init(void) {
  printf("mem_trace_init...\n");
  if (access(MEM_TRACE_PATH, F_OK) != 0) {
    mkdir(MEM_TRACE_PATH, S_IRWXG);
  }
  AUTO_LOCK_ASSIGN(sg_mutex, sg_init, 1);
}
void mem_trace_uninit(void) { AUTO_LOCK_ASSIGN(sg_mutex, sg_init, 0); }

void* malloc(long unsigned int size) {
  void* array[MEM_BACK_TRACE_DEPTH];
  char** strings;

  int enable = 0;
  AUTO_LOCK_ASSIGN(sg_mutex, enable, sg_enable_malloc_hook);

  if (enable && sg_init) {
    AUTO_LOCK_ASSIGN(sg_mutex, sg_enable_malloc_hook, 0);

    void* return_ptr = __builtin_return_address(0);
    void* p = __libc_malloc(size);

    MEM_PRINT("+%p: addr[%p] size:%lu\n", return_ptr, p, size);

    char file_name[MEM_FILE_NAME_LEN] = {0};
    snprintf(file_name, sizeof(file_name), MEM_TRACE_PATH "%p.trace", p);

    int fd = open(file_name, O_WRONLY | O_CREAT | O_APPEND);

    char format_buffer[100];
    char thread_name[MEM_FILE_NAME_LEN];
    memset(thread_name, 0, sizeof(thread_name));
    prctl(PR_GET_NAME, thread_name);

    snprintf(format_buffer, sizeof(format_buffer),
             "thread_name:[%s] \nmalloc addr +%p: mem:%p size:%lu\n",
             thread_name, return_ptr, p, size);
    write(fd, format_buffer, strlen(format_buffer));

    int size = backtrace(array, MEM_BACK_TRACE_DEPTH);
    backtrace_symbols_fd(array, MEM_BACK_TRACE_DEPTH, fd);

    close(fd);

    AUTO_LOCK_ASSIGN(sg_mutex, sg_enable_malloc_hook, 1);
    return p;
  } else {
    return __libc_malloc(size);
  }
}
void free(void* ptr) {
  int enable = 0;
  AUTO_LOCK_ASSIGN(sg_mutex, enable, sg_enable_malloc_hook);
  if (enable && sg_init) {
    AUTO_LOCK_ASSIGN(sg_mutex, sg_enable_malloc_hook, 0);

    void* return_ptr = __builtin_return_address(0);
    MEM_PRINT("-%p: addr[%p]\n", return_ptr, ptr);

    char file_name[MEM_FILE_NAME_LEN] = {0};
    snprintf(file_name, sizeof(file_name), MEM_TRACE_PATH "%p.trace", ptr);
    if (unlink(file_name) < 0) {
      MEM_PRINT("double free [addr: %p ] ptr:%p\n", return_ptr, ptr);
    }

    __libc_free(ptr);

    AUTO_LOCK_ASSIGN(sg_mutex, sg_enable_malloc_hook, 1);
  } else {
    __libc_free(ptr);
  }
}