/*
*********************************************************************************************************
*
*    模块名称: dump_hex
*    文件名称: dump_hex.c
*    版   本: V1.0
*    说   明: 打印十六进制和ASCII表示的数据块
*    修改记录:
*               V1.0  2023-09-13  首版
*    参考资料:
                C 语言内存 dump 函数
*               https://murphy.tech/posts/de9ea2d9.html
*********************************************************************************************************
*/

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "dump_hex.h"

// 定义一个宏，用于检查字符是否可打印
#define IS_PRINT(ch) ((unsigned int)((ch) - ' ') < 127u - ' ')

// 定义常量字符串，提高可读性和可维护性
#define TXT_OFFSET "| Offset |"
#define TXT_HEX_8 " 00 01 02 03 04 05 06 07 "
#define TXT_HEX_16 " 00 01 02 03 04 05 06 07  08 09 0A 0B 0C 0D 0E 0F "
#define TXT_HEX_32 " 00 01 02 03 04 05 06 07  08 09 0A 0B 0C 0D 0E 0F  10 11 12 13 14 15 16 17  18 19 1A 1B 1C 1D 1E 1F "
#define TXT_ASCII "| ASCII |"

/**
 * 打印十六进制和ASCII表示的数据块。
 *
 * @param buf    要打印的数据块的指针
 * @param size   数据块的大小（以字节为单位）
 * @param number 每行打印的字节数（8、16、32等）
 */
void dump_hex(const void *buf, uint32_t size, uint32_t number)
{
    uint8_t *buffer = (uint8_t *)buf;
    const char *hex_table = (number == 32) ? TXT_HEX_32 : ((number == 8) ? TXT_HEX_8 : (number = 16, TXT_HEX_16));
    printf("\r\n%s%s%s\r\n", TXT_OFFSET, hex_table, TXT_ASCII); // hex table
    printf("== base address 0x%08X length %d ==\r\n", (uint32_t)buffer, size);

        for (uint32_t i = 0; i < size; i += number)
    {
        printf("%08X: ", i);

        for (uint32_t j = 0; j < number; j++)
        {
            if (j % 8 == 0)
            {
                printf(" ");
            }
            if (i + j < size)
            {
                printf("%02X ", buffer[i + j]);
            }
            else
            {
                printf("   ");
            }
        }

        printf(" ");
        for (uint32_t j = 0; j < number; j++)
        {
            if (i + j < size)
            {
                printf("%c", IS_PRINT(buffer[i + j]) ? buffer[i + j] : '.');
            }
        }
        printf("\r\n");
    }
}
