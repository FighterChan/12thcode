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
#include "tools.h"

static int st_priority = 1;

int
parse_cmd (FILE *fp)
{
    struct mac_in pmac_in;
    memset (&pmac_in, 0, sizeof(struct mac_in));

    struct int_out pint_out;
    memset (&pint_out, 0, sizeof(struct int_out));

    struct esi pesi;
    memset (&pesi, 0, sizeof(struct esi));

    INIT_LIST_HEAD (&mac_head);
    INIT_LIST_HEAD (&int_head);
    INIT_LIST_HEAD (&esi_head);

    int nRet;
    int valid;
    char type[64];

    memset (type, 0, sizeof(type));
    struct mac_in smac_in;
    memset (&smac_in, 0, sizeof(struct mac_in));
    struct int_out sint_out;
    memset (&sint_out, 0, sizeof(struct int_out));
    struct esi sesi;
    memset (&sesi, 0, sizeof(struct esi));
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
                            pmac_in.priority = 0;

                            strcpy (pmac_in.type, "ADD-MAC");

                            nRet = fscanf (
                                    fp, "%*s%s %*s%s %*s%d, %*s%s %*s%s %*s%d",
                                    smac_in.proto, smac_in.source, &smac_in.fid,
                                    smac_in.macaddress, smac_in.nexthoptype,
                                    &smac_in.nexthop);

                            memcpy (pmac_in.proto, smac_in.proto + 1,
                                    strlen (smac_in.proto) - 3);

                            memcpy (pmac_in.source, smac_in.source + 1,
                                    strlen (smac_in.source) - 3);

                            pmac_in.fid = smac_in.fid;

                            memcpy (pmac_in.macaddress, smac_in.macaddress + 1,
                                    strlen (smac_in.macaddress) - 3);

                            memcpy (pmac_in.nexthoptype,
                                    smac_in.nexthoptype + 1,
                                    strlen (smac_in.nexthoptype) - 3);

                            pmac_in.nexthop = smac_in.nexthop;

                            /* 输入参数检查,入参错误时不加入链表*/
                            valid = check_mac_in (&pmac_in);
                            if (valid < 0)
                                {
                                    valid = 0;
                                    continue;
                                }

                            if (strcmp (pmac_in.source, "STATIC") == 0)
                                {
                                    /*不存在多个STATIC的情况*/
//                                    pmac_in.priority = 0;
                                }
                            else
                                {
                                    /*判断优先级*/
                                    st_priority++;
                                    pmac_in.priority = st_priority;
                                }
                            add_mac_in (&pmac_in);
//                            list_add_tail (&pmac_in->list, &mac_head);

                        }
                    else if (strcmp (type, "\"ADD-INT\",") == 0)
                        {
                            strcpy (pint_out.type, "ADD-INT");

                            nRet = fscanf (fp, "%*s%s %*s%d,", sint_out.inttype,
                                           &sint_out.ifx);

                            memcpy (pint_out.inttype, sint_out.inttype + 1,
                                    strlen (sint_out.inttype) - 3);
                            pint_out.ifx = sint_out.ifx;

                            nRet = fscanf (fp, "%*s%s", sint_out.ifname);

                            /*以太网口，显示接口,隧道则显示peerip*/
                            if (strcmp (pint_out.inttype, "ETHERNET") == 0)
                                {
                                    memcpy (pint_out.ifname,
                                            sint_out.ifname + 1,
                                            strlen (sint_out.ifname) - 2);
                                }
                            else if (strcmp (pint_out.inttype, "TUNNEL") == 0)
                                {
                                    nRet = fscanf (fp, "%*s%s",
                                                   sint_out.peerip);
                                    memcpy (pint_out.ifname,
                                            sint_out.ifname + 1,
                                            strlen (sint_out.ifname) - 3);
                                    memcpy (pint_out.peerip,
                                            sint_out.peerip + 1,
                                            strlen (sint_out.peerip) - 2);
                                }
                            /*检查ifx是否合法*/
                            valid = check_ifx_nexthop (pint_out.ifx);
                            if (valid < 0)
                                {
                                    valid = 0;
                                    continue;
                                }
                            add_int_out (&pint_out);
//                            list_add_tail (&pint_out.list, &int_head);

                        }
                    else if (strcmp (type, "\"ADD-ESI\",") == 0)
                        {
//                            pesi = (struct esi *) malloc (
//                                    sizeof(struct esi));
//                            if (!pesi)
//                                {
//                                    return -1;
//                                }
//                            strcpy (pesi->type, "ADD-ESI");
//
//                            nRet = fscanf (
//                                    fp, "%*s%s %*s%s %*s%d, %*s%s %*s%s %*s%d",
//                                    smac_in.proto, smac_in.source, &smac_in.fid,
//                                    smac_in.macaddress, smac_in.nexthoptype,
//                                    &smac_in.nexthop);
//
//                            memcpy (pmac_in->proto, smac_in.proto + 1,
//                                    strlen (smac_in.proto) - 3);
//
//                            memcpy (pmac_in->source, smac_in.source + 1,
//                                    strlen (smac_in.source) - 3);
//
//                            pmac_in->fid = smac_in.fid;
//
//                            memcpy (pmac_in->macaddress, smac_in.macaddress + 1,
//                                    strlen (smac_in.macaddress) - 3);
//
//                            memcpy (pmac_in->nexthoptype,
//                                    smac_in.nexthoptype + 1,
//                                    strlen (smac_in.nexthoptype) - 3);
//
//                            pmac_in->nexthop = smac_in.nexthop;
//
//                            list_add_tail (&pmac_in->list, &smac_head);

                        }

                }
            memset (type, 0, sizeof(type));
            memset (&smac_in, 0, sizeof(struct mac_in));
            memset (&pmac_in, 0, sizeof(struct mac_in));
            memset (&sint_out, 0, sizeof(struct int_out));
            memset (&pint_out, 0, sizeof(struct int_out));
        }
    return 0;
}

void
vid_vni_show (struct mac_type *tmp, char *p)
{
    if (tmp->type == 0)
        {
            //VID
            sprintf (p, "%d/%d", tmp->val, tmp->type);
        }
    else
        {
            //VNI
            sprintf (p, "%d/%d", tmp->type, tmp->val);
        }
}
int
deal_with_cmd (FILE *fp)
{
    struct mac_in *pin, *nin;
    struct int_out *pout, *nout;
    int list_max = 0;
    char buf[32];

    struct mac_type tmp;
    memset (&tmp, 0, sizeof(struct mac_type));
    /*遍历int*/
    list_for_each_entry_safe(pout,nout,&int_head,list)
        {
            list_for_each_entry_safe(pin,nin,&mac_head,list)
                {
                    /*接口出口存在的时候才可连接*/
                    if (pout->ifname && pout->ifx && pout->ifx == pin->nexthop)
                        {
                            fid2mac_type (pin->fid, &tmp);

                            memset (buf, 0, sizeof(buf));
                            vid_vni_show (&tmp, buf);
                            if (strcmp (pin->source, "STATIC") == 0)
                                {
                                    if (strcmp (pout->inttype, "ETHERNET") == 0)
                                        {
                                            fprintf (fp, "%s %s %s %s\n", buf,
                                                     pin->macaddress,
                                                     pin->source, pout->ifname);
                                        }
                                    else
                                        {
                                            fprintf (fp, "%s %s %s %s\n",
                                                     buf, pin->macaddress,
                                                     pin->source, pout->peerip);
                                        }
                                    list_del_init (&pin->list);
                                }
                            else
                                {
                                    list_max = MAX(list_max, pin->priority);
                                    printf ("-----------MAX= %d\n", list_max);
                                }
                        }
                }
        }
#if 1
    memset (&tmp, 0, sizeof(struct mac_type));

    /*遍历2 int*/
    list_for_each_entry(pout,&int_head,list)
        {
            list_for_each_entry(pin,&mac_head,list)
                {
                    /*接口出口存在的时候才可连接*/
                    if (pout->ifname && pout->ifx && pout->ifx == pin->nexthop)
                        {
                            fid2mac_type (pin->fid, &tmp);
                            memset (buf, 0, sizeof(buf));
                            vid_vni_show (&tmp, buf);
//                            if (pin->priority == list_max)
                                {
                                    printf (" max= %d\n", list_max);
                                    if (strcmp (pout->inttype, "ETHERNET") == 0)
                                        {
                                            fprintf (fp, "%s %s %s %s\n",
                                                     buf,
                                                     pin->macaddress,
                                                     pin->source, pout->ifname);
                                        }
                                    else
                                        {
                                            fprintf (fp, "%s %s %s %s\n",
                                                     buf,
                                                     pin->macaddress,
                                                     pin->source, pout->peerip);
                                        }

                                }
                        }
                }
        }
#endif
    return 0;
}

int
main (int argc, char **argv)
{
    char outpath[32];
    memset (outpath, 0, sizeof(outpath));

    if (argc != 2)
        {
            printf ("parameter error!\n");
            return -1;
        }
    FILE *infp;
    infp = fopen (argv[1], "rb");
    if (!infp)
        {
            printf ("can not open!\n");
            return -1;
        }

    /*解析文件*/
    parse_cmd (infp);
    fclose (infp);
    /*处理*/
    conver_filename (argv[1], outpath);
    FILE *outfp;
    outfp = fopen (outpath, "w");
    if (!outfp)
        {
            printf ("can not open file!\n");
            return -1;
        }

//    list_free();
    struct mac_in *p;
    list_for_each_entry(p,&mac_head,list)
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

    struct int_out *p2;
    list_for_each_entry(p2,&int_head,list)
        {
            printf ("type:%s\n", p2->type);
            printf ("inttype:%s\n", p2->inttype);
            printf ("ifx:%d\n", p2->ifx);
            printf ("ifname:%s\n", p2->ifname);
            printf ("peerip:%s\n", p2->peerip);
            printf ("-----------------------------\n");
        }

    deal_with_cmd (outfp);
    fclose (outfp);

    return 0;
}
