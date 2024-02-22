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

/**
 * 使用指定的值填充内存块。
 *
 * @param ptr 指向要填充的内存块的指针。
 * @param value 要填充的值，可以是8位、16位、32位或64位。
 * @param num 要填充值的数量。
 * @param size 填充值多少字节。
 * @return void
 */
void memfill(void *ptr, uint64_t value, size_t num, size_t size)
{
    unsigned char *p = (unsigned char *)ptr; // 转换指针以便逐字节操作
    while (num-- > 0)
    { // 遍历每个值
        for (size_t j = 0; j < size; ++j)
        {                                             // 遍历每个值的每个字节
            *p++ = (unsigned char)(value >> (j * 8)); // 从value中提取字节并填充
        }
    }
}

#if 1               // DEBUG_MODE
#include <stdlib.h> // 为了使用strtoul函数
#include "shell.h"  // 确保包含了letter_shell的头文件

// letter_shell命令函数的包装器
int _cmd(int argc, char **argv)
{
    /* 检查参数数量 */
    if (argc != 3)
    {
        printf("Usage: dump_hex <address> <size>\r\n");
        return -1;
    }

    /* 解析命令行参数 */
    uint32_t address = (uint32_t)strtoul(argv[1], NULL, 0);
    uint32_t size = strtoul(argv[2], NULL, 10);

    /* 调用原始的dump_hex函数 */
    dump_hex((const void *)address, size, 16);

    return 0; // 返回0表示成功
}
// 注册命令到letter_shell
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), dump_hex, _cmd, "Dump memory in hex");

// 适配器函数，用于将memset_custom适配为shell命令
int _cmd_memfill(int argc, char **argv)
{
    if (argc != 5)
    {
        printf("Usage: memfill <address> <value> <num> <byte*x>\r\n");
        return -1; // 如果参数数量不正确，返回错误
    }

    void *ptr = (void *)strtoul(argv[1], NULL, 0); // 解析地址
    uint64_t value = strtoull(argv[2], NULL, 0);   // 解析值
    size_t num = strtoul(argv[3], NULL, 0);        // 解析数量
    size_t size = strtoul(argv[4], NULL, 0);       // 解析每个值的大小

    memfill(ptr, value, num, size); // 调用memset_custom函数

    return 0; // 成功返回0
}
// 使用SHELL_EXPORT_CMD宏注册命令
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), memfill, _cmd_memfill, "fill memory");
#endif
