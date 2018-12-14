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
    strcpy (p->nexthop, s->nexthop);
    return 0;
}

struct mac_in *
look_up_mac_in (struct mac_in *s)
{

    struct mac_in *p, *new;
    struct mac_in *n;
    u32 key, keytmp;
    key = get_mac_in_key (s->fid, s->macaddress);
    printf ("mackey = %ld\n", key);
    list_for_each_entry_safe(p, n, &mac_head,list)
        {
            keytmp = get_mac_in_key (p->fid, p->macaddress);
            if (key == keytmp)
                {
                    if (strcmp (p->source, "STATIC") == 0)
                        {
                            /*优先级最高，不更新*/
                            return p;
                        }
                    else
                        {
                            /*后来者优先级较高，更新*/
                            copy_to_mac_in (p, s);
                            return p;
                        }

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
            return 0;
        }
    return -1;
}

int
del_mac_in (struct mac_in *s)
{
    struct mac_in *p = NULL;
    struct mac_in *n;
    int key;
    int keytmp;
    key = get_mac_in_key (s->fid, s->macaddress);
    printf ("del mackey = %d\n", key);
    list_for_each_entry_safe(p, n, &mac_head,list)
        {
            keytmp = get_mac_in_key (p->fid, p->macaddress);
            if (key == keytmp)
                {
                    list_del_init (&p->list);
                    free (p);
                }
        }
    return -1;
}

int
free_mac_in (void)
{
    struct mac_in *p;
    struct mac_in *n;

    list_for_each_entry_safe(p,n,&mac_head,list)
        {
            list_del_init (&p->list);
            free (p);
            p = NULL;
        }
    return 0;
}

u32
get_mac_in_key (int fid, const char *mac_addr)
{
    struct mac_type tmp;
    memset (&tmp, 0, sizeof(struct mac_type));
    fid2mac_type (fid, &tmp);
    return (jhash_2words (tmp.val, jhash (mac_addr, strlen (mac_addr), 0), 0)
            % 1024);
}

/*出接口*/
int
copy_to_int_out (struct int_out *p, struct int_out *s)
{

    strcpy (p->type, s->type);
    strcpy (p->inttype, s->inttype);
    p->ifx = s->ifx;
    strcpy (p->ifname, s->ifname);
    strcpy (p->peerip, s->peerip);
    return 0;
}

struct int_out *
look_up_int_out (struct int_out *s)
{

    struct int_out *p;
    struct int_out *n;
    u32 key, keytmp;
    key = get_int_out_key (s->ifx);
    printf ("intkey = %ld\n", key);
    list_for_each_entry_safe(p, n, &int_head,list)
        {
            keytmp = get_int_out_key (p->ifx);
            if (key == keytmp)
                {
                    copy_to_int_out (p, s);
                    return p;
                }
        }
    return NULL;
}

int
add_int_out (struct int_out *s)
{
    struct int_out *p = NULL;
    struct int_out *n;
    p = look_up_int_out (s);
    if (p == NULL)
        {
            p = (struct int_out *) malloc (sizeof(struct int_out));
            if (p == NULL)
                {
                    return -1;
                }
            copy_to_int_out (p, s);
            list_add_tail (&p->list, &int_head);
            return 0;
        }
    else
        {
            return 0;
        }
    return -1;
}

int
del_int_out (struct int_out *s)
{
    struct int_out *p, *n;

    list_for_each_entry_safe(p, n, &int_head,list)
        {
            if (s->ifx == p->ifx)
                {
                    list_del_init (&p->list);
                    free (p);
                }
        }
    return 0;
}

int
free_int_out (void)
{
    struct int_out *p;
    struct int_out *n;

    list_for_each_entry_safe(p,n,&int_head,list)
        {
            list_del_init (&p->list);
            free (p);
            p = NULL;
        }
    return 0;
}

u32
get_int_out_key (int ifx)
{
    return (jhash_1word (ifx, 0) % 1024);
}

/*ESI*/
int
copy_to_esi (struct esi *p, struct esi *s, int count)
{
    int i;
    strcpy (p->type, s->type);
    strcpy (p->name, s->name);
    p->nexthopcount = s->nexthopcount;
    for (i = 0; i < count; ++i)
        {
            p->nexthopifx[i] = s->nexthopifx[i];
        }
    return 0;
}

int
add_esi (struct esi *s, int count)
{
    struct esi *p = NULL;
    struct esi *n;
    if (p == NULL)
        {
            p = (struct esi *) malloc (sizeof(struct esi));
            if (p == NULL)
                {
                    return -1;
                }
            copy_to_esi (p, s, count);
            list_add_tail (&p->list, &esi_head);
            return 0;
        }
    else
        {
            return 0;
        }
    return -1;
}

int
del_esi (struct esi *s)
{
    struct esi *p, *n;

    list_for_each_entry_safe(p, n, &esi_head,list)
        {
            if (strcmp (s->name, p->name) == 0)
                {
                    list_del_init (&p->list);
                    free (p);
                }
        }
    return 0;
}

int
free_esi (void)
{
    struct esi *p;
    struct esi *n;

    list_for_each_entry_safe(p,n,&esi_head,list)
        {
            list_del_init (&p->list);
            free (p);
            p = NULL;
        }
    return 0;
}

/*输出链表*/
int
copy_to_out_tab (struct out_tab *p, struct out_tab *s)
{
    strcpy (p->strfid, s->strfid);
    strcpy (p->macaddress, s->macaddress);
    strcpy (p->source, s->source);
    strcpy (p->nexthop, s->nexthop);
    p->flg = s->flg;
    p->set.type = s->set.type;
    p->set.val = s->set.val;
    return 0;
}

int
add_out_tab (struct out_tab *s)
{
    struct out_tab *p;
    p = (struct out_tab *) malloc (sizeof(struct out_tab));
    if (!p)
        {
            return -1;
        }

    copy_to_out_tab (p, s);
    sort_out_tab (p);
//    list_add_tail (&p->list, &out_head);
    return 0;
}

int
free_out_tab (void)
{
    struct out_tab *p;
    struct out_tab *n;

    list_for_each_entry_safe(p,n,&out_head,list)
        {
            list_del_init (&p->list);
            free (p);
            p = NULL;
        }
    return 0;
}

void
free_list (void)
{
    free_mac_in ();
    free_int_out ();
    free_esi ();
    free_out_tab ();
}

/*out_tab排序
 * 1.vid/vni 類型相同，直接排出
 * 2.vid/vni 大小
 * 3.macaddr
 *
 * */
int
sort_out_tab (struct out_tab *new)
{

    struct out_tab *p, *n;
    struct out_tab *tmp;
    if (list_empty (&out_head))
        {
            list_add_tail (&new->list, &out_head);
            return 0;
        }
    list_for_each_entry_safe(p,n,&out_head,list)
        {
            if (new->set.type == p->set.type && new->set.val == p->set.val)
                {
                    tmp = p;
                    if (strcmp (new->macaddress, p->macaddress) > 0)
                        {
                            list_add (&new->list, &p->list);
                            return 0;
                        }
                }
        }

    if (tmp != NULL)
        {
            list_add_tail (&new->list, &tmp->list);
            return 0;
        }

    tmp = NULL;
    list_for_each_entry_safe(p,n,&out_head,list)
        {
            if (new->set.type == p->set.type)
                {
                    tmp = p;
                    if (new->set.val > p->set.val)
                        {
                            list_add (&new->list, &p->list);
                            return 0;
                        }
                }
        }
    if (tmp != NULL)
        {
            list_add_tail (&new->list, &tmp->list);
            return 0;
        }

    tmp = NULL;
    list_for_each_entry_safe(p,n,&out_head,list)
        {
            tmp = p;
            if (new->set.type > p->set.type)
                {
                    list_add (&new->list, &p->list);
                    return 0;
                }
        }
    if (tmp != NULL)
        {
            list_add_tail (&new->list, &tmp->list);
            return 0;
        }
    list_add_tail (&new->list, &out_head);
    return 0;
}

/* 过滤不合法的输入参数 */
int
check_mac_in (struct mac_in *p, int add_del)
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
    /*添加时才检查*/
    if (add_del == _ADD)
        {
            /*检查nexthoptype是否合法*/
            valid = 0;
            for (i = 0; i < sizeof(strnexthoptype) / sizeof(strnexthoptype[0]);
                    ++i)
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
            if (strcmp (p->nexthoptype, "INTERFACE") == 0)
                {
                    if (check_ifx_nexthop (atoi (p->nexthop)) < 0)
                        {
                            return -1;
                        }
                }
            else if (strcmp (p->nexthoptype, "ESI") == 0)
                {
                    if (!(p->nexthop))
                        {
                            return -1;
                        }
                }

        }
    return 0;
}
#endif
