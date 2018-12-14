/* 
 * test.c
 * Original Author: chenwei3@ruijie.com.cn, 2018-12-14
 *
 * 
 * 
 *
 * History 
 * 
 *  
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char str[] = "[1234, 5678]";

int
do_esi (const char *in, int data[])
{
    printf("len = %d\n",strlen(in));

    char buf[1024];
    memset (buf, 0, sizeof(buf));
    memcpy (buf, in + 1, strlen (in) - 2);
    char *token;
    char *p = buf;
    int *k = data;
    int count = 0;
    while ((token = strsep (&p, ",")) != NULL)
        {
            *k = atoi (token);
            k++;
            count++;
        }
    return count;
}

int d[256] =
    { 0 };
int
main (int argc, char **argv)
{

    int count = 0;
    int j;
    count =do_esi (str, d);

    for (j = 0; j < count; ++j)
        {
            printf("%d ",d[j]);
        }
    printf("\n%d ",count);

    return 0;
}
