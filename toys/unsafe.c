/*
 * CS 261 Example
 *
 * Demonstrates the safe vs. unsafe use of I/O functions.
   Must be compiled using the C(( standard as follows
        gcc   -std=c99    -o unsafe    unsafe.c
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define BUFFLEN  8

int main( int argc , char *argv[] )
{
    char buffer[BUFFLEN] ;
    char c ;
    
    printf("\nType some long string (%d characters or longer):" , BUFFLEN );
    fgets( buffer , BUFFLEN , stdin ) ;
    printf("buffer = '%s' \n" , buffer );

    while (( c = getchar()) != '\n'  );  /* Clear leftover of previous input */
    printf("\nType some long string (%d characters or longer):" , BUFFLEN );
    gets( buffer ) ;
    printf("buffer = '%s' \n" , buffer );
    
    return 0;
}