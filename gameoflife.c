/************************************************************************
**
** NAME:        gameoflife.c
**
** DESCRIPTION: CS61C Fall 2020 Project 1
**
** AUTHOR:      Justin Yokota - Starter Code
**				YOUR NAME HERE
**
**
** DATE:        2020-08-23
**
**************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "imageloader.h"

//Determines what color the cell at the given row/col should be. This function allocates space for a new Color.
//Note that you will need to read the eight neighbors of the cell in question. The grid "wraps", so we treat the top row as adjacent to the bottom row
//and the left column as adjacent to the right column.

// Return the nth bit of x.
// Assume 0 <= n <= 31
unsigned get_bit(unsigned x,
                 unsigned n) {
    unsigned n_mask = 1 << n;
    unsigned masked_x = x & n_mask;
    int res = masked_x >> n;
    return res;
}

// given the current location and the addition to the row and col, find the color of the neighbor
uint8_t find_neighbor_color(Image *image, int row, int col, int row_addition, int col_addition, char color) {
	int neighbor_row = (row + row_addition + image->rows) % image->rows;
	int neighbor_col = (col + col_addition + image->cols) % image->cols;
	if (color == 'R')
	{
		return image->image[neighbor_row][neighbor_col].R;
	} else if (color == 'G')
	{
		return image->image[neighbor_row][neighbor_col].G;
	} else if (color == 'B')
	{
		return image->image[neighbor_row][neighbor_col].B;
	} else {
		fprintf(stderr, "Invalid color");
		exit(-1);
	}
}

// given the neighbor states of a color(e.g. all neighbors' red color states) current state, and the rule, find the next color
uint8_t find_next_color(uint8_t *neighbor_states, uint8_t current_state, uint32_t rule) {
	uint8_t ret_color = 0;
	for (size_t i = 0; i < 8; i++)
	{
		uint8_t neighbor_alive_count = 0;
		for (size_t j = 0; j < 8; j++)
		{
			if (get_bit(neighbor_states[j], i) == 1)
			{
				neighbor_alive_count++;
			}
		}
		if (get_bit(rule, neighbor_alive_count) == 1)
		{
			ret_color = ret_color | (1 << i);
		}
		
	}
	return ret_color;
}

Color *evaluateOneCell(Image *image, int row, int col, uint32_t rule)
{
	//YOUR CODE HERE
	Color *ret_color	=	(Color *) malloc(sizeof(Color));
	ret_color->R 		= 	0;
	ret_color->G 		= 	0;
	ret_color->B 		= 	0;
	// 1. Get my current state for each bit (alive/dead)
	// 2. Get the color of my neighbors
	// 3. for each bit in 24 bits, count the number of alive neighbors bits that are at the location as mine.
	// 4. if the number of alive neighbors is in the rule, then I am alive, otherwise I am dead.
	uint8_t current_red_state = (image->image[row][col]).R;
	uint8_t current_green_state = (image->image[row][col]).G;
	uint8_t current_blue_state = (image->image[row][col]).B;

	uint8_t red_neighbor_states[8];
	uint8_t green_neighbor_states[8];
	uint8_t blue_neighbor_states[8];

	int row_additions[8] = {1, 1, 1, 0, 0, -1, -1, -1};
	int col_additions[8] = {1, 0, -1, 1, -1, 1, 0, -1};
	for (size_t i = 0; i < sizeof(row_additions) / sizeof(row_additions[0]); i++)
	{
		red_neighbor_states[i] = find_neighbor_color(image, row, col, row_additions[i], col_additions[i], 'R');
		green_neighbor_states[i] = find_neighbor_color(image, row, col, row_additions[i], col_additions[i], 'G');
		blue_neighbor_states[i] = find_neighbor_color(image, row, col, row_additions[i], col_additions[i], 'B');
	}
	
	ret_color->R = find_next_color(red_neighbor_states, current_red_state, rule);
	ret_color->G = find_next_color(green_neighbor_states, current_green_state, rule);
	ret_color->B = find_next_color(blue_neighbor_states, current_blue_state, rule);
	

	return ret_color;
}

//The main body of Life; given an image and a rule, computes one iteration of the Game of Life.
//You should be able to copy most of this from steganography.c
Image *life(Image *image, uint32_t rule)
{
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
			Color *ret_color 		=	evaluateOneCell(image, i, j, rule);
			ret_image->image[i][j] 	=	*ret_color;
			free(ret_color);
			ret_color				=	NULL;
		}
	}
	return ret_image;
}

/*
Loads a .ppm from a file, computes the next iteration of the game of life, then prints to stdout the new image.

argc stores the number of arguments.
argv stores a list of arguments. Here is the expected input:
argv[0] will store the name of the program (this happens automatically).
argv[1] should contain a filename, containing a .ppm.
argv[2] should contain a hexadecimal number (such as 0x1808). Note that this will be a string.
You may find the function strtol useful for this conversion.
If the input is not correct, a malloc fails, or any other error occurs, you should exit with code -1.
Otherwise, you should return from main with code 0.
Make sure to free all memory before returning!

You may find it useful to copy the code from steganography.c, to start.
*/

void processCLI(int argc, char **argv, char **filename, uint32_t *rule_ptr) 
{
	if (argc != 3) {
		printf("usage: %s filename rule\n",argv[0]);
		printf("filename is an ASCII PPM file (type P3) with maximum value 255.\n");
		printf("rule is a hex number beginning with 0x; Life is 0x1808.\n");
		exit(-1);
	}
	*filename = argv[1];
	*rule_ptr = strtol(argv[2], NULL, 16);
}

int main(int argc, char **argv)
{
	//YOUR CODE HERE
	char *filename;
	uint32_t *rule_ptr = malloc(sizeof(uint32_t));
	processCLI(argc, argv, &filename, rule_ptr);
	Image *original_image	=	readData(filename);
	Image *result_image		=	life(original_image, *rule_ptr);
	writeData(result_image);
	free(rule_ptr);
	freeImage(original_image);
	freeImage(result_image);
}
