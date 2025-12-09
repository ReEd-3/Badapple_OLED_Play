#ifndef __OLED_H
#define __OLED_H

#include "main.h"
#include "i2c.h"

#define OLED_I2C_ADDR        0x78 // 地址

#define OLED_WIDTH 128 // 宽度
#define OLED_HEIGHT 64 // 高度
#define OLED_LINES 8 // 行数

#define OLED_CMD 0x00 // 发送命令位置
#define OLED_DATA 0x40 // 发送数据位置
#define HI2C &hi2c1 // i2c句柄

void OLED_Init(); // 初始化
void OLED_Show_On(); // 开启
void OLED_Show_Off();// 关闭
void OLED_Clear(); // 清屏
void OLED_Show_Pix(uint8_t x, uint8_t y); // 显示特定像素
void OLED_Release_Pix(uint8_t x, uint8_t y); // 熄灭特定像素
void OLED_Cmds(uint8_t* cmd, uint16_t size); // 写入指令
void OLED_Data(uint8_t* data, uint16_t size); // 写入数据
void OLED_All_Lit(); // 全部点亮
void OLED_Show_Image(uint8_t* img, uint8_t x);


#endif
