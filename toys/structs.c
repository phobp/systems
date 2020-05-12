#include <stdbool.h>
#include <stdio.h>
#include <math.h>

typedef struct {
    char a;
    int  x ;
    char b;
    char c;
    long y;
    char d ;   /* What happens if this is uncommented? */
} nonpacked_t;

typedef struct __attribute__((__packed__)) {
    char a;
    int  x ;
    char b;
    char c;
    long y;
    char d ;  /* What happens if this is uncommented? */
} packed_t;


struct vertex {
    double x;
    double y;
    bool visited;
};

typedef struct vertex vertex_t;

// Just for the fun and illustration purposes,
// second argument is a pointer to a vertex_t
double dist( vertex_t a , vertex_t *b)
{
    return sqrt( pow( (a.x - b->x) , 2 ) +
                 pow( (a.y - b->y) , 2 ) );
}

int main()
{
    struct vertex p1;
    p1.x = 4.25;
    p1.y = 6.61;
    p1.visited = false;

    vertex_t p2;
    p2.x = 2.81;
    p2.y = 8.18;
    p2.visited = false;

    printf( "%f\n", dist(p1, &p2) );

    printf("unpacked size = %lu\n", sizeof(nonpacked_t));
    printf("  packed size = %lu\n", sizeof(packed_t));

    return 0;
}

