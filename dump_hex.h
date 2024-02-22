#ifndef DUMP_HEX_H
#define DUMP_HEX_H

#include <stdint.h>

void dump_hex(const void *buf, uint32_t size, uint32_t number);
void memfill(void *ptr, uint64_t value, size_t num, size_t size);

#endif // DUMP_HEX_H
