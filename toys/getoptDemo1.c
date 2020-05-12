/*
 * CS 261 Example
 *
 * Demonstrates how getopt() works to parse command-line options.
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <getopt.h>

void usage ()
{
    printf("Usage: ./command [options] <filename>\n");
    printf(" Valid options are (in any order/combination):\n");
    printf("  -a \n");
    printf("  -b \n");
    printf("  -n <decimal integer> \n");
}

bool parseCL( int argc, char **argv, 
              bool *optionA, bool *optionN, int *valN, 
              bool *optionB, char **file )
{
    if( !argv )
        return false;
    // should also check the other pointers are not NULL

    const int numBase = 10 ; 
    int number ;  
    *optionA = *optionN = *optionB = false ;
    *file    = NULL ;

    opterr = 0 ;           /* Prevent getopt() from printing error messages */
    char *optionStr = "+an:b" ; /* A leading '+' prevents reordering of argv */
    char opt , *end;
    while ( ( opt = getopt(argc, argv, optionStr ) ) != -1)  
    {
        switch (opt) 
        {
            case 'a':	*optionA = true; break ;
            case 'b':   *optionB = true; break ;
            case 'n':   end = optarg;
                        number = strtol( optarg, &end, numBase );
                        if (end == optarg)   {  
                            // 'optarg' string does not represent an integer
                            usage() ;
                            return false ;
                        }
                        *optionN = true;
                        *valN    = number ;
                        break;
            default:	usage() ;   // invalid options found
                        return false ; 
        }
    }

    if( optind != argc - 1)  // Exactly ONE FileName must be passed at the end
    {
        usage() ; // either no fileNames, or more than one fileName present
        return false;
    }

    *file = argv[optind];   
    return true;
}

int main (int argc, char **argv)
{
    bool opA, opN, opB ;
    int nVal ;
    char *fileName ;

    // Call the C.L. parsing function
    if ( !parseCL( argc , argv , &opA , &opN , &nVal , &opB , &fileName) ) 
    {
        printf("The parseCL function encountered some problem(s)\n");
        exit( EXIT_FAILURE );
    }
    
    printf("The following options were present on the C.L. :\n") ;
    // Now, respond to each of the options you received
    if( opA ) printf("-a\n") ;
    
    if( opB ) printf("-b\n") ;
    
    if( opN ) printf("-n %d\n" , nVal ) ;
    
    printf("Filename: '%s' \n", fileName );
    
    return EXIT_SUCCESS;
}

