#ifndef MAIN_H	/******	it prevents the header file include multiple times  */
#define MAIN_H
#include <stdint.h>


typedef struct
{
  uint8_t r;
  uint8_t g;
  uint8_t b;
} rgb;


int gpu_init(void);
void gpu_lsci(uint16_t *buffer);
void colormap_avg_img(int);



#endif

