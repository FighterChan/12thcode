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
doit (char *text, long len)
{

    int nRet;
    long i = 0;
    char *tmp[128];
    char type[64];
    memset (type, 0, sizeof(type));
    struct mac_in indata;
    memset (&indata, 0, sizeof(struct mac_in));
    while (1)
        {
            nRet = sscanf (&text[i], "%s", type);
            if (nRet < 0)
                {
                    break;
                }
            if (strcmp ("type:", type) == 0)
                {
                    nRet = sscanf (&text[i], "%s", type);
                    if (strcmp (type, "\"ADD-MAC\",") == 0)
                        {
                            printf ("%s\n", type);
                        }

                }
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
    doit (data, len);
    free (data);
    err: fclose (f);
}

void
dotest ()
{
    char out[16];
    char out2[16];
    char *buf = "type: \"ADD-MAC\","
            "\r\n proto: \"DOT1Q\",";
    sscanf (buf, "%*s%*s%*s%s", out);
    printf ("%s\n", out);
    memcpy (out2, out + 1, strlen (out) - 3);
    printf ("%s\n", out2);

    while (1)
        ;
}
struct list_head smac_head;
struct mac_in *pmac_in;
int
main (int argc, char **argv)
{
//    dotest();
//    dofile (argv[1]);
    INIT_LIST_HEAD (&smac_head);
    char *line = NULL, *tmp_buf;
    char *name;
    size_t len;
    ssize_t read;
    int nRet;
    char type[64];
    char tmp[256][64];

    memset (type, 0, sizeof(type));
    FILE *infp;
    infp = fopen (argv[1], "r");
    if (!infp)
        {
            printf ("can not open!\n");
            return -1;
        }
    int i = 0;
    int flg = 0;

    struct mac smac_in;
    memset (&smac_in, 0, sizeof(struct mac));
    while (!feof (infp))
        {
            nRet = fscanf (infp, "%s", type);
            if (nRet < 0)
                {
                    break;
                }

            if (strcmp ("type:", type) == 0)
                {
                    nRet = fscanf (infp, "%s", type);

                    if (strcmp (type, "\"ADD-MAC\",") == 0)
                        {
                            pmac_in = (struct mac_in *) malloc (
                                    sizeof(struct mac_in));
                            if (!pmac_in)
                                {
                                    return -1;
                                }
                            strcpy (pmac_in->type, "ADD-MAC");

                            nRet = fscanf (
                                    infp, "%*s%s %*s%s %*s%s %*s%s %*s%s %*s%s",
                                    smac_in.proto, smac_in.source, smac_in.fid,
                                    smac_in.macaddress, smac_in.nexthoptype,
                                    smac_in.nexthop);

                            memcpy (pmac_in->proto, smac_in.proto + 1,
                                    strlen (smac_in.proto) - 3);

                            memcpy (pmac_in->source, smac_in.source + 1,
                                    strlen (smac_in.source) - 3);

                            char buf[8];
                            memset (buf, 0, sizeof(buf));
                            memcpy (buf, smac_in.fid, strlen (smac_in.fid) - 1);
                            pmac_in->fid = atoi (buf);

                            memcpy (pmac_in->macaddress, smac_in.macaddress + 1,
                                    strlen (smac_in.macaddress) - 3);

                            memcpy (pmac_in->nexthoptype,
                                    smac_in.nexthoptype + 1,
                                    strlen (smac_in.nexthoptype) - 3);

                            pmac_in->nexthop = atoi (smac_in.nexthop);
                            list_add_tail (&pmac_in->list, &smac_head);

                        }
                }
            memset (type, 0, sizeof(type));
            memset (&smac_in, 0, sizeof(struct mac));
        }

    struct mac_in *p;
    list_for_each_entry(p,&smac_head,list)
        {
            printf ("type:%s\n", p->type);
            printf ("proto:%s\n", p->proto);
            printf ("source:%s\n", p->source);
            printf ("fid:%d\n", p->fid);
            printf ("macaddress:%s\n", p->macaddress);
            printf ("nexthoptype:%s\n", p->nexthoptype);
            printf ("nexthop:%d\n", p->nexthop);
            printf("-----------------------------\n");
        }
    return 0;
}
