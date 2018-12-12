/* 
 * main.c
 * Original Author: chenwei3@ruijie.com.cn, 2018-12-12
 *
 * 
 * 
 *
 * History 
 * 
 *  
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "mac.h"

struct list_head in_head;
int
doit (char *text)
{

    int ret;
    int i = 0;
    char tmp[1024];
    char type[8];
    memset (type, 0, sizeof(type));
    struct mac_in indata;
    memset (&indata, 0, sizeof(struct mac_in));
    for (i = 0;i < strlen (text);i++)
        {
            ret = sscanf (&text[i], "%*[^{]{%[^}]", tmp);
            printf ("%s\n", tmp);
            ret = sscanf (tmp, "%*%s", type);//sscanf全匹配试试
            printf ("type:%s\n", type);
            i++;
        }

    return 0;
}

/* Read a file, parse */
void
dofile (char *filename)
{
    FILE *f;
    long len;
    char *data;

    f = fopen (filename, "rb");
    fseek (f, 0, SEEK_END);
    len = ftell (f);
    fseek (f, 0, SEEK_SET);
    data = (char*) malloc (len + 1);
    if (!data)
        {
            printf ("data内存申请失败!\n");
            goto err;
        }
    fread (data, 1, len, f);
    doit (data);
    free (data);
    err: fclose (f);
}
int
main (int argc, char **argv)
{
    dofile (argv[1]);
//    int day, year;
//    char weekday[20], month[1024];
//    char *dtm = " {type:\"ADD-MAC\",     proto: \"DOT1Q\",     source: \"MLAG\",     fid: 1024,    macaddress: \"0000.0000.0001\",     nexthoptype: \"INTERFACE\",     nexthop: 1  }";
//    sscanf (dtm, "%*[^{]{%[^}]", month);
//
//    sscanf(month,"");
//    printf ("%s\n", month);
    return 0;
}
