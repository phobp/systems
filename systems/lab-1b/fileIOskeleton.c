#include ....
#include <stdio.h>
#include <stdlib.h>
#include ....
#include ....

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


    // Initialize the call-by-ref agruments
    *sOpt = *cOpt =  false ;   
    *sVal = 0;  *cVal=1 ;
    *file   = NULL ;  
    
    opterr = 0 ;  /* Prevent getopt() from printing error messages */
    char *optionStr = "hsn:c" ; 

    ....
    while ( ( opt = getopt(argc, argv, optionStr ) ) != -1)  
    {
        switch (opt) 
        {
            case 'h':	usage( argv[0] );                    
                        return true ;
                        break ;
                        
            case 's':  .... 
                        break ;

            case 'c':   ....
                        break ;

            default:	usage( argv[0] ) ;   // invalid options found
                        return false ; 
        }
    }

    // Get the mandatory file name
    // Exactly ONE FileName must be passed at the end of command line
    
    if ( optind  !=  argc -1 )
    {
        usage();
        return false;
    }

    *file = argv[optind];
    return true;
}

/*-------------------------------------------------------------------------*/

bool read_bytes (FILE *file, unsigned long start, unsigned long count, void* buffer)
{
    if ( fseek ( .... ) != 0 )
        return false;   // Invalid starting location in the file

    if( fread( .... ) != 1 )
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
    if( ! parse_CL ( .... ) )
        exit( EXIT_FAILURE );

    if( !fname )  // Parsing succeeds, but no file name, it was a '-h' case
        exit( EXIT_SUCCESS ); 

    // Open the input file
    FILE *input = fopen( .... );
    if( !input ) 
    {
        puts("Failed to read file");
        exit( EXIT_FAILURE );
    }

    // Read the desired file bytes into the buffer
    if( ! read_bytes( .... ) )
    {
        puts("Failed to read file");
        exit( EXIT_FAILURE );
    }

    // Now, dump those bytes to the stdout in hexadecimal format
    int i=0 ;
    while ( i < c_val )
    {
        printf("%10u ", i );
        
        ....
        
        printf("\n") ;
    }

    exit( EXIT_SUCCESS );
}
