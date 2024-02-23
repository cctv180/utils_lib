/*
*********************************************************************************************************
*
*    模块名称: utils_lib
*    文件名称: utils_lib.c
*    版   本: V1.1
*    说   明: 实用工具库
*    修改记录:
*               V1.1  2024-02-23    dump_hex改为utils_lib
*                                   增加 mem_fill 使用指定的值填充内存块
*                                   增加 find_PowerOf2 查找与给定数值最接近的2的幂
*
*               V1.0  2023-09-13    首版
*    参考资料:
                C 语言内存 dump 函数
*               https://murphy.tech/posts/de9ea2d9.html
*********************************************************************************************************
*/

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#if 1
#include "bsp.h"
#endif

#include "utils_lib.h"

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
void dump_hex(const void *_buf, uint32_t _size, uint32_t _number)
{
    uint8_t *buffer = (uint8_t *)_buf;
    const char *hex_table = (_number == 32) ? TXT_HEX_32 : ((_number == 8) ? TXT_HEX_8 : (_number = 16, TXT_HEX_16));
    printf("\r\n%s%s%s\r\n", TXT_OFFSET, hex_table, TXT_ASCII); // hex table
    printf("== base address 0x%08X length %d ==\r\n", (uint32_t)buffer, _size);

    for (uint32_t i = 0; i < _size; i += _number)
    {
        printf("%08X: ", i);

        for (uint32_t j = 0; j < _number; j++)
        {
            if (j % 8 == 0)
            {
                printf(" ");
            }
            if (i + j < _size)
            {
                printf("%02X ", buffer[i + j]);
            }
            else
            {
                printf("   ");
            }
        }

        printf(" ");
        for (uint32_t j = 0; j < _number; j++)
        {
            if (i + j < _size)
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
 * @param ptr   指向要填充的内存块的指针。
 * @param value 要填充的值，可以是8位、16位、32位或64位。
 * @param num   要填充值的数量。
 * @param size  要填充值多少个字节。
 */
void mem_fill(void *_ptr, uint64_t _value, size_t _num, size_t _size)
{
    unsigned char *p = (unsigned char *)_ptr; // 转换指针以便逐字节操作

    while (_num-- > 0) // 遍历每个值
    {
        for (size_t j = 0; j < _size; ++j)
        {                                              // 遍历每个值的每个字节
            *p++ = (unsigned char)(_value >> (j * 8)); // 从value中提取字节并填充
        }
    }
}

/**
 * 查找与给定数值最接近的2的幂。
 *
 * @param _num 输入的数值。根据此数值查找最接近的2的幂。
 * @param _findNext 控制查找方向的标志。
 *                 如果为true，查找不小于_num的最小2的幂；
 *                 如果为false，查找小于_num的最大2的幂。
 * @return uint32_t 返回符合条件的2的幂。对于_num为0时，根据_findNext返回0或1。
 */
uint32_t find_PowerOf2(uint32_t _num, _Bool _findNext)
{
    if (_num == 0)
    {
        return _findNext ? 1 : 0; // 对于_num为0，返回0或1取决于_findNext的值。
    }
    if (_num == 1)
    {
        return _findNext ? 1 : 0; // 对于_num为1，寻找下一个2的幂返回1，否则返回0。
    }

    // 如果_num本身是2的幂
    if ((_num & (_num - 1)) == 0)
    {
        return _num;
    }

    // 启用 STM32 硬件提供的计算前导零指令 CLZ
    // return (0x80000000UL >> (__clz(_num) - 1)); //向上取整为2次幂
    // return (0x80000000UL >> __clz(_num)); // 向下取整为2次幂

    _num |= _num >> 1; // 将高位至少保留两位1
    _num |= _num >> 2;
    _num |= _num >> 4;
    _num |= _num >> 8;
    _num |= _num >> 16;
    _num++; // 最终结果是原始值向上取整到的最小2的幂

    return _findNext ? _num : _num >> 1; // 根据_findNext标志返回适当的2的幂
}

#if defined(__SHELL_H__) && DEBUG_MODE == 1

#include <stdlib.h> // 为了使用strtoul函数
// 适配器函数，用于将dump_hex适配为shell命令
int _cmd_dump_hex(int argc, char **argv)
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

    return 0;
}
// 使用SHELL_EXPORT_CMD宏注册命令
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), dump_hex, _cmd_dump_hex, "Dump memory in hex");

// 适配器函数，用于将memset_custom适配为shell命令
int _cmd_memfill(int argc, char **argv)
{
    /* 检查参数数量 */
    if (argc != 5)
    {
        printf("Usage: memfill <address> <value> <num> <byte*x>\r\n");
        return -1;
    }

    /* 解析命令行参数 */
    void *ptr = (void *)strtoul(argv[1], NULL, 0);
    uint64_t value = strtoull(argv[2], NULL, 0);
    size_t num = strtoul(argv[3], NULL, 0);
    size_t size = strtoul(argv[4], NULL, 0);

    mem_fill(ptr, value, num, size); // 调用memset_custom函数

    return 0;
}
// 使用SHELL_EXPORT_CMD宏注册命令
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), mem_fill, _cmd_memfill, "fill memory");

#endif // #if defined(__SHELL_H__) && DEBUG_MODE == 1
