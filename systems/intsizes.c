/*
 * CS 261 Example
 *
 * Illustrates the sizes of C integer data types.
 */

#include <stdio.h>
#include <stdint.h>
#include <limits.h>

#define PRINT_SIZE(T) printf("%20s %2lu bytes:\t", #T, (long unsigned) sizeof(T))

int main ()
{

    printf("\n");
    PRINT_SIZE(char);
    printf("%20lld -->  %-20lld\n" , (long long) SCHAR_MIN, (long long) SCHAR_MAX) ;
    PRINT_SIZE(unsigned char);
    printf("%20llu -->  %-20llu\n" , 0LLU , (unsigned long long) UCHAR_MAX) ;

    PRINT_SIZE(short);
    printf("%20lld -->  %-20lld\n" , (long long) SHRT_MIN, (long long) SHRT_MAX) ;

    PRINT_SIZE(unsigned short);
    printf("%20llu -->  %-20llu\n" , 0LLU , (unsigned long long) USHRT_MAX) ;

    PRINT_SIZE(int);
    printf("%20lld -->  %-20lld\n" , (long long) INT_MIN, (long long) INT_MAX) ;

    PRINT_SIZE(unsigned int);
    printf("%20llu -->  %-20llu\n" , 0LLU, (unsigned long long) UINT_MAX) ;

    PRINT_SIZE(long);
    printf("%20lld -->  %-20lld\n" , (long long) LONG_MIN, (long long) LONG_MAX) ;

    PRINT_SIZE(unsigned long);
    printf("%20llu -->  %-20llu\n" , 0LLU, (unsigned long long) ULONG_MAX) ;

    PRINT_SIZE(long long);
    printf("%20lld -->  %-20lld\n" , LLONG_MIN, LLONG_MAX) ;

    PRINT_SIZE(unsigned long long);
    printf("%20llu -->  %-20llu\n", 0LLU, ULLONG_MAX) ;

    PRINT_SIZE(int8_t);
    printf("\n");

    PRINT_SIZE(uint8_t);
    printf("\n");

    PRINT_SIZE(int16_t);
    printf("\n");

    PRINT_SIZE(uint16_t);
    printf("\n");

    PRINT_SIZE(int32_t);
    printf("\n");

    PRINT_SIZE(uint32_t);
    printf("\n");

    PRINT_SIZE(int64_t);
    printf("\n");

    PRINT_SIZE(uint64_t);
    printf("\n");
   
}
