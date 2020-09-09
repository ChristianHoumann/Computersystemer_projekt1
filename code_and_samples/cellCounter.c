
//To compile (win): gcc cbmp.c main.c -o main.exe -std=c99
//To run (win): main.exe example.bmp example_inv.bmp

#include <stdlib.h>
#include <stdio.h>
#include "cbmp.h"

//Declaring the array to store the image (unsigned char = unsigned 8 bit)
unsigned char input_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS];
unsigned char output_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS];

// combines r-g-b pixels to singular gray pixel
void convertToGray() {

}

// Make image binary meaning alle colors are either 0=black or 1=white
// use threshold aroud 90
void applyBinaryThreshold() {

}

// take the input image and run though it with a predefined shape removing pixels
// save the new image in another memory slot.
void ErodeImg() {

}

// use capturing area of 12-12 pixels and a 14-14 exclusion frame around, when a cell is detected count it and remeber its
// center (coordinates) and remove the cell from the image.
void DetectSpots() {

}

//take original image and put red x on all coordinates
void constructOutputImg() {

}

//Main function
int main(int argc, char** argv) {
  //argc counts how may arguments are passed
  //argv[0] is a string with the name of the program
  //argv[1] is the first command line argument (input image)
  //argv[2] is the second command line argument (output image)

  //Checking that 2 arguments are passed
  if (argc != 3) {
      fprintf(stderr, "Usage: %s <output file path> <output file path>\n", argv[0]);
      exit(1);
  }

  //Load image from file
  read_bitmap(argv[1], input_image);

  //Run operations here
  


  //Save image to file
  write_bitmap(output_image, argv[2]);

  printf("Done!\n");
  return 0;
}
