#include "oled.h"
#include <string.h>
#include <stdarg.h>
#include <stdio.h>


static uint8_t OLED_Buffer[OLED_LINES][OLED_WIDTH] = {0x00};
static uint8_t lit_data[128];					// 亮屏

uint8_t initcmd1[111] = {
	0xAE,		//关闭显示
	0xD5, 0x80, //显示时钟频率
	0xA8, 0x3F, //复用率（默认值1/64）
	0xD3, 0x00, //显示偏移（设为0）
	0x40,		//显示开始线
	0x8d, 0x14, //开启VCC电源
	0xa1,		//段重定义设置
	0xc8,		//列输出扫描方向：从上到下
	0xda, 0x12, //设置COM硬件引脚配置
	0x81, 0xFF, //设置对比度，0-255
	0xd9, 0x1f, //设置充电周期
	0xdb, 0x40, //设置VCOM电压级别
	0xa4,		//显示模式：正常显示
	0xaf		//关闭显示
};

void OLED_Init(){ // 初始化函数
		memset(lit_data, 0xFF, 128);
		HAL_Delay(100);
    OLED_Show_Off(); // 关闭OLED屏幕
    OLED_Clear();    // 清屏
		OLED_Cmds(initcmd1, 22); //初始化
}

void OLED_Cmds(uint8_t* cmd, uint16_t size){ // 写入多个指令
	HAL_I2C_Mem_Write(HI2C, OLED_I2C_ADDR, OLED_CMD, I2C_MEMADD_SIZE_8BIT, cmd, size, HAL_MAX_DELAY);
}

void OLED_Data(uint8_t *data, uint16_t size) { // 写入数据
    HAL_I2C_Mem_Write(HI2C, OLED_I2C_ADDR, OLED_DATA, I2C_MEMADD_SIZE_8BIT, data, size, HAL_MAX_DELAY);
}

void OLED_Show_On(){
	uint8_t s_on_cmd = 0xAF;
	OLED_Cmds(&s_on_cmd, 1);
}

void OLED_Show_Off(){
	uint8_t s_off_cmd[3] = {
		0xAE,		// 关闭显示
		0x8d,0x10,	// 关闭VCC电源
	};
	OLED_Cmds(s_off_cmd, 3);
}

void OLED_Clear(){ // 清屏
	uint8_t i,j;
	uint8_t clr_cmd[3] = {
		0xB0, // 页地址
		0x00, // 低四位
		0x10, // 高四位
	};
	static uint8_t clr_data[128] = {0x00}; // 清屏
	for(i = 0; i < 8; i++){
		OLED_Cmds(clr_cmd, 3);
		OLED_Data(clr_data, 128);
		clr_cmd[0]++;
	}
	memset(OLED_Buffer, 0x00, sizeof(OLED_Buffer));
}

void OLED_All_Lit(){ // 亮屏
	uint8_t i,j;
	uint8_t lit_cmd[4] = {
		0xB0, // 页地址
		0x00, // 低四位
		0x10, // 高四位
		0xFF,
	};
	for(i = 0; i < 8; i++){
		OLED_Cmds(lit_cmd, 3);
		for(j = 0; j < 128; j++){
			OLED_Data(lit_cmd + 3, 1);
		}
		lit_cmd[0]++;
	}
	memset(OLED_Buffer, 0xFF, sizeof(OLED_Buffer));
}

void OLED_Show_Pix(uint8_t x, uint8_t y){ // 显示特定像素
	uint8_t line = y / 8;
	uint8_t bit_mask = 1 << (y % 8);
	uint8_t pix_cmd[4] = {
		0xB0 + line,	// 页地址
		0x00 | x,			// 低四位
		0x10 | (x >> 4),// 高四位
	};
	OLED_Buffer[line][x] |= bit_mask;
	OLED_Cmds(pix_cmd, 3);
	OLED_Data(&OLED_Buffer[line][x], 1);
}

void OLED_Release_Pix(uint8_t x, uint8_t y){
	uint8_t line = y / 8;
	uint8_t bit_mask = 1 << (y % 8);
	uint8_t pix_cmd[4] = {
		0xB0 + line, // 页地址
		0x00 | x,			// 低四位
		0x10 | (x >> 4),// 高四位
	};
	OLED_Buffer[line][x] &= ~bit_mask;
	OLED_Cmds(pix_cmd, 3);
	OLED_Data(&OLED_Buffer[line][x], 1);
};

void OLED_Show_Image(uint8_t* img, uint8_t wid){
	uint8_t img_cmd[3] = {
		0xB0, // 页地址
		0x06, // 低四位
		0x11, // 高四位
	};
	for(uint8_t i = 0; i < 8; i++){
		OLED_Cmds(img_cmd, 3);
		OLED_Data(&img[i * wid], wid);
		img_cmd[0]++;
	}
}
