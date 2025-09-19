#include <stdio.h>
int main(void) 
{
 int i,j,k,n;
 scanf("%d", &n);
 for (i=1; i<=n; i++)
  for (j=1; j<=i; j++) 
  {
   for(k=1; k<=n-j; k++)
    printf(" ");
   for(k=1; k<=2*j-1; k++)
    printf("*");
   printf("\n");
  } 
 return 0;
}
