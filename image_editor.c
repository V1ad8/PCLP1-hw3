// Copyright Ungureanu Vlad-Marin 315CAa 2023-2024

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Maximum pixel value in the image
#define MAX_VALUE 255

// Maximum length of a file name
#define FILE_NAME_LENGTH 100

// Maximum length of an input line
#define MAX_INPUT_LINE_LENGTH 1000

// Maximum number of digits in a numerical parameter
#define MAX_NUMBER_SIZE 5

// Maximum length of a command (e.g., "LOAD", "SAVE")
#define MAX_COMMAND_LENGTH 11

// Custom boolean type for improved readability
typedef enum { false, true } bool;

// Structure representing a pixel in an image
typedef struct pixel_t {
	unsigned short red; // Red channel intensity (0 to 255)
	unsigned short green; // Green channel intensity (0 to 255)
	unsigned short blue; // Blue channel intensity (0 to 255)
} pixel_t;

// Structure representing an image
typedef struct image_t {
	pixel_t **picture; // 2D array representing the image pixels
	bool color; // Flag indicating whether the image is color or grayscale
	unsigned short height; // Height of the image in pixels
	unsigned short width; // Width of the image in pixels
} image_t;

// Structure representing an area within an image
typedef struct area_t {
	bool all; // Flag indicating whether the entire image is selected
	unsigned short column_start; // Starting column index of the selected area
	unsigned short line_start; // Starting line index of the selected area
	unsigned short
	    column_end; // Ending column index (exclusive) of the selected area
	unsigned short
	    line_end; // Ending line index (exclusive) of the selected area
} area_t;

// Function to round a double to a signed short
//
// Parameters:
//	 - value: The double value to be rounded
//
// Returns:
//	 - The rounded value as a signed short
signed short round_double(double value)
{
	// Add 0.5 to the double value and cast the result to a signed short
	// The addition of 0.5 ensures proper rounding
	return (signed short)(value + 0.5);
}

// Function to clamp a value between 0 and 255
//
// Parameters:
//	 - number: The value to be clamped
//
// Returns:
//	 - The clamped value between 0 and 255
unsigned short clamp(signed short number)
{
	// If the number is less than or equal to 0, return 0
	if (number <= 0)
		return 0;

	// If the number is greater than or equal to 255, return 255
	if (number >= MAX_VALUE)
		return MAX_VALUE;

	// Otherwise, return the original number
	return number;
}

// Function to free memory allocated for an image
//
// Parameters:
//	 - image: Pointer to the array of pointers to rows
//	 - number_of_lines: Number of rows in the image
void free_picture(pixel_t ***image, unsigned short number_of_lines)
{
	unsigned short line;

	// Free memory for each row of pixels
	for (line = 0; line < number_of_lines; line++)
		free((*image)[line]);

	// Free memory for the array of pointers to rows
	free(*image);

	// Set the pointer to NULL to avoid using a dangling pointer
	*image = NULL;
}

// Function to create an empty picture
//
// Parameters:
//	 - height: Height of the picture (number of rows)
//	 - width: Width of the picture (number of columns)
//
// Returns:
//	 - Pointer to the newly created picture or NULL if allocation fails
pixel_t **create_picture(unsigned short height, unsigned short width)
{
	// Allocate memory for the array of pointers to rows
	pixel_t **new_picture = (pixel_t **)malloc(height * sizeof(pixel_t *));

	// Check if memory allocation was successful
	if (!new_picture)
		return NULL;

	unsigned short line = 0;

	// Allocate memory for each row (array of pixels)
	for (line = 0; line < height; line++) {
		new_picture[line] = (pixel_t *)malloc(width * sizeof(pixel_t));

		// Check if memory allocation for the row was successful
		if (!new_picture[line]) {
			// Free memory for previously allocated rows
			free_picture(&new_picture, line);
			return NULL;
		}
	}

	// Return the pointer to the newly created picture
	return new_picture;
}

// Function to skip comments in the header of a file
//
// Parameters:
//   - file: Pointer to the FILE structure representing the open file
void skip_comments(FILE *file)
{
	char check, comment[MAX_INPUT_LINE_LENGTH + 1];

	// Read the first character
	fscanf(file, "%c", &check);

	// Continue reading comments while the line starts with '#'
	while (check == '#') {
		// Read and discard the entire comment line
		fgets(comment, MAX_INPUT_LINE_LENGTH, file);

		// Read the next character after the comment
		fscanf(file, "%c", &check);
	}

	// Unread the character after the last comment
	ungetc(check, file);
}

// Function to read grayscale pixels from a P2 (ASCII) image file
//
// Parameters:
//   - file: Pointer to the FILE structure representing the open file
//   - image: Pointer to the image structure to store the pixel data
//   - max_value: Maximum pixel value specified in the image file
void read_P2(FILE *file, image_t *image, unsigned short max_value)
{
	unsigned short line, column, value;

	for (line = 0; line < image->height; line++) {
		for (column = 0; column < image->width; column++) {
			// Read pixel value from the file
			fscanf(file, "%hd", &value);

			// Calculate and store the red, green, and blue values
			image->picture[line][column].red =
				clamp(round_double((value * MAX_VALUE * 1.) / max_value));
			image->picture[line][column].green =
			    image->picture[line][column].red;
			image->picture[line][column].blue =
			    image->picture[line][column].red;
		}
	}
}

// Function to read color pixels from a P3 (ASCII) image file
//
// Parameters:
//   - file: Pointer to the FILE structure representing the open file
//   - image: Pointer to the image structure to store the pixel data
//   - max_value: Maximum pixel value specified in the image file
void read_P3(FILE *file, image_t *image, unsigned short max_value)
{
	unsigned short line, column, value;

	for (line = 0; line < image->height; line++) {
		for (column = 0; column < image->width; column++) {
			// Read red, green, and blue values from the file
			fscanf(file, "%hd", &value);
			image->picture[line][column].red =
				clamp(round_double((value * MAX_VALUE * 1.) / max_value));

			fscanf(file, "%hd", &value);
			image->picture[line][column].green =
				clamp(round_double((value * MAX_VALUE * 1.) / max_value));

			fscanf(file, "%hd", &value);
			image->picture[line][column].blue =
				clamp(round_double((value * MAX_VALUE * 1.) / max_value));
		}
	}
}

// Function to read grayscale pixels from a P5 (binary) image file
//
// Parameters:
//   - file: Pointer to the FILE structure representing the open file
//   - image: Pointer to the image structure to store the pixel data
//   - max_value: Maximum pixel value specified in the image file
void read_P5(FILE *file, image_t *image, unsigned short max_value)
{
	unsigned short line, column, value;
	unsigned char byte;

	for (line = 0; line < image->height; line++) {
		for (column = 0; column < image->width; column++) {
			// Read a byte from the file
			fread(&byte, 1, 1, file);

			// Calculate and store the grayscale pixel value
			value =
				clamp(round_double((byte * MAX_VALUE * 1.) / max_value));

			image->picture[line][column].red = value;
			image->picture[line][column].green = value;
			image->picture[line][column].blue = value;
		}
	}
}

// Function to read color pixels from a P6 (binary) image file
//
// Parameters:
//   - file: Pointer to the FILE structure representing the open file
//   - image: Pointer to the image structure to store the pixel data
//   - max_value: Maximum pixel value specified in the image file
void read_P6(FILE *file, image_t *image, unsigned short max_value)
{
	unsigned short line, column;
	unsigned char byte;

	for (line = 0; line < image->height; line++) {
		for (column = 0; column < image->width; column++) {
			// Read red, green, and blue values from the file
			fread(&byte, 1, 1, file);
			image->picture[line][column].red =
				clamp(round_double((byte * MAX_VALUE * 1.) / max_value));

			fread(&byte, 1, 1, file);
			image->picture[line][column].green =
				clamp(round_double((byte * MAX_VALUE * 1.) / max_value));

			fread(&byte, 1, 1, file);
			image->picture[line][column].blue =
				clamp(round_double((byte * MAX_VALUE * 1.) / max_value));
		}
	}
}

// Function to read the magic number and determine the color type of the image
//
// Parameters:
//   - file: Pointer to the FILE structure representing the open file
//   - image: Pointer to the image structure to store the image data
//
// Returns:
//   - true if the image is successfully read, false otherwise
bool read_image(FILE *file, image_t *image)
{
	unsigned short magic_number, max_value;
	char residual;

	// Skip comments in the header
	skip_comments(file);

	// Read the magic number
	fscanf(file, "P%hd", &magic_number);

	// Set the color type based on the magic number
	if (magic_number == 2 || magic_number == 5)
		image->color = false; // Grayscale
	else if (magic_number == 3 || magic_number == 6)
		image->color = true; // Color
	else
		return false; // Unsupported magic number

	fscanf(file, "%c", &residual);

	// Skip comments in the header
	skip_comments(file);

	// Read image dimensions
	fscanf(file, "%hd %hd", &image->width, &image->height);

	// Skip comments in the header
	skip_comments(file);

	// Read the maximum pixel value
	fscanf(file, "%hd", &max_value);

	fscanf(file, "%c", &residual);

	// Allocate memory for the image pixels
	image->picture = create_picture(image->height, image->width);
	if (!image->picture)
		return false; // Memory allocation failed

	// Read pixels based on the magic number
	switch (magic_number) {
	case 2:
		read_P2(file, image, max_value);
		return true;

	case 3:
		read_P3(file, image, max_value);
		return true;

	case 5:
		read_P5(file, image, max_value);
		return true;

	case 6:
		read_P6(file, image, max_value);
		return true;

	default:
		return false; // Unsupported magic number
	}
}

// Function to load an image from a file and set the initial selection area
//
// Parameters:
//   - file_name: The name of the file to load
//   - selection: Pointer to the area structure to be set based on the
//                loaded image
//
// Returns:
//   - An image_t structure representing the loaded image or
//     an empty_image structure if the load fails
image_t load_image(char file_name[FILE_NAME_LENGTH])
{
	// Open the file for reading
	FILE *file = fopen(file_name, "r");

	// Create an empty image structure
	image_t empty_image;
	empty_image.picture = NULL;

	// Check if the file opened successfully
	if (!file) {
		printf("Failed to load %s\n", file_name);
		return empty_image;
	}

	image_t image;

	// Attempt to read the image from the file
	if (!read_image(file, &image)) {
		printf("Failed to load %s\n", file_name);
		fclose(file);
		return empty_image;
	}

	// Print a success message
	printf("Loaded %s\n", file_name);

	// Close the file
	fclose(file);

	// Return the loaded image
	return image;
}

// Function to handle the "LOAD" command, loading an image from a file and
// setting the initial selection area
//
// Parameters:
//   - image: Pointer to the image structure to be updated with the loaded image
//   - selection: Pointer to the area structure to be set based on the
//				  loaded image
//   - file_name: The name of the file to load
void load_command(image_t *image, area_t *selection,
				  char file_name[FILE_NAME_LENGTH])
{
	// Free existing image if it exists
	if (image->picture)
		free_picture(&image->picture, image->height);

	// Load the image from the file
	*image = load_image(file_name);

	// Set the selection area to cover the entire image if the image is loaded
	if (image->picture) {
		selection->all = true;
		selection->column_start = 0;
		selection->column_end = image->width;
		selection->line_start = 0;
		selection->line_end = image->height;
	}
}

// Function to select an area within the image
//
// Parameters:
//	 - selection: The current area selection
//	 - image: The image to select an area from
//	 - first_value, second_value, third_value, fourth_value:
//					Coordinates of the selected area
//
// Returns:
//	 - The updated area selection based on the input coordinates
area_t area_select(area_t selection, image_t image, signed short first_value,
				   signed short second_value, signed short third_value,
				   signed short fourth_value)
{
	// Check if the coordinates are valid
	if ((first_value < 0 || first_value > image.width) ||
	    (second_value < 0 || second_value > image.height) ||
	    (third_value < 0 || third_value > image.width) ||
	    (fourth_value < 0 || fourth_value > image.height) ||
	    (first_value == third_value || second_value == fourth_value)) {
		// Print error message and return the original selection
		printf("Invalid set of coordinates\n");
		return selection;
	}

	// Create a new area structure to store the selected area
	area_t new_area;

	// Determine the column start and end based on the input coordinates
	if (first_value < third_value) {
		new_area.column_start = first_value;
		new_area.column_end = third_value;
	} else {
		new_area.column_start = third_value;
		new_area.column_end = first_value;
	}

	// Determine the line start and end based on the input coordinates
	if (second_value < fourth_value) {
		new_area.line_start = second_value;
		new_area.line_end = fourth_value;
	} else {
		new_area.line_start = fourth_value;
		new_area.line_end = second_value;
	}

	// Update the 'all' flag based on the selected area
	new_area.all =
	    (!new_area.line_start && new_area.line_end == image.height &&
	     !new_area.column_start && new_area.column_end == image.width);

	// Print a message indicating the selected area
	printf("Selected %hd %hd %hd %hd\n", new_area.column_start,
	       new_area.line_start, new_area.column_end, new_area.line_end);

	// Return the newly selected area
	return new_area;
}

// Function to handle the "SELECT" command, updating the selection area
//
// Parameters:
//   - image: Image structure containing the loaded image data
//   - selection: Pointer to the area structure representing the current
//				  selection
//   - parameter_1: First parameter of the SELECT command
//   - parameter_2: Second parameter of the SELECT command
//   - parameter_3: Third parameter of the SELECT command
//   - parameter_4: Fourth parameter of the SELECT command
//   - parameter_5: Fifth parameter of the SELECT command
void select_command(image_t image, area_t *selection,
					char parameter_1[MAX_INPUT_LINE_LENGTH],
					char parameter_2[MAX_NUMBER_SIZE + 1],
					char parameter_3[MAX_NUMBER_SIZE + 1],
					char parameter_4[MAX_NUMBER_SIZE + 1], char parameter_5[1])
{
	// Check if an image is loaded
	if (!image.picture) {
		printf("No image loaded\n");
		return;
	}

	// Check for the "ALL" option
	if (!strcmp(parameter_1, "ALL") && !strlen(parameter_2)) {
		// Select the entire image
		selection->all = true;
		selection->line_start = 0;
		selection->column_start = 0;
		selection->line_end = image.height;
		selection->column_end = image.width;

		printf("Selected ALL\n");
		return;
	}

	// Check for valid parameters and update the selection area
	if (strlen(parameter_1) && strlen(parameter_2) && strlen(parameter_3) &&
	    strlen(parameter_4) && !strlen(parameter_5)) {
		// Check for invalid parameters
		if ((!atoi(parameter_1) && parameter_1[0] != '0') ||
		    (!atoi(parameter_2) && parameter_2[0] != '0') ||
		    (!atoi(parameter_3) && parameter_3[0] != '0') ||
		    (!atoi(parameter_4) && parameter_4[0] != '0')) {
			printf("Invalid command\n");
			return;
		}

		// Update the selection area
		*selection = area_select(*selection, image, atoi(parameter_1),
								 atoi(parameter_2), atoi(parameter_3),
								 atoi(parameter_4));
		return;
	}

	// Invalid command if none of the conditions are met
	printf("Invalid command\n");
}

// Function to print stars representing a histogram bin
//
// Parameters:
//	 - number: The number of stars to print
void print_stars(unsigned short number)
{
	// Print the bin number followed by a tab, a pipe symbol and another tab
	printf("%hd\t|\t", number);

	// Print the stars representing the bin count
	unsigned short index;
	for (index = 0; index < number; index++)
		printf("*");

	// Move to the next line for the next bin
	printf("\n");
}

// Function that constructs and prints a histogram for an image, focusing on
// the red channel for simplicity since all color channels are identical
//
// Parameters:
//	 - image: The image to analyze
//	 - number_of_stars: The maximum number of stars to use for representing
//						each bin
//	 - number_of_bins: The number of bins in the histogram
void make_histogram(image_t image, short number_of_stars, short number_of_bins)
{
	// Calculate the step size for each histogram bin
	unsigned short step = (MAX_VALUE + 1) / number_of_bins;

	// Array to store the frequency of values in each bin
	unsigned long frequency[MAX_VALUE + 1];

	unsigned short value, line, column, previous_value = 0, index;

	// Iterate over each bin
	for (index = 0; index < number_of_bins; index++) {
		// Determine the upper value for the current bin
		value = (index + 1) * step;

		// Initialize frequency for the current bin
		frequency[index] = 0;

		// Iterate over each pixel in the image
		for (line = 0; line < image.height; line++) {
			for (column = 0; column < image.width; column++) {
				// Check if the red channel value falls within the current bin
				if (image.picture[line][column].red >=
					previous_value &&
				    image.picture[line][column].red < value) {
					frequency[index]++;
				}
			}
		}
		previous_value = value;
	}

	// Find the maximum frequency to normalize the histogram
	unsigned long max_value = frequency[0];
	for (index = 1; index < number_of_bins; index++) {
		if (max_value < frequency[index])
			max_value = frequency[index];
	}

	// Print stars for each bin, scaled based on their frequency
	for (index = 0; index < number_of_bins; index++) {
		print_stars((1. * frequency[index]) / (1. * max_value) *
			    (1. * number_of_stars));
	}
}

// Function to handle the "HISTOGRAM" command, generating a histogram for the
// specified channel of the image
//
// Parameters:
//   - image: Image structure containing the loaded image data
//   - parameter_1: First parameter of the HISTOGRAM command
//   - parameter_2: Second parameter of the HISTOGRAM command
//   - parameter_3: Third parameter of the HISTOGRAM command
void histogram_command(image_t image, char parameter_1[MAX_INPUT_LINE_LENGTH],
					   char parameter_2[MAX_NUMBER_SIZE + 1],
				       char parameter_3[MAX_NUMBER_SIZE + 1])
{
	// Check if an image is loaded
	if (!image.picture)
		printf("No image loaded\n");

	// Check for the correct number of parameters
	else if (strlen(parameter_1) && strlen(parameter_2) &&
			 !strlen(parameter_3)) {
		// Check if the image is a color image
		if (image.color) {
			printf("Black and white image needed\n");
		} else {
			// Generate and display the histogram for the specified channel
			make_histogram(image, atoi(parameter_1),
						   atoi(parameter_2));
		}
	} else {
		// Print an error message for an invalid command
		printf("Invalid command\n");
	}
}

// Function to perform histogram equalization on an image
//
// Parameters:
//   - image: Pointer to the image structure to be modified
void equalize(image_t *image)
{
	// Initialize arrays to store frequency and cumulative distribution
	unsigned int frequency[MAX_VALUE + 1] = { 0 };
	double cumulative_distribution[MAX_VALUE + 1] = { 0 };
	unsigned short index, line, column;

	// Calculate frequency of each intensity level
	for (line = 0; line < image->height; line++)
		for (column = 0; column < image->width; column++)
			frequency[image->picture[line][column].red]++;

	// Calculate cumulative distribution function
	cumulative_distribution[0] =
	    (double)frequency[0] / (image->height * image->width);
	for (index = 1; index <= MAX_VALUE; index++) {
		cumulative_distribution[index] =
		    cumulative_distribution[index - 1] +
		    (double)frequency[index] / (image->height * image->width);
	}

	// Perform histogram equalization
	for (line = 0; line < image->height; line++) {
		for (column = 0; column < image->width; column++) {
			double result =
			    cumulative_distribution[image->picture[line][column]
							.red] *
			    MAX_VALUE;

			// Update pixel values after equalization
			image->picture[line][column].red =
			    clamp(round_double(result));
			image->picture[line][column].green =
			    image->picture[line][column].red;
			image->picture[line][column].blue =
			    image->picture[line][column].red;
		}
	}

	// Print a message indicating the completion of equalization
	printf("Equalize done\n");
}

// Function to rotate a selected area within an image
//
// Parameters:
//	 - image: Pointer to the image structure to be rotated
//	 - selection: The selected area to be rotated
//	 - angle: The angle (in degrees) by which the area should be rotated
void rotate_area(image_t *image, area_t selection, signed short angle)
{
	// Adjust negative angles
	signed short flip = angle;
	if (flip < 0)
		flip += 360;

	// Determine the number of 90-degree rotations needed
	flip = (flip / 90) % 4;

	// No rotation needed for multiples of 360 degrees
	if (!flip) {
		printf("Rotated %hd\n", angle);
		return;
	}

	// Create a copy of the selected area
	pixel_t **copy =
	    create_picture((selection.line_end - selection.line_start),
					   (selection.column_end - selection.column_start));

	if (!copy)
		return;

	// Perform the specified number of 90-degree rotations
	short line, column, index;
	for (index = flip; index != 0; index--) {
		for (line = 0; line < selection.line_end - selection.line_start;
		     line++) {
			for (column = 0; column < selection.column_end -
						      selection.column_start;
			     column++) {
				// Rotate the pixels
				copy[line][column] =
				    image->picture[selection.line_end - 1 -
						   column]
						  [line +
						   selection.column_start];
			}
		}

		// Copy the rotated pixels back to the original location
		for (line = 0; line < selection.line_end - selection.line_start;
		     line++) {
			for (column = 0; column < selection.column_end -
						      selection.column_start;
			     column++) {
				image->picture[line + selection.line_start]
					      [column + selection.column_start] =
				    copy[line][column];
			}
		}
	}

	// Free memory used for the copy
	free_picture(&copy, (selection.line_end - selection.line_start));

	// Print a message indicating the completion of rotation
	printf("Rotated %hd\n", angle);
}

// Function to rotate the entire image
//
// Parameters:
//	 - image: Pointer to the image structure to be rotated
//	 - angle: The angle (in degrees) by which the image should be rotated
void rotate_all(image_t *image, signed short angle)
{
	// Adjust negative angles
	signed short flip = angle;
	if (flip < 0)
		flip += 360;

	// Determine the number of 90-degree rotations needed
	flip = (flip / 90) % 4;

	// No rotation needed for multiples of 360 degrees
	if (!flip) {
		printf("Rotated %hd\n", angle);
		return;
	}

	pixel_t **copy;

	unsigned short index, auxiliary, line, column;
	for (index = flip; index != 0; index--) {
		// Create a copy of the image
		copy = create_picture(image->width, image->height);
		if (!copy)
			return;

		// Perform the 90-degree rotation
		for (line = 0; line < image->width; line++) {
			for (column = 0; column < image->height; column++) {
				copy[line][column] =
				    image->picture[image->height - column - 1]
						  [line];
			}
		}

		// Free memory used by the original image
		free_picture(&image->picture, image->height);

		// Swap the image dimensions
		auxiliary = image->height;
		image->height = image->width;
		image->width = auxiliary;

		// Allocate memory for the rotated image
		image->picture = create_picture(image->height, image->width);
		if (!image->picture)
			return;

		// Copy the rotated pixels back to the image structure
		for (line = 0; line < image->height; line++)
			for (column = 0; column < image->width; column++)
				image->picture[line][column] =
				    copy[line][column];

		// Free memory used by the copy
		free_picture(&copy, image->height);
	}

	// Print a message indicating the completion of rotation
	printf("Rotated %hd\n", angle);
}

// Function to handle the "ROTATE" command, rotating the image or a selected
// area
//
// Parameters:
//   - image: Pointer to the image structure to be modified
//   - selection: Area selection structure specifying the region to rotate
//   - parameter_1: Angle parameter of the ROTATE command
void rotate_command(image_t *image, area_t selection,
					char parameter_1[MAX_NUMBER_SIZE + 1])
{
	// Check if the angle parameter is missing
	if (!strlen(parameter_1)) {
		printf("Invalid command\n");
		return;
	}

	// Convert the angle parameter to a signed short
	signed short angle = atoi(parameter_1);

	// Check if an image is loaded
	if (!image->picture) {
		printf("No image loaded\n");
		return;
	}

	// Check if the rotation angle is supported
	if (angle != -360 && angle != -270 && angle != -180 && angle != -90 &&
	    angle && angle != 90 && angle != 180 && angle != 270 &&
	    angle != 360) {
		printf("Unsupported rotation angle\n");
		return;
	}

	// Check if the rotation is applied to the entire image or a selected area
	if (selection.all) {
		// Rotate the entire image
		rotate_all(image, angle);
	} else {
		// Check if the selected area is square
		if (selection.line_end - selection.line_start !=
		    selection.column_end - selection.column_start) {
			printf("The selection must be square\n");
			return;
		}

		// Rotate the selected area
		rotate_area(image, selection, angle);
	}
}

// Function to crop the image based on the specified selection area
//
// Parameters:
//	 - image: Pointer to the image structure to be cropped
//	 - selection: Pointer to the area selection structure specifying the
//                crop region
void crop(image_t *image, area_t *selection)
{
	// Create a copy of the selected area
	pixel_t **copy =
	    create_picture((selection->line_end - selection->line_start),
					   (selection->column_end - selection->column_start));

	// Check if memory allocation for the copy was successful
	if (!copy)
		return;

	short line, column;
	// Copy pixels from the selected area to the copy
	for (line = 0; line < selection->line_end - selection->line_start;
	     line++) {
		for (column = 0;
		     column < selection->column_end - selection->column_start;
		     column++) {
			copy[line][column] =
			    image->picture[line + selection->line_start]
					  [column + selection->column_start];
		}
	}

	// Free memory used by the original image
	free_picture(&image->picture, image->height);

	// Update the image structure with the cropped image
	image->picture = copy;
	image->height = selection->line_end - selection->line_start;
	image->width = selection->column_end - selection->column_start;

	// Update the selection area to cover the entire cropped image
	selection->all = true;
	selection->line_start = 0;
	selection->line_end = image->height;
	selection->column_start = 0;
	selection->column_end = image->width;

	// Print a message indicating the completion of cropping
	printf("Image cropped\n");
}

// Function to apply an edge filter to the specified area of the image
//
// Parameters:
//	 - image: Pointer to the image structure to be modified
//	 - selection: Area selection structure specifying the region to apply
//				  the edge filter
//
// Returns:
//   - A dynamically allocated copy of the image with the edge filter applied
pixel_t **apply_edge(image_t image, area_t selection)
{
	// Create a copy of the image
	pixel_t **copy = create_picture(image.height, image.width);

	// Check if memory allocation for the copy was successful
	if (!copy)
		return NULL;

	unsigned short line, column;
	// Iterate through each pixel in the image
	for (line = 0; line < image.height; line++) {
		for (column = 0; column < image.width; column++) {
			// Check if the pixel is within the specified area or on the image
			// boundary
			if ((line < selection.line_start ||
			     line > selection.line_end - 1 ||
			     column < selection.column_start ||
			     column > selection.column_end - 1) ||
			    (line == image.height - 1 || line == 0 ||
			     column == image.width - 1 || column == 0)) {
				// If so, copy the pixel as is
				copy[line][column] =
				    image.picture[line][column];
			} else {
				// Apply the edge filter to the pixel
				copy[line][column].red =
					clamp(8 * image.picture[line][column].red -
						image.picture[line - 1][column - 1].red -
						image.picture[line - 1][column].red -
						image.picture[line - 1][column + 1].red -
						image.picture[line][column - 1].red -
						image.picture[line][column + 1].red -
						image.picture[line + 1][column - 1].red -
						image.picture[line + 1][column].red -
						image.picture[line + 1][column + 1].red);

				copy[line][column].green =
					clamp(8 * image.picture[line][column].green -
						image.picture[line - 1][column - 1].green -
						image.picture[line - 1][column].green -
						image.picture[line - 1][column + 1].green -
						image.picture[line][column - 1].green -
						image.picture[line][column + 1].green -
						image.picture[line + 1][column - 1].green -
						image.picture[line + 1][column].green -
						image.picture[line + 1][column + 1].green);

				copy[line][column].blue =
					clamp(8 * image.picture[line][column].blue -
						image.picture[line - 1][column - 1].blue -
						image.picture[line - 1][column].blue -
						image.picture[line - 1][column + 1].blue -
						image.picture[line][column - 1].blue -
						image.picture[line][column + 1].blue -
						image.picture[line + 1][column - 1].blue -
						image.picture[line + 1][column].blue -
						image.picture[line + 1][column + 1].blue);
			}
		}
	}

	// Return the dynamically allocated copy of the image with the edge filter
	// applied
	return copy;
}

// Function to apply a sharpening filter to the specified area of the image
//
// Parameters:
//	 - image: Pointer to the image structure to be modified
//	 - selection: Area selection structure specifying the region to apply
//				  the sharpening filter
//
// Returns:
//   - A dynamically allocated copy of the image with the sharpening filter
//	   applied
pixel_t **apply_sharpen(image_t image, area_t selection)
{
	// Create a copy of the image
	pixel_t **copy = create_picture(image.height, image.width);

	// Check if memory allocation for the copy was successful
	if (!copy)
		return NULL;

	unsigned short line, column;
	// Iterate through each pixel in the image
	for (line = 0; line < image.height; line++) {
		for (column = 0; column < image.width; column++) {
			// Check if the pixel is within the specified area or on the image
			// boundary
			if ((line < selection.line_start ||
			     line > selection.line_end - 1 ||
			     column < selection.column_start ||
			     column > selection.column_end - 1) ||
			    (line == image.height - 1 || line == 0 ||
			     column == image.width - 1 || column == 0)) {
				// If so, copy the pixel as is
				copy[line][column] =
				    image.picture[line][column];
			} else {
				// Apply the sharpening filter to the pixel
				copy[line][column].red =
				    clamp(5 * image.picture[line][column].red -
					  image.picture[line - 1][column].red -
					  image.picture[line][column - 1].red -
					  image.picture[line][column + 1].red -
					  image.picture[line + 1][column].red);

				copy[line][column].green =
					clamp(5 * image.picture[line][column].green -
						image.picture[line - 1][column].green -
						image.picture[line][column - 1].green -
						image.picture[line][column + 1].green -
						image.picture[line + 1][column].green);

				copy[line][column].blue =
				    clamp(5 * image.picture[line][column].blue -
					  image.picture[line - 1][column].blue -
					  image.picture[line][column - 1].blue -
					  image.picture[line][column + 1].blue -
					  image.picture[line + 1][column].blue);
			}
		}
	}

	// Return the dynamically allocated copy of the image with the sharpening
	// filter applied
	return copy;
}

// Function to apply a blur filter to the specified area of the image
//
// Parameters:
//	 - image: Pointer to the image structure to be modified
//	 - selection: Area selection structure specifying the region to apply
//				  the blur filter
//
// Returns:
//   - A dynamically allocated copy of the image with the blur filter applied
pixel_t **apply_blur(image_t image, area_t selection)
{
	// Create a copy of the image
	pixel_t **copy = create_picture(image.height, image.width);

	// Check if memory allocation for the copy was successful
	if (!copy)
		return NULL;

	unsigned short line, column;
	// Iterate through each pixel in the image
	for (line = 0; line < image.height; line++) {
		for (column = 0; column < image.width; column++) {
			// Check if the pixel is within the specified area or on the image
			// boundary
			if ((line < selection.line_start ||
			     line > selection.line_end - 1 ||
			     column < selection.column_start ||
			     column > selection.column_end - 1) ||
			    (line == image.height - 1 || line == 0 ||
			     column == image.width - 1 || column == 0)) {
				// If so, copy the pixel as is
				copy[line][column] =
				    image.picture[line][column];
			} else {
				// Apply the blur filter to the pixel
				copy[line][column].red =
					round_double(1. *
						(image.picture[line - 1][column - 1].red +
						image.picture[line - 1][column].red +
						image.picture[line - 1][column + 1].red +
						image.picture[line][column - 1].red +
						image.picture[line][column].red +
						image.picture[line][column + 1].red +
						image.picture[line + 1][column - 1].red +
						image.picture[line + 1][column].red +
						image.picture[line + 1][column + 1].red) /
						9.);

				copy[line][column].green =
					round_double(1. *
						(image.picture[line - 1][column - 1].green +
						image.picture[line - 1][column].green +
						image.picture[line - 1][column + 1].green +
						image.picture[line][column - 1].green +
						image.picture[line][column].green +
						image.picture[line][column + 1].green +
						image.picture[line + 1][column - 1].green +
						image.picture[line + 1][column].green +
						image.picture[line + 1][column + 1].green) /
						9.);

				copy[line][column].blue =
					round_double(1. *
						(image.picture[line - 1][column - 1].blue +
						image.picture[line - 1][column].blue +
						image.picture[line - 1][column + 1].blue +
						image.picture[line][column - 1].blue +
						image.picture[line][column].blue +
						image.picture[line][column + 1].blue +
						image.picture[line + 1][column - 1].blue +
						image.picture[line + 1][column].blue +
						image.picture[line + 1][column + 1].blue) /
						9.);
			}
		}
	}

	// Return the dynamically allocated copy of the image with the blur filter
	// applied
	return copy;
}

// Function to apply a Gaussian blur filter to the specified area of the image
//
// Parameters:
//	 - image: Pointer to the image structure to be modified
//	 - selection: Area selection structure specifying the region to apply
//				  the Gaussian blur filter
//
// Returns:
//   - A dynamically allocated copy of the image with the Gaussian blur filter
//	   applied
pixel_t **apply_gaussian_blur(image_t image, area_t selection)
{
	// Create a copy of the image
	pixel_t **copy = create_picture(image.height, image.width);

	// Check if memory allocation for the copy was successful
	if (!copy)
		return NULL;

	unsigned short line, column;
	// Iterate through each pixel in the image
	for (line = 0; line < image.height; line++) {
		for (column = 0; column < image.width; column++) {
			// Check if the pixel is within the specified area or on the image
			// boundary
			if ((line < selection.line_start ||
			     line > selection.line_end - 1 ||
			     column < selection.column_start ||
			     column > selection.column_end - 1) ||
			    (line == image.height - 1 || line == 0 ||
			     column == image.width - 1 || column == 0)) {
				// If so, copy the pixel as is
				copy[line][column] =
				    image.picture[line][column];
			} else {
				// Apply the Gaussian blur filter to the pixel
				copy[line][column].red =
					round_double(1. *
						(image.picture[line - 1][column - 1].red +
						2 * image.picture[line - 1][column].red +
						image.picture[line - 1][column + 1].red +
						2 * image.picture[line][column - 1].red +
						4 * image.picture[line][column].red +
						2 * image.picture[line][column + 1].red +
						image.picture[line + 1][column - 1].red +
						2 * image.picture[line + 1][column].red +
						image.picture[line + 1][column + 1].red) /
						16.);

				copy[line][column].green =
					round_double(1. *
						(image.picture[line - 1][column - 1].green +
						2 * image.picture[line - 1][column].green +
						image.picture[line - 1][column + 1].green +
						2 * image.picture[line][column - 1].green +
						4 * image.picture[line][column].green +
						2 * image.picture[line][column + 1].green +
						image.picture[line + 1][column - 1].green +
						2 * image.picture[line + 1][column].green +
						image.picture[line + 1][column + 1].green) /
						16.);

				copy[line][column].blue =
					round_double(1. *
						(image.picture[line - 1][column - 1].blue +
						2 * image.picture[line - 1][column].blue +
						image.picture[line - 1][column + 1].blue +
						2 * image.picture[line][column - 1].blue +
						4 * image.picture[line][column].blue +
						2 * image.picture[line][column + 1].blue +
						image.picture[line + 1][column - 1].blue +
						2 * image.picture[line + 1][column].blue +
						image.picture[line + 1][column + 1].blue) /
						16.);
			}
		}
	}

	// Return the dynamically allocated copy of the image with the Gaussian
	// blur filter applied
	return copy;
}

// Function to apply a specified filter to the specified area of the image
//
// Parameters:
//   - image: Pointer to the image structure to be modified
//   - selection: Area selection structure specifying the region to apply the
//				  filter
//   - parameter_1: String specifying the filter to apply
//   - parameter_2: Extra parameter (not used for now)
void apply_command(image_t *image, area_t selection,
				   char parameter_1[MAX_INPUT_LINE_LENGTH],
				   char parameter_2[MAX_NUMBER_SIZE + 1])
{
	// Check if an image is loaded
	if (!image->picture) {
		printf("No image loaded\n");
		return;
	}

	// Check for invalid parameters
	if (!strlen(parameter_1) || strlen(parameter_2)) {
		printf("Invalid command\n");
		return;
	}

	// Check if the image is a grayscale image
	if (!image->color) {
		printf("Easy, Charlie Chaplin\n");
		return;
	}

	// Declare a variable to store the resulting image after applying the filter
	pixel_t **new_image;

	// Compare the input parameter with different filter options
	if (!strcmp(parameter_1, "EDGE")) {
		// Apply the edge filter
		new_image = apply_edge(*image, selection);
	} else if (!strcmp(parameter_1, "SHARPEN")) {
		// Apply the sharpen filter
		new_image = apply_sharpen(*image, selection);
	} else if (!strcmp(parameter_1, "BLUR")) {
		// Apply the blur filter
		new_image = apply_blur(*image, selection);
	} else if (!strcmp(parameter_1, "GAUSSIAN_BLUR")) {
		// Apply the Gaussian blur filter
		new_image = apply_gaussian_blur(*image, selection);
	} else {
		// Print an error message if the input parameter is not valid
		printf("APPLY parameter invalid\n");
		return;
	}

	// Check if the filter application was successful (new_image is not NULL)
	if (new_image) {
		// Free the memory of the original image
		free_picture(&image->picture, image->height);
		// Update the image structure with the new image
		image->picture = new_image;

		// Print a success message
		printf("APPLY %s done\n", parameter_1);
	}
}

// Function to save an image in P2 format
//
// Parameters:
//	 - image: Image structure containing the data to be saved
//	 - file_name: String specifying the name of the file to save
void save_P2(image_t image, char file_name[FILE_NAME_LENGTH])
{
	// Open the file for writing
	FILE *file = fopen(file_name, "wt");

	// Check if the file is opened successfully
	if (!file)
		return;

	// Write the P2 header information to the file
	fprintf(file, "P2\n# %s\n%hd %hd\n%hd\n", file_name, image.width,
			image.height, MAX_VALUE);

	// Write the pixel values to the file
	short line, column;
	for (line = 0; line < image.height; line++) {
		for (column = 0; column < image.width; column++)
			fprintf(file, "%3hd ", image.picture[line][column].red);
		fprintf(file, "\n");
	}

	// Close the file
	fclose(file);

	printf("Saved %s\n", file_name);
}

// Function to save an image in P3 format
//
// Parameters:
//	 - image: Image structure containing the data to be saved
//	 - file_name: String specifying the name of the file to save
void save_P3(image_t image, char file_name[FILE_NAME_LENGTH])
{
	// Open the file for writing
	FILE *file = fopen(file_name, "wt");

	// Check if the file is opened successfully
	if (!file)
		return;

	// Write the P3 header information to the file
	fprintf(file, "P3\n# %s\n%hd %hd\n%hd\n", file_name, image.width,
			image.height, MAX_VALUE);

	// Write the pixel values to the file
	short line, column;
	for (line = 0; line < image.height; line++) {
		for (column = 0; column < image.width; column++)
			fprintf(file, "%3hd %3hd %3hd ",
					image.picture[line][column].red,
					image.picture[line][column].green,
					image.picture[line][column].blue);
		fprintf(file, "\n");
	}

	// Close the file
	fclose(file);

	printf("Saved %s\n", file_name);
}

// Function to save an image in P5 format
//
// Parameters:
//	 - image: Image structure containing the data to be saved
//	 - file_name: String specifying the name of the file to save
void save_P5(image_t image, char file_name[FILE_NAME_LENGTH])
{
	// Open the file for writing in binary mode
	FILE *file = fopen(file_name, "wb");

	// Check if the file is opened successfully
	if (!file)
		return;

	// Write the P5 header information to the file
	fprintf(file, "P5\n# %s\n%hd %hd\n%hd\n", file_name, image.width,
			image.height, MAX_VALUE);

	// Write the pixel values to the file
	short line, column;
	for (line = 0; line < image.height; line++)
		for (column = 0; column < image.width; column++)
			fputc(image.picture[line][column].red, file);

	// Close the file
	fclose(file);

	printf("Saved %s\n", file_name);
}

// Function to save an image in P6 format
//
// Parameters:
//	 - image: Image structure containing the data to be saved
//	 - file_name: String specifying the name of the file to save
void save_P6(image_t image, char file_name[FILE_NAME_LENGTH])
{
	// Open the file for writing in binary mode
	FILE *file = fopen(file_name, "wb");

	// Check if the file is opened successfully
	if (!file)
		return;

	// Write the P6 header information to the file
	fprintf(file, "P6\n# %s\n%hd %hd\n%hd\n", file_name, image.width,
			image.height, MAX_VALUE);

	// Write the RGB pixel values to the file
	short line, column;
	for (line = 0; line < image.height; line++) {
		for (column = 0; column < image.width; column++) {
			fputc(image.picture[line][column].red, file);
			fputc(image.picture[line][column].green, file);
			fputc(image.picture[line][column].blue, file);
		}
	}

	// Close the file
	fclose(file);

	printf("Saved %s\n", file_name);
}

// Function to save an image based on the required format (P2, P3, P5, P6)
//
// Parameters:
//   - image: Image structure containing the data to be saved
//   - file_name: String specifying the name of the file to save
//   - parameter_2: String specifying the format ("ascii" for ASCII,
//					otherwise binary)
void save_command(image_t image, char file_name[FILE_NAME_LENGTH],
				  char parameter_2[MAX_NUMBER_SIZE + 1])
{
	// Print an error message if no image is loaded
	if (!image.picture) {
		printf("No image loaded\n");
		return;
	}

	// Check the color type and call the appropriate save function
	if (!strncmp(parameter_2, "ascii", strlen("ascii"))) {
		if (!image.color) {
			// Save grayscale image in ASCII format (P2)
			save_P2(image, file_name);
		} else {
			// Save color image in ASCII format (P3)
			save_P3(image, file_name);
		}
	} else {
		if (!image.color) {
			// Save grayscale image in binary format (P5)
			save_P5(image, file_name);
		} else {
			// Save color image in binary format (P6)
			save_P6(image, file_name);
		}
	}
}

// Function to get command and parameters from user input
//
// Parameters:
//	 - command: String to store the command entered by the user
//	 - parameter_1: String to store the first parameter entered by the user
//	 - parameter_2: String to store the second parameter entered by the user
//	 - parameter_3: String to store the third parameter entered by the user
//	 - parameter_4: String to store the fourth parameter entered by the user
//	 - parameter_5: String to store the fifth parameter entered by the user
void get_command(char command[MAX_COMMAND_LENGTH],
				 char parameter_1[MAX_INPUT_LINE_LENGTH],
				 char parameter_2[MAX_NUMBER_SIZE + 1],
				 char parameter_3[MAX_NUMBER_SIZE + 1],
				 char parameter_4[MAX_NUMBER_SIZE + 1], char parameter_5[1])
{
	// Read input line from user
	char input_line[MAX_INPUT_LINE_LENGTH], *input_parameter;
	fgets(input_line, MAX_INPUT_LINE_LENGTH, stdin);

	// Extract command from input line
	strcpy(command, strtok(input_line, "\n "));

	// Extract first parameter from input line
	input_parameter = strtok(NULL, "\n ");
	if (input_parameter) {
		strcpy(parameter_1, input_parameter);
	} else {
		parameter_1[0] = '\0';
		parameter_2[0] = '\0';
		parameter_3[0] = '\0';
		parameter_4[0] = '\n';
		parameter_5[0] = '\0';
		return;
	}

	// Extract second parameter from input line
	input_parameter = strtok(NULL, "\n ");
	if (input_parameter) {
		strcpy(parameter_2, input_parameter);
	} else {
		parameter_2[0] = '\0';
		parameter_3[0] = '\0';
		parameter_4[0] = '\n';
		parameter_5[0] = '\0';
		return;
	}

	// Extract third parameter from input line
	input_parameter = strtok(NULL, "\n ");
	if (input_parameter) {
		strcpy(parameter_3, input_parameter);
	} else {
		parameter_3[0] = '\0';
		parameter_4[0] = '\n';
		parameter_5[0] = '\0';
		return;
	}

	// Extract fourth parameter from input line
	input_parameter = strtok(NULL, "\n ");
	if (input_parameter) {
		strcpy(parameter_4, input_parameter);
	} else {
		parameter_4[0] = '\n';
		parameter_5[0] = '\0';
		return;
	}

	// Extract fifth parameter from input line
	input_parameter = strtok(NULL, "\n ");
	if (input_parameter)
		strcpy(parameter_5, input_parameter);
	else
		parameter_5[0] = '\0';
}

// Main function to execute the image processing program
int main(void)
{
	// Declare variables to store user commands and parameters
	char command[MAX_COMMAND_LENGTH], parameter_1[MAX_INPUT_LINE_LENGTH],
	    parameter_2[MAX_NUMBER_SIZE + 1], parameter_3[MAX_NUMBER_SIZE + 1],
	    parameter_4[MAX_NUMBER_SIZE + 1], parameter_5[1];

	// Declare a selection area structure
	area_t selection;

	// Declare an image structure and initialize the picture pointer to NULL
	image_t image;
	image.picture = NULL;

	// Main program loop
	while (true) {
		// Get user command and parameters
		get_command(command, parameter_1, parameter_2, parameter_3,
					parameter_4, parameter_5);

		if (!strcmp(command, "EXIT")) {
			// Execute the EXIT command
			if (!image.picture)
				printf("No image loaded\n");
			else
				free_picture(&image.picture, image.height);

			// Exit the program
			return 0;
		}

		// Check and execute the appropriate command
		if (!strcmp(command, "LOAD") && strlen(parameter_1) &&
		    !strlen(parameter_2)) {
			// Execute the LOAD command
			load_command(&image, &selection, parameter_1);

			/* print_image(image); */
		} else if (!strcmp(command, "SELECT")) {
			// Execute the SELECT command
			select_command(image, &selection, parameter_1,
						   parameter_2, parameter_3, parameter_4,
						   parameter_5);
		} else if (!strcmp(command, "HISTOGRAM")) {
			// Execute the HISTOGRAM command
			histogram_command(image, parameter_1, parameter_2,
							  parameter_3);
		} else if (!strcmp(command, "EQUALIZE") &&
			   !strlen(parameter_1)) {
			// Execute the EQUALIZE command
			if (!image.picture)
				printf("No image loaded\n");
			else if (image.color)
				printf("Black and white image needed\n");
			else
				equalize(&image);
		} else if (!strcmp(command, "ROTATE")) {
			// Execute the ROTATE command
			rotate_command(&image, selection, parameter_1);
		} else if (!strcmp(command, "CROP") && !strlen(parameter_1)) {
			// Execute the CROP command
			if (!image.picture) {
				printf("No image loaded\n");
			} else {
				if (selection.all)
					printf("Image cropped\n");
				else
					crop(&image, &selection);
			}
		} else if (!strcmp(command, "APPLY")) {
			// Execute the APPLY command
			apply_command(&image, selection, parameter_1,
						  parameter_2);
		} else if (!strcmp(command, "SAVE") && strlen(parameter_1)) {
			// Execute the SAVE command
			save_command(image, parameter_1, parameter_2);
		} else {
			// Print an error message for an invalid command
			printf("Invalid command\n");
		}
	}
}
