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
    printf ("len = %d\n", strlen (in));

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

struct mac_type
{
    unsigned char type;
    int val;
};
#define VID_MIN 1
#define VID_MAX 4094
#define VNI_MIN 1
#define VNI_MAX 16777215
int
mac_type2fid (struct mac_type *p)
{
    if (p->type == 0)
        {
            if (p->val < VID_MIN || p->val > VID_MAX)
                {
                    return -1;
                }
        }
    else
        {
            if (p->val < VNI_MIN || p->val > VNI_MAX)
                {
                    return -1;
                }
        }

    return ((p->type << 24) | (p->val & 0xffffff));
}

int
fid2mac_type (int fid, struct mac_type *p)
{
    int nRet;
    struct mac_type *tmp = p;

    tmp->type = (unsigned char) (fid >> 24);
    tmp->val = (fid & 0xffffff);
    nRet = mac_type2fid (tmp);

    if (nRet < 0)
        {
            return -1;
        }
    return 0;
}
int
main (int argc, char **argv)
{
    int fid = 16789561;
    struct mac_type s;
    memset(&s,0,sizeof(struct mac_type));
    fid2mac_type(fid,&s);
    printf("type = %d,val = %d\n",s.type,s.val);

    fid = 0;
    fid = mac_type2fid(&s);
    printf("fid = %d\n",fid);
    return 0;
}
