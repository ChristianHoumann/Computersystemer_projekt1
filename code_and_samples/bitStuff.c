
#include <stdlib.h>
#include <stdio.h>
#include "cbmp.h"

#define SetBit(A,k)     ( A[(k/32)] |= (1 << (k%32)) )  
#define ClearBit(A,k)   ( A[(k/32)] &= ~(1 << (k%32)) )  
#define TestBit(A,k)    ( A[(k/32)] & (1 << (k%32)) )


int main(int argc, char** argv)
{
    int A[2];
    A[0]=0;
    
    printf("%d\n", A[0]);

    SetBit(A,2);

    printf("%d\n", A[0]);

    SetBit(A,0);

    printf("%d\n", A[0]);

    SetBit(A,3);

    printf("%d\n", TestBit(A,3));
    printf("%d\n", A[0]);
  return 0;
}
