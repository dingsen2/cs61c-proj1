/************************************************************************
**
** NAME:        steganography.c
**
** DESCRIPTION: CS61C Fall 2020 Project 1
**
** AUTHOR:      Dan Garcia  -  University of California at Berkeley
**              Copyright (C) Dan Garcia, 2020. All rights reserved.
**				Justin Yokota - Starter Code
**				YOUR NAME HERE
**
** DATE:        2020-08-23
**
**************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "imageloader.h"

//Determines what color the cell at the given row/col should be. This should not affect Image, and should allocate space for a new Color.
Color *evaluateOnePixel(Image *image, int row, int col)
{
	//YOUR CODE HERE
	Color *ret_color	=	(Color *) malloc(sizeof(Color));
	ret_color->R 		= 	0;
	ret_color->G 		= 	0;
	ret_color->B 		= 	0;

	Color original_color = image->image[row][col];
	uint8_t last_bit = original_color.B & 1;
	if (last_bit == 1)
	{
		ret_color->R = 255;
		ret_color->G = 255;
		ret_color->B = 255;
	}
	return ret_color;
}

//Given an image, creates a new image extracting the LSB of the B channel.
Image *steganography(Image *image)
{
	//YOUR CODE HERE
	
	uint32_t image_row_num	=	image->rows;
	uint32_t image_col_num	=	image->cols;
	
	Image *ret_image		=	malloc(sizeof(Image));
	if (NULL == ret_image)
	{
		fprintf(stderr, "Memory allocation for ret_image failed");
		exit(-1);
	}
	
	ret_image->rows			=	image_row_num;
	ret_image->cols			=	image_col_num;
	ret_image->image		=	(Color **) malloc(image_row_num * sizeof(Color *));

	for (size_t i = 0; i < image_row_num; i++)
	{
		ret_image->image[i] = (Color *) malloc(image_col_num * sizeof(Color));
		if (NULL == ret_image->image[i]) 
		{
			fprintf(stderr, "Memory allocation for image->image[%ld] failed", i);
			for (size_t m = 0; m < i; m++)
			{
				free(ret_image->image[m]);
				ret_image->image[m]	= NULL;
			}
			free(ret_image->image);
			ret_image->image = NULL;
			exit(-1);
		}
		
		for (size_t j = 0; j < image_col_num; j++)
		{
			Color *ret_color 		=	evaluateOnePixel(image, i, j);
			ret_image->image[i][j] 	=	*ret_color;
			free(ret_color);
			ret_color				=	NULL;
		}
	}
	return ret_image;
}

void processCLI(int argc, char **argv, char **filename) 
{
	if (argc != 2) {
		printf("usage: %s filename\n",argv[0]);
		printf("filename is an ASCII PPM file (type P3) with maximum value 255.\n");
		exit(-1);
	}
	*filename = argv[1];
}

/*
Loads a file of ppm P3 format from a file, and prints to stdout (e.g. with printf) a new image, 
where each pixel is black if the LSB of the B channel is 0, 
and white if the LSB of the B channel is 1.

argc stores the number of arguments.
argv stores a list of arguments. Here is the expected input:
argv[0] will store the name of the program (this happens automatically).
argv[1] should contain a filename, containing a file of ppm P3 format (not necessarily with .ppm file extension).
If the input is not correct, a malloc fails, or any other error occurs, you should exit with code -1.
Otherwise, you should return from main with code 0.
Make sure to free all memory before returning!
*/
int main(int argc, char **argv)
{
	//YOUR CODE HERE
	char *filename;
	processCLI(argc, argv, &filename);
	Image *original_image	=	readData(filename);
	Image *result_image		=	steganography(original_image);
	writeData(result_image);
	freeImage(original_image);
	freeImage(result_image);
}
