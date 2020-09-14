#include <stdlib.h>
#include <stdio.h>
#include "cbmp.h"

// Make image binary meaning alle colors are either 0=black or 1=white
// use threshold aroud 90
void applyBinaryThreshold(unsigned char binary_image[BMP_WIDTH][BMP_HEIGTH])
{
    //makes edge black
    for (int x = 0; x < BMP_WIDTH; x++)
    {
        binary_image[x][0] = 0;
        binary_image[0][x] = 0;
        binary_image[x][BMP_HEIGTH - 1] = 0;
        binary_image[BMP_HEIGTH - 1][x] = 0;
    }

    for (int x = 0; x < BMP_WIDTH; x++)
    {
        for (int y = 0; y < BMP_HEIGTH; y++)
        {
            if (binary_image[x][y] < threshold)
            {
                binary_image[x][y] = 0;
            }
            else
            {
                binary_image[x][y] = 255;
            }
        }
    }
}

// combines r-g-b pixels to singular gray pixel
void convertToBinary(unsigned char input_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS], unsigned char binary_image[BMP_WIDTH][BMP_HEIGTH])
{
    for (int x = 0; x < BMP_WIDTH; x++)
    {
        for (int y = 0; y < BMP_HEIGTH; y++)
        {
            binary_image[x][y] = (input_image[x][y][0] + input_image[x][y][1] + input_image[x][y][2]) / 3;
        }
    }

    applyBinaryThreshold(binary_image);
}
