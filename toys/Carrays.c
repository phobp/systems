#include <stdio.h>

#define N_LNGS 10
#define G_ROWS 5
#define G_COLS 6

long nums[N_LNGS];
double grades[G_ROWS][G_COLS];

int main()
{
    int i, j;
    for(  i=0 ; i < N_LNGS ; i++ )
        printf("nums[%2d]= %ld\n" , i , nums[i] );

    for( i=0 ; i < G_ROWS ; i++)
    {
        for( j=0 ; j < G_COLS ; j++)
        {
            grades[i][j] = i + (double) j / 100.0 ;
            printf("grades[%2d][%2d]= %4.2f  " , i , j, grades[i][j] );
        }
        
        puts("");
    }        
}


