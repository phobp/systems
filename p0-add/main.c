/*
 * CS 261: Demo project driver
 *
 * Name: 
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "p0-add.h"

int main (int argc, char **argv)
{
    // parse command-line options
    if (argc != 3) {
        printf("Usage: add num1 num2\n");
        return EXIT_SUCCESS;
    }
    int num1 = strtol (argv[1], NULL, 10);
    int num2 = strtol (argv[2], NULL, 10);

    // add numbers and print the result
    printf("%d\n", add_abs(num1, num2));

    return EXIT_SUCCESS;
}

