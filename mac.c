/* 
 * mac.c
 * Original Author: chenwei3@ruijie.com.cn, 2018-12-12
 *
 * 
 * 
 *
 * History 
 * 
 *  
 */
#if 1
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mac.h"
#include "tools.h"

char *strproto[8] =
    { "VXLAN", "DOT1Q" };

char *strsource[8] =
    { "STATIC", "LOCAL", "MLAG", "EVPN" };

char *strnexthoptype[16] =
    { "INTERFACE", "ESI" };

int
copy_to_mac_in (struct mac_in *p, struct mac_in *s)
{

    strcpy (p->type, s->type);
    strcpy (p->proto, s->proto);
    strcpy (p->source, s->source);
    p->fid = s->fid;
    strcpy (p->macaddress, s->macaddress);
    strcpy (p->nexthoptype, s->nexthoptype);
    p->nexthop = s->nexthop;
    return 0;
}

struct mac_in *
look_up_mac_in (struct mac_in *s)
{

    struct mac_in *p;
    struct mac_in *n;
    u32 key, keytmp;
    key = get_mac_in_key (s->fid, s->macaddress);
    printf("key = %d\n",key);
    list_for_each_entry_safe(p, n, &mac_head,list)
        {
            keytmp = get_mac_in_key (p->fid, p->macaddress);
            if (key == keytmp)
                {
                    return p;
                }
        }
    return NULL;
}

int
add_mac_in (struct mac_in *s)
{
    struct mac_in *p = NULL;
    struct mac_in *n;
    p = look_up_mac_in (s);
    if (p == NULL)
        {
            p = (struct mac_in *) malloc (sizeof(struct mac_in));
            if (p == NULL)
                {
                    return -1;
                }
            copy_to_mac_in (p, s);
            list_add_tail (&p->list, &mac_head);
            return 0;
        }
    else
        {
            copy_to_mac_in (p, s);
            return 0;
        }
    return -1;
}

u32
get_mac_in_key (int fid, const char *mac_addr)
{
    struct mac_type tmp;
    memset (&tmp, 0, sizeof(struct mac_type));
    fid2mac_type (fid, &tmp);
    printf("vid = %d\n",tmp.val);
    return (jhash_2words (tmp.val, jhash (mac_addr, strlen (mac_addr), 0), 0)%1024);
}

/* 过滤不合法的输入参数 */
int
check_mac_in (struct mac_in *p)
{
    int valid;
    valid = 0;
    struct mac_type tmp;
    memset (&tmp, 0, sizeof(struct mac_type));

    /*检查fid是否合法*/
    valid = fid2mac_type (p->fid, &tmp);
    if (valid < 0)
        {
            printf ("fid2mac_type not valid!\n");
            return -1;
        }
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
#endif
