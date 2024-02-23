#ifndef UTILS_LIB_H
#define UTILS_LIB_H

#include <stdint.h>

void dump_hex(const void *_buf, uint32_t _size, uint32_t _number);
void mem_fill(void *_ptr, uint64_t _value, size_t _num, size_t _size);
uint32_t find_PowerOf2(uint32_t _num, _Bool _findNext);

#endif // UTILS_LIB_H
