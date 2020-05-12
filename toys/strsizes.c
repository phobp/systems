/*
 * CS 261 Example
 *
 * Illustrates the sizes of C strings and pointer data types.
 */

#include <stdio.h>
//#include <stdint.h>
#include <string.h>

#define PRINT_SIZE(T) printf("sizeof(%10s) is %lu bytes\n", #T, (long unsigned) sizeof(T))

int main ()
{
    char fname[]= "Ann" ;
    char *lname = "0123456789" ;
    
    PRINT_SIZE(fname);
    printf("strlen(%10s) is %lu\n" , "fname" , (unsigned long) strlen(fname) );
    PRINT_SIZE(*fname);
    PRINT_SIZE(lname);
    printf("strlen(%10s) is %lu\n" , "lname" , (unsigned long) strlen(lname) );
    PRINT_SIZE(*lname);
    
    printf("\n");   
}

