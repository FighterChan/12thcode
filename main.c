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

    printf("%s\n",text);
    struct mac_in indata;
    memset(&indata,0,sizeof(struct mac_in));

    sscanf (text,"type:\"%s\",proto:\"%s\",source:\"%s\",fid:%dmacaddress:\"%s\",nexthoptype:\"%s\",nexthop:%d",
            indata.type, indata.proto, indata.source, &indata.fid,
            indata.macaddress, indata.nexthoptype, &indata.nexthop);
    printf ("indata->fid = %d\n", indata.fid);
    printf ("indata->nexthop = %d\n", indata.nexthop);
    printf ("indata->macaddress = %s\n", indata.macaddress);
    return 0;
}

/* Read a file, parse */
void
dofile (char *filename)
{
    FILE *f;
    long len;
    char *data;

    f = fopen (filename, "r");
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
//    dofile (argv[1]);
    char k1[16] = {0};
    char k2[16] = {0};


    sscanf("[ { key1: \"value1\"        \n key2: \"value2\"","%*s%*s%*s%s%*s%s",k1,k2);
    printf("%s\n%s\n",k1,k2);
    return 0;
}
