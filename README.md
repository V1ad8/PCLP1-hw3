Copyright Ungureanu Vlad-Marin 315CAa 2023-2024

Image Editor

Overview:

The solution to the problem relies on three structures: area_t (which
contains data about the selected area of the image), image_t (which
contains data about the image: its dimensions, whether it is grayscale
or color, and the picture as a 2D array pixels), and pixel_t (which
contains the RGB values of a pixel; for a grayscale pixel, the values
are equal). In the main() function, a selection and a picture
(initialized with a NULL picture) are declared. In an infinite loop,
each command and its parameters are read from STDIN by the get_command()
function, and each command is executed. Due to the impossibility of
using a switch case, multiple if cases are tested.

Tasks:

I will explain each command in the order that they appear in the if
cases of the infinite loop.

Task: EXIT

The EXIT command is first, so if the program needs to be exited, it does
not go through all of the if cases. It is checked if there is any image
loaded, in which case it is freed; otherwise, 'No image loaded' is
printed to STDOUT. Afterwards, the program reaches return and closes.

Task: LOAD <file_name>

First of all, it is checked whether the right number of parameters is
present (1), and then the load_command() function is called. It checks
if there is already an image loaded and frees it. Then it replaces the
image with the one returned by the load_image() function and modifies
the selected area to cover the entire image. The load_image() function
opens the file, then tries to read from the file with the read_image()
function and then displays a success message. If at any point in this
function, something fails, an error message is displayed, and it
returns an empty image. The read_image() function returns true (if it
succeeded in reading from the file) or false (if it did not). The
function reads the magic number, the dimensions of the image and its
maximum value. It checks between all the lines for comments with the
skip_comments() function and skips residual values discovered by trial
and error. Then, depending on the magic number, it is determined
whether the image is grayscale or color and then a corresponding
function is called for each type of file: read_P2(), read_P3(),
read_P5(), read_P6(). The skip_comments() function works by reading a
char, checking if it is '#' and, if it is, reading the whole line. At
the end, it puts the last character read back into the file. All of the
'read_PX()' functions are very similar, so I'll only explain the
differences. The ASCII functions (2 & 3) read each value with fscanf,
process it and put it in its corresponding place. The binary functions
(5 & 6) read a byte with fread, then cast it to short, process it and
put it in its corresponding place. The grayscale functions (2 & 5) read
one value at a time and copy it to all channels of the pixel, while the
color functions (3 & 6), read three values at a time.

Task: SELECT <column_start> <line_start> <column_end> <line_end>
        & SELECT ALL
        
The select_command() function is called. It determines which of the
commands needs to be executed by checking the parameters. If the first
parameter is 'ALL' and the others do not exist, the selected area is
updated. If the four parameters are all numbers, the area_select()
function is called. Otherwise, 'Invalid command' is displayed. The
area_select() function checks for the validity of the coordinates, in
which order they should be recorded, and then returns an area_t variable
with the updated coordinates. Then a success message is printed.

Task: HISTOGRAM <number_of_stars> <number_of_bins>

The histogram_command() function is called. It checks for errors and
displays the corresponding message; otherwise, it calls the
make_histogram() function. It goes through each one of the bins and
calculates the value of each one, then it passes it to the
print_stars() function, which displays the value for each bin and the '*'.

Task: EQUALIZE

It is only executed when there are no parameters present. It is checked
for errors, in which case displaying a corresponding message. If no
errors are found, the equalize() function is called. It calculates the
frequency for each value (0 to 255), then the cumulative distribution,
by doing the sum of the frequencies up to each value divided by the
area of the image. Then the function updates each pixel by replacing it
with the cumulative distribution of its value multiplied by 255.
Afterwards, a success message is printed.

Task: ROTATE <angle>

The rotate_command() function is called. It checks for errors and
displays a corresponding message. If no errors are found, depending on
whether the whole image is selected or not, either the rotate_area() or
the rotate_all() function is called. The rotate_area() function rotates
only a square selection of an image, while the rotate_all() function
rotates the whole image. The rotation is achieved by repeated clockwise
rotations. The rotated image is firstly calculated in a second 2D
array, then copied over the original. The only difference between the
two functions is that the rotate_area() function declares the copy only
once, while the rotate_all() function declares the copy on each
rotation, as it is changing dimensions (going from mxn to nxm with each
rotation). Then each function displays a success message.

Task: CROP

It is only executed when there are no parameters present. It is checked
for errors, in which case displaying a corresponding message. If no
errors are found, the crop() function is called. It creates a copy of
the image in which it copies only the selected area, then updates the
dimensions to the dimensions of the selection and the selected area to
cover the new image. Then a success message is printed.

Task: APPLY <parameter>

The apply_command() function is called. It checks for errors and
displays a corresponding message. If no errors are found, depending on
which filter needs to be applied, one of the following functions is
called: apply_edge(), apply_sharpen(), apply_blur() or
apply_gaussian_blur(). Each function returns an image with the
respective filter applied. This is achieved by multiplying each
compatible pixel (inside the current selection and not on the edges of
the image) with the corresponding kernel. Then a success message is printed.

Task: SAVE <file_name> [ascii]

The save_command() function is called. It checks for errors and
displays a corresponding message. If no errors are found, depending on
whether the image is grayscale or color and whether the save is in
ascii or binary format, one of the following functions is called:
save_P2(), save_P3(), save_P5() or save_P6(). Each function prints the
header, including a comment with the name of the file, then it prints
the image. The ASCII functions (2 & 3) print with fprintf, and the
binary functions (5 & 6) print with fputc. The grayscale functions (2 &
5) print one value at a time, while the color functions (3 & 6) print
three values at a time, corresponding to the RGB channels. Then each
function displays a success message.
