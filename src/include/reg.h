#ifndef REG_H
#define REG_H

#include <stdint.h>

typedef struct {
  uint32_t gs, fs, es, ds;
  uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax; // pusha
  uint32_t isr_no, err_c;
  uint32_t eip, cs, eflags, useresp, ss; // automatically pused (useresp is not setuped yet)
} reg_t;

#endif