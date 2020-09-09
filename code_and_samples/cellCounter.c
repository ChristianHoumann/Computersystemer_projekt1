
//To compile (win): gcc cbmp.c cellCounter.c -o cellCounter.exe -std=c99
//To run (win): ./cellCounter.exe example.bmp example_result.bmp

#include <stdlib.h>
#include <stdio.h>
#include "cbmp.h"

//Declaring the array to store the image (unsigned char = unsigned 8 bit)
unsigned char input_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS];
unsigned char output_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS];
unsigned char binary_image[BMP_WIDTH][BMP_HEIGTH];

unsigned char coordinates[420][2];

int cellCount = 0;
int threshold = 90;
int eroded = 1;

// combines r-g-b pixels to singular gray pixel
void convertToGray(unsigned char input_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS], unsigned char binary_image[BMP_WIDTH][BMP_HEIGTH])
{
    for (int x = 0; x < BMP_WIDTH; x++)
    {
        for (int y = 0; y < BMP_HEIGTH; y++)
        {
            binary_image[x][y] = (input_image[x][y][0] + input_image[x][y][1] + input_image[x][y][2]) / 3;
        }
    }
}

// Make image binary meaning alle colors are either 0=black or 1=white
// use threshold aroud 90
void applyBinaryThreshold(unsigned char binary_image[BMP_WIDTH][BMP_HEIGTH], int threshold)
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

void tmpBinaryOut(unsigned char binary_image[BMP_WIDTH][BMP_HEIGTH], unsigned char output_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS])
{
    for (int x = 0; x < BMP_WIDTH; x++)
    {
        for (int y = 0; y < BMP_HEIGTH; y++)
        {
            for (int c = 0; c < BMP_CHANNELS; c++)
            {
                output_image[x][y][c] = binary_image[x][y];
            }
        }
    }
}

// take the input image and run though it with a predefined shape removing pixels
// save the new image in another memory slot.
void ErodeImg(unsigned char binary_image[BMP_WIDTH][BMP_HEIGTH])
{
    eroded = 0;

    unsigned char tmp_image[BMP_WIDTH][BMP_HEIGTH];

    for (int x = 0; x < BMP_WIDTH; x++)
    {
        for (int y = 0; y < BMP_HEIGTH; y++)
        {
            tmp_image[x][y] = binary_image[x][y];
        }
    }

    for (int x = 1; x < BMP_WIDTH - 1; x++)
    {
        for (int y = 1; y < BMP_HEIGTH - 1; y++)
        {
            if (binary_image[x][y] == 255)
            {
                if (binary_image[x + 1][y] == 0 || binary_image[x - 1][y] == 0 || binary_image[x][y + 1] == 0 || binary_image[x][y - 1] == 0)
                {
                    tmp_image[x][y] = 0;
                    eroded = 1;
                }
            }
        }
    }

    for (int x = 0; x < BMP_WIDTH; x++)
    {
        for (int y = 0; y < BMP_HEIGTH; y++)
        {
            binary_image[x][y] = tmp_image[x][y];
        }
    }
}

// use capturing area of 12-12 pixels and a 14-14 exclusion frame around, when a cell is detected count it and remeber its
// center (coordinates) and remove the cell from the image.
void DetectSpots(unsigned char binary_image[BMP_WIDTH][BMP_HEIGTH], unsigned char coordinates[420][2])
{
    for (int x = 0; x < BMP_WIDTH - 13; x++)
    {
        for (int y = 0; y < BMP_HEIGTH - 13; y++)
        {
            int whiteFoundEdge = 0;
            for (int x1 = x; x1 < x + 15; x1++)
            {
                if (binary_image[x1][y] == 255 || binary_image[x1][y + 13] == 255)
                {
                    whiteFoundEdge = 1;
                    break;
                }
            }
            if (!whiteFoundEdge)
            {
                for (int y1 = y; y1 < y + 15; y1++)
                {
                    if (binary_image[x][y1] == 255 || binary_image[x + 13][y1] == 255)
                    {
                        whiteFoundEdge = 1;
                        break;
                    }
                }
            }

            if (!whiteFoundEdge)
            {
                int whiteCellFound = 0;
                for (int x1 = x+1; x1 < x+13; x1++)
                {
                    for (int y1 = y+1; y1 < y+13; y1++)
                    {
                        if (binary_image[x1][y1] == 255)
                        {
                            if (!whiteCellFound) {
                                coordinates[cellCount][0] = x+7;
                                coordinates[cellCount][1] = y+7;
                                printf("%d, %d\n", coordinates[cellCount][0], coordinates[cellCount][1]);
                                cellCount++;
                                whiteCellFound = 1;
                            }
                            
                            binary_image[x1][y1] = 0;
                        }
                        
                    }
                    
                }
                
            }
            
        }
    }
}

//take original image and put red x on all coordinates
void constructOutputImg()
{
}

//Main function
int main(int argc, char **argv)
{
    //argc counts how may arguments are passed
    //argv[0] is a string with the name of the program
    //argv[1] is the first command line argument (input image)
    //argv[2] is the second command line argument (output image)

    //Checking that 2 arguments are passed
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <output file path> <output file path>\n", argv[0]);
        exit(1);
    }

    //Load image from file
    read_bitmap(argv[1], input_image);

    //Run operations here
    convertToGray(input_image, binary_image);
    applyBinaryThreshold(binary_image, threshold);

    while (1)
    {
        ErodeImg(binary_image);
        if (!eroded)
        {
            break;
        }
        DetectSpots(binary_image, coordinates);
    }
    
    /* for (int i = 0; i < 4; i++)
    {
        ErodeImg(binary_image);
        DetectSpots(binary_image, coordinates);
    } */
    



    printf( "%d\n", cellCount);
    tmpBinaryOut(binary_image, output_image);

    //Save image to file
    //and print coordinates and cellcount here
    write_bitmap(output_image, argv[2]);

    printf("Done!\n");
    return 0;
}
