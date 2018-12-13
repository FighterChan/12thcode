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

struct list_head smac_head;

int
parse_cmd (FILE *fp)
{
    struct mac_in *pmac_in;

    INIT_LIST_HEAD (&smac_head);
    int nRet;
    char type[64];

    memset (type, 0, sizeof(type));
    struct mac_in smac_in;
    memset (&smac_in, 0, sizeof(struct mac_in));
    while (1)
        {

            nRet = fscanf (fp, "%s", type);
            if (nRet < 0)
                {
                    break;
                }

            if (strcmp ("type:", type) == 0)
                {
                    nRet = fscanf (fp, "%s", type);

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
                                    fp, "%*s%s %*s%s %*s%d, %*s%s %*s%s %*s%d",
                                    smac_in.proto, smac_in.source, &smac_in.fid,
                                    smac_in.macaddress, smac_in.nexthoptype,
                                    &smac_in.nexthop);

                            memcpy (pmac_in->proto, smac_in.proto + 1,
                                    strlen (smac_in.proto) - 3);

                            memcpy (pmac_in->source, smac_in.source + 1,
                                    strlen (smac_in.source) - 3);

                            pmac_in->fid = smac_in.fid;

                            memcpy (pmac_in->macaddress, smac_in.macaddress + 1,
                                    strlen (smac_in.macaddress) - 3);

                            memcpy (pmac_in->nexthoptype,
                                    smac_in.nexthoptype + 1,
                                    strlen (smac_in.nexthoptype) - 3);

                            pmac_in->nexthop = smac_in.nexthop;

                            list_add_tail (&pmac_in->list, &smac_head);

                        }
                }
            else if (strcmp (type, "\"ADD-INT\",") == 0)
                {

                    nRet = fscanf (fp, "%s", type);

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
                                    fp,
                                    "%*s%s %*s%s %*s%d, %*s%s %*s%s %*s%d",
                                    smac_in.proto, smac_in.source, &smac_in.fid,
                                    smac_in.macaddress, smac_in.nexthoptype,
                                    &smac_in.nexthop);

                            memcpy (pmac_in->proto, smac_in.proto + 1,
                                    strlen (smac_in.proto) - 3);

                            memcpy (pmac_in->source, smac_in.source + 1,
                                    strlen (smac_in.source) - 3);

                            pmac_in->fid = smac_in.fid;

                            memcpy (pmac_in->macaddress, smac_in.macaddress + 1,
                                    strlen (smac_in.macaddress) - 3);

                            memcpy (pmac_in->nexthoptype,
                                    smac_in.nexthoptype + 1,
                                    strlen (smac_in.nexthoptype) - 3);

                            pmac_in->nexthop = smac_in.nexthop;

                            list_add_tail (&pmac_in->list, &smac_head);

                        }

                }
            else if (strcmp (type, "\"ADD-MAC\",") == 0)
                {

                    nRet = fscanf (fp, "%s", type);

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
                                    fp,
                                    "%*s%s %*s%s %*s%d, %*s%s %*s%s %*s%d",
                                    smac_in.proto, smac_in.source, &smac_in.fid,
                                    smac_in.macaddress, smac_in.nexthoptype,
                                    &smac_in.nexthop);

                            memcpy (pmac_in->proto, smac_in.proto + 1,
                                    strlen (smac_in.proto) - 3);

                            memcpy (pmac_in->source, smac_in.source + 1,
                                    strlen (smac_in.source) - 3);

                            pmac_in->fid = smac_in.fid;

                            memcpy (pmac_in->macaddress, smac_in.macaddress + 1,
                                    strlen (smac_in.macaddress) - 3);

                            memcpy (pmac_in->nexthoptype,
                                    smac_in.nexthoptype + 1,
                                    strlen (smac_in.nexthoptype) - 3);

                            pmac_in->nexthop = smac_in.nexthop;

                            list_add_tail (&pmac_in->list, &smac_head);

                        }

                }
            memset (type, 0, sizeof(type));
            memset (&smac_in, 0, sizeof(struct mac_in));
        }
    return 0;
}

void
dotest ()
{
    int f = 0;
    char out[16];
    char out2[16];
    char *buf = "type: \"ADD-MAC\","
            "\r\n proto: 123,";
    sscanf (buf, "%*s%*s%*s%d", &f);
    printf ("%d\n", f);

    while (1)
        ;
}
int
main (int argc, char **argv)
{
#if 0
    dotest();
#else
    FILE *infp;
    infp = fopen (argv[1], "rb");
    if (!infp)
        {
            printf ("can not open!\n");
            return -1;
        }
    parse_cmd (infp);
#endif
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
            printf ("-----------------------------\n");
        }
    return 0;
}
