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

struct list_head mac_head;
struct list_head int_head;
struct list_head esi_head;

static int st_priority = 1;

struct mac_type
{
    char type;
    int val;
};
enum
{
    _VID, _VNI
};
int
mac_type2fid (struct mac_type *p)
{
    if (p->type == _VID)
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
fid2mac_type (int fid, struct mac_type **tmp)
{
    int nRet;
    *tmp = (struct mac_type *) malloc (sizeof(struct mac_type));
    if (!(*tmp))
        {
            printf ("struct mac_type malloc 失败!\n");
            return -1;
        }
    (*tmp)->type = (char) (fid >> 24);
    (*tmp)->val = (fid & 0xffffff);
    nRet = mac_type2fid (*tmp);
    if (nRet < 0)
        {
            /*内存申请成功，但是mac_type2fid返回数据不合法*/
            return 1;
        }
    return 0;
}

int
check_ifx_nexthop (int ifx)
{
    if (ifx < 1 || ifx > 65535)
        {
            return -1;
        }
    return 0;
}
/* 过滤不合法的输入参数 */
int
check_mac_in (struct mac_in *p)
{
    int valid;
    valid = 0;
    struct mac_type *tmp;
    /*检查fid是否合法*/
    valid = fid2mac_type (p->fid, &tmp);
    if (valid == -2)
        {
            free (tmp);
            return -1;
        }
    if (valid == -1)
        {
            printf ("fid2mac_type malloc error!\n");
            return -1;
        }
    /*成功也得释放*/
    free (tmp);
    /*检查proto是否合法*/
    int i;
    for (i = 0; i < sizeof(strproto) / sizeof(strproto[0]); i++)
        {
            if (strcmp (p->proto, strproto[i]) == 0)
                {
                    /*满足一个proto项即可*/
                    valid = 0;
                    break;
                }
        }
    if (valid != 0)
        {
            return -1;
        }
    /*检查source是否合法*/
    valid = 0;
    for (i = 0; i < sizeof(strsource) / sizeof(strsource[0]); ++i)
        {
            if (strcmp (p->source, strsource[i]) == 0)
                {
                    /*满足一个source项即可*/
                    valid = 0;
                    break;
                }
        }
    if (valid != 0)
        {
            return -1;
        }
    /*检查nexthoptype是否合法*/
    valid = 0;
    for (i = 0; i < sizeof(strnexthoptype) / sizeof(strnexthoptype[0]); ++i)
        {
            if (strcmp (p->nexthoptype, strnexthoptype[i]) == 0)
                {
                    /*满足一个source项即可*/
                    valid = 0;
                    break;
                }
        }
    if (valid != 0)
        {
            return -1;
        }

    /*检查nexthop是否合法*/
    if (check_ifx_nexthop (p->nexthop) < 0)
        {
            return -1;
        }
    return 0;
}

int
parse_cmd (FILE *fp)
{
    struct mac_in *pmac_in;
    struct int_out *pint_out;
    struct esi *pesi;

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
                            pmac_in = (struct mac_in *) malloc (
                                    sizeof(struct mac_in));
                            if (!pmac_in)
                                {
                                    return -1;
                                }
                            pmac_in->priority = 0;

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

                            /* 输入参数检查,入参错误时不加入链表*/
                            valid = check_mac_in (pmac_in);
                            if (valid < 0)
                                {
                                    valid = 0;
                                    continue;
                                }

                            if (strcmp (pmac_in->source, "STATIC") == 0)
                                {
                                    /*不存在多个STATIC的情况*/
//                                    pmac_in->priority = 0;
                                }
                            else
                                {
                                    /*判断优先级*/
                                    st_priority++;
                                    pmac_in->priority = st_priority;
                                }

                            list_add (&pmac_in->list, &mac_head);

                        }
                    else if (strcmp (type, "\"ADD-INT\",") == 0)
                        {
                            pint_out = (struct int_out *) malloc (
                                    sizeof(struct int_out));
                            if (!pint_out)
                                {
                                    return -1;
                                }
                            strcpy (pint_out->type, "ADD-INT");

                            nRet = fscanf (fp, "%*s%s %*s%d,", sint_out.inttype,
                                           &sint_out.ifx);

                            memcpy (pint_out->inttype, sint_out.inttype + 1,
                                    strlen (sint_out.inttype) - 3);
                            pint_out->ifx = sint_out.ifx;

                            nRet = fscanf (fp, "%*s%s", sint_out.ifname);

                            /*以太网口，显示接口,隧道则显示peerip*/
                            if (strcmp (pint_out->inttype, "ETHERNET") == 0)
                                {
                                    memcpy (pint_out->ifname,
                                            sint_out.ifname + 1,
                                            strlen (sint_out.ifname) - 2);
                                }
                            else if (strcmp (pint_out->inttype, "TUNNEL") == 0)
                                {
                                    nRet = fscanf (fp, "%*s%s",
                                                   sint_out.peerip);
                                    memcpy (pint_out->ifname,
                                            sint_out.ifname + 1,
                                            strlen (sint_out.ifname) - 3);
                                    memcpy (pint_out->peerip,
                                            sint_out.peerip + 1,
                                            strlen (sint_out.peerip) - 2);
                                }
                            /*检查ifx是否合法*/
                            valid = check_ifx_nexthop (pint_out->ifx);
                            if (valid < 0)
                                {
                                    valid = 0;
                                    continue;
                                }

                            list_add (&pint_out->list, &int_head);

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
        }
    return 0;
}

int
conver_filename (char *infile, char *outfile)
{

    char *token;
    char *outpath = outfile;
    if ((token = strsep (&infile, ".")) != NULL)
        {
            sprintf (outpath, "%s%s", token, "_result111.txt");
        }
    else
        {
            return -1;
        }
    return 0;
}

int
deal_with_cmd (FILE *fp)
{
    struct mac_in *pin, *nin;
    struct int_out *pout, *nout;
    int list_max = 0;

    struct mac_type *tmp;
    int nRet = 0;
    memset (&tmp, 0, sizeof(struct mac_type));
    /*遍历int*/
    list_for_each_entry_safe(pout,nout,&int_head,list)
        {
            list_for_each_entry_safe(pin,nin,&mac_head,list)
                {
                    /*接口出口存在的时候才可连接*/
                    if (pout->ifname && pout->ifx && pout->ifx == pin->nexthop)
                        {
                            nRet = fid2mac_type (pin->fid, &tmp);
                            if (nRet >= 0)
                                {
                                    if (pin->priority == 0)
                                        {
                                            if (strcmp (pout->inttype,
                                                        "ETHERNET") == 0)
                                                {
                                                    fprintf (fp,
                                                             "%d/%d %s %s %s\n",
                                                             pin->fid,
                                                             tmp->type,
                                                             pin->macaddress,
                                                             pin->source,
                                                             pout->ifname);
                                                }
                                            else
                                                {
                                                    fprintf (fp,
                                                             "%d/%d %s %s %s\n",
                                                             pin->fid,
                                                             tmp->type,
                                                             pin->macaddress,
                                                             pin->source,
                                                             pout->peerip);
                                                }
                                            list_del_init (&pin->list);
                                        }
                                    else
                                        {
                                            list_max = MAX(list_max,
                                                           pin->priority);
                                            printf ("-----------MAX= %d\n",
                                                    list_max);
                                        }
                                    free (tmp);
                                }
                        }
                }
        }

    /*遍历2 int*/
    list_for_each_entry(pout,&int_head,list)
        {
            list_for_each_entry(pin,&mac_head,list)
                {
                    /*接口出口存在的时候才可连接*/
                    if (pout->ifname && pout->ifx && pout->ifx == pin->nexthop)
                        {
                            nRet = fid2mac_type (pin->fid, &tmp);
                            if (nRet >= 0)
                                {
//                                    if (pin->priority == list_max)
                                        {
                                            printf (" max= %d\n", list_max);
                                            if (strcmp (pout->inttype,
                                                        "ETHERNET") == 0)
                                                {
                                                    fprintf (
                                                            fp,
                                                            "%d/%d %s %s %s %d\n",
                                                            pin->fid, tmp->type,
                                                            pin->macaddress,
                                                            pin->source,
                                                            pout->ifname,
                                                            pin->priority);
                                                }
                                            else
                                                {
                                                    fprintf (
                                                            fp,
                                                            "%d/%d %s %s %s %d\n",
                                                            pin->fid, tmp->type,
                                                            pin->macaddress,
                                                            pin->source,
                                                            pout->peerip,
                                                            pin->priority);
                                                }

                                        }
                                    free (tmp);
                                }
                        }
                }
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
    deal_with_cmd (outfp);
    fclose (outfp);
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

    return 0;
}
