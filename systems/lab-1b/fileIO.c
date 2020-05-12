#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <getopt.h>
#include <errno.h>
#include <string.h>

void usage( char *cmd )
{
    printf("Usage: %s <option(s)> fileName\n" , cmd);
    printf(" Options are:\n");
    printf("  -h      display this help screen\n");
    printf("  -s n    start reading at location 'n'. Default n=0\n");
    printf("  -c m    read 'm' bytes. Default m=1\n");
}

/*-------------------------------------------------------------------------*/

bool parse_CL( int argc, char **argv, bool *sOpt, unsigned *sVal ,
               bool *cOpt, unsigned *cVal, char **file )
{
    int opt;


    // Initialize the call-by-ref arguments
    *sOpt = *cOpt =  false ;   
    *sVal = 0;  *cVal=1 ;
    *file   = NULL ;  
    
    opterr = 0 ;  /* Prevent getopt() from printing error messages */
    char *optionStr = "+hs:c:" ; 

    while ( ( opt = getopt(argc, argv, optionStr ) ) != -1)  
    {
        switch (opt) 
        {
            case 'h':	
                usage( argv[0] );                    
                return true ;
                break ;
                        
            case 's':
                *sOpt = true;
                *sVal = atoi(argv[optind-1]);
                break ;

            case 'c':
			    *cOpt = true;
			    *cVal = atoi(argv[optind-1]);
                break ;

            default:	usage( argv[0] ) ;   // invalid options found
                        return false ; 
        }
    }

    // Get the mandatory file name
    // Exactly ONE FileName must be passed at the end of command line
    if (argc - optind == 1) {
	    *file = argv[optind];
	    return true;
    }
    
    return false;
}

/*-------------------------------------------------------------------------*/

bool read_bytes (FILE *file, unsigned long start, unsigned long count, void* buffer)
{
    if ( fseek ( file, start, SEEK_SET ) != 0 )
        return false;   // Invalid starting location in the file
        
    if( fread( buffer, sizeof(uint8_t), count, file) != count )
        return false;  // The desired number of bytes could not be read from the file

    return true;
}
/*-------------------------------------------------------------------------*/


#define MAXBUF  2000

int main (int argc, char **argv)
{
    bool        s_opt , c_opt ;
    unsigned    s_val , c_val ;
    char        *fname = NULL ;
    uint8_t     data[MAXBUF] ;
        
    // Parse command line and check for failure
    if( ! parse_CL ( argc, argv, &s_opt, &s_val, &c_opt, &c_val, &fname ) )
        exit( EXIT_FAILURE );

    if( !fname )  // Parsing succeeds, but no file name, it was a '-h' case
        exit( EXIT_SUCCESS );

    // Open the input file
    FILE *input = fopen( fname, "r");

    if( input == NULL) 
    {
        puts("Failed to read file");
        exit( EXIT_FAILURE );
    }
    
    // Read the desired file bytes into the buffer
    if( ! read_bytes( input, s_val, c_val, data ) )
    {
        puts("Failed to read file");
        exit( EXIT_FAILURE );
    }

    // Now, dump those bytes to the stdout in hexadecimal format
    int i=0 ;
    while ( i < c_val )
    {      
        if (i % 16 == 0) {
            printf("%08d  ", i+ s_val);
        }
        if (i % 16 == 8) {
            printf("  ");
        }
        printf("%02x ", data[i]);
        
        if (i % 16 == 15) {
            printf("\n");
        }
        i++;
    }
    printf("\n");

    exit( EXIT_SUCCESS );
}
