
//To compile (win): gcc cbmp.c cellCounter.c -o cellCounter.exe -std=c99
//To run (win): ./cellCounter.exe example.bmp example_result.bmp

#include <stdlib.h>
#include <stdio.h>
#include "cbmp.h"
#include <time.h>
// #include "convertToBinary.c"

#define coordinateSize 1000
#define bit_width 30

#define SetBit(A, x, y) (A[(x / 32)][y] |= (1 << (x % 32)))
#define ClearBit(A, x, y) (A[(x / 32)][y] &= ~(1 << (x % 32)))
#define TestBit(A, x, y) (A[(x / 32)][y] & (1 << (x % 32)))

unsigned char input_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS];
unsigned int binary_image1[bit_width][BMP_HEIGTH];
unsigned int binary_image2[bit_width][BMP_HEIGTH];

int coordinates[coordinateSize][2];

int cellCount = 0;
unsigned char threshold = 95;
unsigned char eroded = 1;
unsigned char flip = 1;

// combines r-g-b pixels to singular gray pixel.
// Then make image binary meaning alle colors are either 0=black or 255=white
void convertToBinary(unsigned char input_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS], unsigned int binary_image1[bit_width][BMP_HEIGTH])
{
    unsigned char tmpGray;
    for (int x = 0; x < BMP_WIDTH; x++)
    {
        for (int y = 0; y < BMP_HEIGTH; y++)
        {
            tmpGray = (input_image[x][y][0] + input_image[x][y][1] + input_image[x][y][2]) / 3;
            if (tmpGray < threshold)
            {
                ClearBit(binary_image1, x, y);
            }
            else
            {
                SetBit(binary_image1, x, y);
            }
        }
    }

    //makes edge black
    for (int x = 0; x < BMP_WIDTH; x++)
    {
        ClearBit(binary_image1, x, 0);
        ClearBit(binary_image1, 0, x);
        ClearBit(binary_image1, x, (BMP_HEIGTH - 1));
        ClearBit(binary_image1, (BMP_HEIGTH - 1), x);
    }
}

void tmpBinaryOut(unsigned int binary_image1[bit_width][BMP_HEIGTH], unsigned char output_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS])
{
    for (int x = 0; x < BMP_WIDTH; x++)
    {
        for (int y = 0; y < BMP_HEIGTH; y++)
        {
            for (int c = 0; c < BMP_CHANNELS; c++)
            {
                output_image[x][y][c] = TestBit(binary_image1, x, y) ? 255 : 0;
            }
        }
    }
}

// take the input image and run though it with a predefined shape removing pixels
// save the new image in another memory slot.
void ErodeImg(unsigned int binary_image1[bit_width][BMP_HEIGTH], unsigned int binary_image2[bit_width][BMP_HEIGTH])
{
    eroded = 0;

    for (int x = 0; x < BMP_WIDTH; x++)
    {
        for (int y = 0; y < BMP_HEIGTH; y++)
        {
            TestBit(binary_image1, x, y) ? SetBit(binary_image2, x, y) : ClearBit(binary_image2, x, y);
        }
    }

    for (int x = 1; x < BMP_WIDTH - 1; x++)
    {
        for (int y = 1; y < BMP_HEIGTH - 1; y++)
        {
            if (TestBit(binary_image1, x, y))
            {
                unsigned char WhiteCounter = 0;

                for (int x1 = x - 1; x1 < x + 1; x1++)
                {
                    if (TestBit(binary_image1, x1, (y - 1)))
                    {
                        WhiteCounter++;
                    }
                    if (TestBit(binary_image1, x1, (y + 1)))
                    {
                        WhiteCounter++;
                    }
                }

                if (TestBit(binary_image1, (x - 1), y))
                {
                    WhiteCounter++;
                }
                if (TestBit(binary_image1, (x + 1), y))
                {
                    WhiteCounter++;
                }

                if (WhiteCounter <= 5)
                {
                    ClearBit(binary_image2, x, y);
                    eroded = 1;
                }
            }
        }
    }

    flip ^= 1;
}

// use capturing area of 12-12 pixels and a 14-14 exclusion frame around, when a cell is detected count it and remeber its
// center (coordinates) and remove the cell from the image.
void DetectSpots(unsigned int binary_image[bit_width][BMP_HEIGTH], int coordinates[coordinateSize][2])
{
    for (int x = 0; x < BMP_WIDTH - 13; x += 2)
    {
        for (int y = 0; y < BMP_HEIGTH - 13; y += 2)
        {
            int whiteFoundEdge = 0;
            for (int x1 = x; x1 < x + 15; x1++)
            {
                if (TestBit(binary_image, x1, y) || TestBit(binary_image, x1, (y + 13)))
                {
                    whiteFoundEdge = 1;
                    break;
                }
            }
            if (!whiteFoundEdge)
            {
                for (int y1 = y; y1 < y + 15; y1++)
                {
                    if (TestBit(binary_image, x, y1) || TestBit(binary_image, (x + 13), y1))
                    {
                        whiteFoundEdge = 1;
                        break;
                    }
                }
            }

            if (!whiteFoundEdge)
            {
                int whiteCellFound = 0;
                for (int x1 = x + 1; x1 < x + 13; x1++)
                {
                    for (int y1 = y + 1; y1 < y + 13; y1++)
                    {
                        if (TestBit(binary_image, x1, y1))
                        {
                            if (!whiteCellFound)
                            {
                                coordinates[cellCount][0] = x + 7;
                                coordinates[cellCount][1] = y + 7;
                                printf("%d, %d\n", coordinates[cellCount][0], coordinates[cellCount][1]);
                                cellCount++;
                                whiteCellFound = 1;
                            }

                            ClearBit(binary_image, x1, y1);
                        }
                    }
                }
            }
        }
    }
}

void makeRedCross(unsigned char output_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS], int x, int y)
{
    int x1 = x - 5;
    int y1 = y - 5;
    while (x1 <= x + 5 && y1 <= y + 5)
    {
        output_image[x1][y + 1][0] = 255;
        output_image[x1][y][0] = 255;
        output_image[x1][y - 1][0] = 255;

        for (int c = 1; c < BMP_CHANNELS; c++)
        {
            output_image[x1][y + 1][c] = 0;
            output_image[x1][y][c] = 0;
            output_image[x1][y - 1][c] = 0;
        }

        if (y1 < y - 1 || y1 > y + 1)
        {
            output_image[x + 1][y1][0] = 255;
            output_image[x][y1][0] = 255;
            output_image[x - 1][y1][0] = 255;

            for (int c = 1; c < BMP_CHANNELS; c++)
            {
                output_image[x + 1][y1][c] = 0;
                output_image[x][y1][c] = 0;
                output_image[x - 1][y1][c] = 0;
            }
        }
        x1++;
        y1++;
    }
}

//take original image and put red x on all coordinates
void constructOutputImg(unsigned char output_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS], int coordinates[coordinateSize][2])
{
    for (int x = 0; x < cellCount; x++)
    {
        makeRedCross(output_image, coordinates[x][0], coordinates[x][1]);
    }
}

void coordinateComparison(int coordinates[coordinateSize][2])
{
    for (int x = 0; x < cellCount; x++)
    {
        for (int x1 = x + 1; x < cellCount; x1++)
        {
            if (coordinates[x][0] == coordinates[x1][0])
            {
                if (coordinates[x][1] == coordinates[x1][1])
                {
                    printf("REEEEEEEE!!!!!: (%d,%d)\n", coordinates[x][0], coordinates[x][1]);
                }
                else
                {
                    printf("yay");
                }
            }
        }
    }
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
    clock_t start, end;
    double cpu_time_used;
    start = clock();
    /* The code that has to be measured. */
    convertToBinary(input_image, binary_image1);

    while (1)
    {
        if (flip)
        {
            ErodeImg(binary_image1, binary_image2);
            if (!eroded)
            {
                break;
            }
            DetectSpots(binary_image2, coordinates);
        }
        else
        {
            ErodeImg(binary_image2, binary_image1);
            if (!eroded)
            {
                break;
            }
            DetectSpots(binary_image1, coordinates);
        }
    }

    // for (int i = 0; i < 40; i++)
    // {
    //     ErodeImg(binary_image);
    //     if (!eroded)
    //     {
    //         break;
    //     }
    //     DetectSpots(binary_image, coordinates);
    // }

    printf("%d\n", cellCount);
    constructOutputImg(input_image, coordinates);
    //tmpBinaryOut(binary_image, input_image);

    end = clock();
    cpu_time_used = end - start;
    printf("Total time: %f ms\n", cpu_time_used * 1000.0 / CLOCKS_PER_SEC);

    //Save image to file
    //and print coordinates and cellcount here
    write_bitmap(input_image, argv[2]);

    //coordinateComparison(coordinates);
    printf("Done!\n");
    return 0;
}
