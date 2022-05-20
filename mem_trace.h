#ifndef __MEMERY_HOOK_H_
#define __MEMERY_HOOK_H_

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

void mem_trace_init(void);
void mem_trace_uninit(void);

#ifdef __cplusplus
}
#endif

#if defined(MEM_HOOK_OVERLOAD_NEW_DELETE) && defined(__cplusplus)
#pragma message("c++ new delete 被mem_hook.h重写!!!!")
extern "C++" {
#include <new>
void* operator new(std::size_t size) throw(std::bad_alloc) {
  void* p = ::malloc(size);
  return p;
}
void operator delete(void* ptr) throw() { ::free(ptr); }
}
#endif

#endif