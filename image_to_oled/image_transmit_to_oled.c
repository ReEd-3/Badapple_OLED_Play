#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define OLED_WIDTH  84  // 宽度
#define OLED_HEIGHT 63  // 高度    
#define OLED_PAGES  8  // 8行每页

uint8_t* convert_png_to_oled(const char* filename, int threshold) { // 传入图片名字和灰度阈值
    int width, height, channels;    //设定宽度高度和通道
    unsigned char* img_data = stbi_load(filename, &width, &height, &channels, 0); // 加载三通道的png图片

    uint8_t* oled_buffer = (uint8_t*)calloc(OLED_WIDTH * OLED_PAGES, 1); // 存放OLED图片帧数据
    uint8_t* grayscale = (uint8_t*)malloc(OLED_WIDTH * OLED_HEIGHT); // 储存灰度数据

    for(int x = 0; x < OLED_WIDTH; x++){
        for(int y = 0; y < OLED_HEIGHT; y++){
            float raw_x = (float) x * width / OLED_WIDTH;
            float raw_y = (float) y * height / OLED_HEIGHT;
            // 得出四个坐标
            int x_0 = (int) raw_x;
            int y_0 = (int) raw_y;
            int x_1 = x_0 < OLED_WIDTH - 1 ? x_0 + 1 : x_0;
            int y_1 = y_0 < OLED_WIDTH - 1 ? y_0 + 1 : y_0;
            // 决定四个点的权重
            float dx = raw_x - x_0;
            float dy = raw_y - y_0;
            // 存放四个点的灰度值
            float gray00 = 0, gray01 = 0, gray10 = 0, gray11 = 0;
            // 获取灰度
            int idx = (y_0 * width + x_0) * channels;
            gray00 = img_data[idx] * 0.299 + img_data[idx+1] * 0.587 + img_data[idx+2] * 0.114;
            
            idx = (y_0 * width + x_1) * channels;
            gray01 = img_data[idx] * 0.299 + img_data[idx+1] * 0.587 + img_data[idx+2] * 0.114;

            idx = (y_1 * width + x_0) * channels;
            gray10 = img_data[idx] * 0.299 + img_data[idx+1] * 0.587 + img_data[idx+2] * 0.114;

            idx = (y_1 * width + x_1) * channels;
            gray11 = img_data[idx] * 0.299 + img_data[idx+1] * 0.587 + img_data[idx+2] * 0.114;

            float gray = (1-dx)*(1-dy)*gray00 + dx*(1-dy)*gray01 + 
            (1-dx)*dy*gray10 + dx*dy*gray11; // 计算该点最终灰度
            // 存放灰度
           grayscale[y * OLED_WIDTH + x] = (gray > threshold) ? 1 : 0;
        }
    }
    // 打包为OLED格式（垂直位图）
    for (int x = 0; x < OLED_WIDTH; x++) {
        for (int page = 0; page < OLED_PAGES; page++) {
            uint8_t byte_val = 0x00; // 某页某位的数据
            for (int bit = 0; bit < 8; bit++) {
                int y = page * 8 + bit;
                if (y < OLED_HEIGHT && grayscale[y * OLED_WIDTH + x]) {
                    byte_val |= (0x01 << bit);  // bit0对应最上方像素
                }
            }
            oled_buffer[page * OLED_WIDTH + x] = byte_val;
        }
    }
    free(grayscale);
    stbi_image_free(img_data);
    
    return oled_buffer;
}

void bin_create(uint8_t* data, int id){
    char file_name[32];
    sprintf(file_name, "badapple_bin/%d.bin", id);
    FILE* fp = fopen(file_name, "wb");
    fwrite(data, sizeof(uint8_t), OLED_WIDTH * OLED_PAGES, fp);
    fclose(fp);
}

int main(){
    char imgname[32]; // 存放图片名字
    uint8_t* img = (uint8_t*)calloc(OLED_WIDTH * OLED_PAGES, 1); // 储存图片数据
    for(int id = 0; id < 6563; id++){
        sprintf(imgname, "../badapple/%d.png", id);
        img = convert_png_to_oled(imgname, 128);
        // for(int i = 0; i < OLED_WIDTH * OLED_PAGES; i++){
        //     if(i % 84 == 0){
        //         printf("\n");
        //     }
        //     printf("0x%02X, ", img[i]);
        // }
        bin_create(img, id);
    }
}
