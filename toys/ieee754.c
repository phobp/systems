/* The IEEE-754 Floating-Point Representation */
/* By: Mohamed Aboutabl */

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdlib.h>


void showBytes( uint8_t *p , size_t n)
{
    unsigned i;
    for ( i=0 ; i<n ; i++)
    {
        printf(" %02x" , *p );
        p++;
    }
}

void showBytesBigEndian( uint8_t *p , size_t n)
{
    unsigned i;
    p += n-1 ;
    for ( i=0 ; i<n ; i++)
    {
        printf(" %02x" , *p );
        p--;
    }
}

int main(int argc , char **argv)
{
    float   flt;
    double  dbl ;

    if ( argc < 2 )
    {
        puts("must provide a floating-point value");
        return 0;
    }

    flt = strtof( argv[1] , NULL ); 
    dbl = strtod( argv[1] , NULL );
    
    printf ("Single Precision, %30.15f = HEX " , flt ); 
    showBytesBigEndian( (uint8_t *) &flt , sizeof(float) );
    puts("");
    
    printf ("Double Precision, %30.15f = HEX " , dbl ); 
    showBytesBigEndian( (uint8_t *) &dbl , sizeof(double) );
    puts("");
   
}
