/************************************************************************
**
** NAME:        imageloader.c
**
** DESCRIPTION: CS61C Fall 2020 Project 1
**
** AUTHOR:      Dan Garcia  -  University of California at Berkeley
**              Copyright (C) Dan Garcia, 2020. All rights reserved.
**              Justin Yokota - Starter Code
**				YOUR NAME HERE
**
**
** DATE:        2020-08-15
**
**************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include "imageloader.h"

#define LINE_SIZE 4096

//Opens a .ppm P3 image file, and constructs an Image object. 
//You may find the function fscanf useful.
//Make sure that you close the file with fclose before returning.
// void read_one_line(char *line_ptr, FILE *file_ptr, char *msg) {
// 	if (fgets(line_ptr, LINE_SIZE, file_ptr));
// 	{
// 		printf(msg);
// 	}
// }


void print_color(Color color) {
	printf("%3d %3d %3d", color.R, color.G, color.B);
}

// void check_allocation_failed

Image *readData(char *filename) 
{
	//YOUR CODE HERE
	FILE *fp = fopen(filename, "r");
	if (fp == NULL) {
		printf("Error opening file\n");
		exit(1);
	}
	Image *image = malloc(sizeof(Image));
	char ppm_format[3];
    uint32_t col_num, row_num, scale;

    fscanf(fp, "%s", ppm_format);
    // printf("%s\n", ppm_format);
    fscanf(fp, "%d", &col_num);
    fscanf(fp, "%d", &row_num);
	fscanf(fp, "%d", &scale);

    image->cols = col_num;
	image->rows = row_num;
	image->image = (Color **) malloc(row_num * sizeof(Color *));
	if (NULL == image->image)
	{
		fprintf(stderr, "Memory allocation for image->image failed");
		return NULL;
	}
	
    for (size_t i = 0; i < row_num; i++)
    {
		image->image[i] = (Color *) malloc(col_num * sizeof(Color));
        if (NULL == image->image[i])
		{
			fprintf(stderr, "Memory allocation for image->image[%ld] failed", i);
			return NULL;
		}
		
		for (size_t j = 0; j < col_num; j++)
        {
			fscanf(fp, "%hhd", &image->image[i][j].R);
			fscanf(fp, "%hhd", &image->image[i][j].G);
			fscanf(fp, "%hhd", &image->image[i][j].B);
        }
        
    }
	
	fclose(fp);
	return image;
}

//Given an image, prints to stdout (e.g. with printf) a .ppm P3 file with the image's data.
void writeData(Image *image)
{
	printf("P3\n");
	printf("%d %d\n", image->cols, image->rows);
	printf("%d\n", 255);
	for (size_t i = 0; i < image->rows; i++)
	{
		for (size_t j = 0; j < image->cols; j++)
		{
			print_color(image->image[i][j]);
			if (j == image->cols - 1)
			{
				break;
			}
			printf("   ");
		}
		printf("\n");
	}
}

//Frees an image
void freeImage(Image *image)
{
	uint32_t row_num = image->rows;
	uint32_t col_num = image->cols;
	for (size_t i = 0; i < row_num; i++)
	{
		free(image->image[i]);
		image->image[i]=NULL;
	}
	free(image->image);
	image->image = NULL;
	free(image);
	image=NULL;
}