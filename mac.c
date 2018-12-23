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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mac.h"
#include "tools.h"
#include "list.h"

int static_count = 0;

LIST_HEAD(mac_head);
LIST_HEAD(int_head);
LIST_HEAD(esi_head);
LIST_HEAD(out_head);

char *strproto[8] =
  { "VXLAN", "DOT1Q", NULL };

char *strsource[8] =
  { "STATIC", "LOCAL", "MLAG", "EVPN", NULL };

char *strnexthoptype[16] =
  { "INTERFACE", "ESI", NULL };

char *strinttype[16] =
  { "ETHERNET", "TUNNEL", NULL };

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
//  p = look_up_mac_in (s);
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
//  p = look_up_int_out (s);
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

//  sort_out_tab (p);
  list_add_tail (&p->list, &out_head);
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
  if (list_empty (&out_head))
    {
      list_add (&new->list, &out_head);
      return 0;
    }

  list_for_each_entry_safe(p,n,&out_head,list)
    {
      if (new->set.type == p->set.type && new->set.val == p->set.val)
        {
          if (strcmp (new->macaddress, p->macaddress) > 0)
            {
              list_add_tail (&new->list, &p->list);
              return 0;
            }
          else
            {
              list_add (&new->list, &p->list);
              return 0;
            }
        }
    }

  list_for_each_entry_safe(p,n,&out_head,list)
    {
      if (new->set.type == p->set.type)
        {
          if (new->set.val > p->set.val)
            {
              list_add_tail (&new->list, &p->list);
              return 0;
            }
          else
            {
              list_add (&new->list, &p->list);
              return 0;
            }
        }
    }

  list_for_each_entry_safe(p,n,&out_head,list)
    {
      if (new->set.type > p->set.type)
        {
          list_add_tail (&new->list, &p->list);
          return 0;
        }
      else
        {
          list_add (&new->list, &p->list);
          return 0;
        }
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

int
check_int_out (struct int_out *p)
{
  int valid;
  int i;
  valid = -1;
  for (i = 0; strinttype[i] != NULL; ++i)
    {
      if (strcmp (p->inttype, strinttype[i]) == 0)
        {
          valid = 0;
          break;
        }
    }
  if (valid < 0)
    {
      printf ("\n====================================inttype valid\n");
      return -1;
    }
  if (check_ifx_nexthop (p->ifx) < 0)
    {
      return -1;
    }
  return 0;
}

/* 过滤不合法的输入参数 */
int
check_mac_in (struct mac_in *p, int add_del)
{
  int valid = 0;
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
  valid = -1;
  int i;
  for (i = 0; strproto[i] != NULL; i++)
    {
      if (strcmp (p->proto, strproto[i]) == 0)
        {
          /*满足一个proto项即可*/
          valid = 0;
          break;
        }

    }
  if (valid < 0)
    {
      return -1;
    }
  /*检查source是否合法*/
  valid = -1;
  for (i = 0; strsource[i] != NULL; i++)
    {
      if (strcmp (p->source, strsource[i]) == 0)
        {
          /*满足一个source项即可*/
          valid = 0;
          break;
        }
    }

  if (valid < 0)
    {
      return -1;
    }
  /*添加时才检查*/
  if (add_del == _ADD)
    {
      /*检查nexthoptype是否合法*/
      valid = -1;
      for (i = 0; strnexthoptype[i] != NULL; i++)
        {
          if (strcmp (p->nexthoptype, strnexthoptype[i]) == 0)
            {
              /*满足一个source项即可*/
              valid = 0;
              break;
            }
        }
      if (valid < 0)
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

/*根据key:ifx查找接口节点*/
struct int_out *
look_up_by_intkey (int ifx)
{
  struct int_out *p, *n;
  list_for_each_entry_safe (p, n, &int_head,list)
    {
      if (p->ifx == ifx)
        {
          return p;
        }
    }
  return NULL;
}

/*根据key:name查找接口ESI节点*/
struct esi *
look_up_by_esikey (const char *name)
{
  struct esi *p, *n;
  list_for_each_entry_safe (p, n, &esi_head,list)
    {
      if (strcmp (p->name, name) == 0)
        {
          return p;
        }
    }
  return NULL;
}

int
cmp_proto_type (struct list_head *a, struct list_head *b)
{
  struct out_tab *pa = list_entry(a, struct out_tab, list);
  struct out_tab *pb = list_entry(b, struct out_tab, list);
  return (pa->set.type - pb->set.type);
}

int
cmp_proto_val (struct list_head *a, struct list_head *b)
{
  struct out_tab *pa = list_entry(a, struct out_tab, list);
  struct out_tab *pb = list_entry(b, struct out_tab, list);
  if (pa->set.type == pb->set.type)
    {
      return (pa->set.val - pb->set.val);
    }
  else
    {
      return 0;
    }
}

int
cmp_macaddress (struct list_head *a, struct list_head *b)
{
  struct out_tab *pa = list_entry(a, struct out_tab, list);
  struct out_tab *pb = list_entry(b, struct out_tab, list);
  if (pa->set.val == pb->set.val)
    {
      return (strcmp (pa->macaddress, pb->macaddress));
    }
  else
    {
      return 0;
    }
}

int
cmd_out (struct list_head *a, struct list_head *b)
{
  struct out_tab *pa = list_entry(a, struct out_tab, list);
  struct out_tab *pb = list_entry(b, struct out_tab, list);
  return 0;
}

void
swap (struct list_head *a, struct list_head *b)
{
  struct list_head flag =
    { NULL, NULL };
  __list_add (&flag, b->prev, b);
  list_del (b);
  __list_add (b, a->prev, a);
  list_del (a);
  __list_add (a, flag.prev, &flag);
  list_del (&flag);
}

void
insert_sort (struct list_head *head, int
(*cmp) (struct list_head *a, struct list_head *b))
{

  struct list_head *i, *j, *temp;
  i = head->next->next;   //i指向第2个结点
  list_for_each_from(i,head)
    { //i从第2个结点开始遍历,因为第1个已经有序
      j = i->prev;  //j指向i的前一个结点

      if (cmp (j, i) <= 0) //从表头开始，按照升序排列
        continue;
      list_for_each_reverse_continue(j,head)
        {
          if (cmp (j, i) <= 0)
            break;
        }
      temp = i->next; //因为下文要删除i结点，所以记录i结点的下一个结点
      list_del (i);
      __list_add (i, j, j->next); //把i插入到j的后面
      i = temp->prev; //i指针归位
    }
}

void
bubble_sort (struct list_head *head, int
(*compar) (struct list_head *, struct list_head *))
{
  struct list_head *start = NULL;
  struct list_head *end = NULL;
  list_for_each_reverse (end, head)
    {
      list_for_each(start, head)
        {
          if (start == end)
            break;

          if (compar (start, start->next) > 0)
            {
              swap (start, start->next);
              start = start->prev; //start归位
              if (start == end)
                end = end->next; //end归位
            }
        }
    }
}

int
parse_cmd (FILE *fp)
{
  struct mac_in pmac_in;
  memset (&pmac_in, 0, sizeof(struct mac_in));

  struct int_out pint_out;
  memset (&pint_out, 0, sizeof(struct int_out));

  struct esi pesi;
  memset (&pesi, 0, sizeof(struct esi));

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

              strcpy (pmac_in.type, "ADD-MAC");

              nRet = fscanf (fp, "%*s%s %*s%s %*s%d, %*s%s %*s%s %*s%s",
                             smac_in.proto, smac_in.source, &smac_in.fid,
                             smac_in.macaddress, smac_in.nexthoptype,
                             smac_in.nexthop);

              memcpy (pmac_in.proto, smac_in.proto + 1,
                      strlen (smac_in.proto) - 3);

              memcpy (pmac_in.source, smac_in.source + 1,
                      strlen (smac_in.source) - 3);

              pmac_in.fid = smac_in.fid;

              memcpy (pmac_in.macaddress, smac_in.macaddress + 1,
                      strlen (smac_in.macaddress) - 3);

              memcpy (pmac_in.nexthoptype, smac_in.nexthoptype + 1,
                      strlen (smac_in.nexthoptype) - 3);

              if (strcmp (pmac_in.nexthoptype, "INTERFACE") == 0)
                {
                  memcpy (pmac_in.nexthop, smac_in.nexthop,
                          strlen (smac_in.nexthop));
                }
              else if (strcmp (pmac_in.nexthoptype, "ESI") == 0)
                {
                  memcpy (pmac_in.nexthop, smac_in.nexthop + 1,
                          strlen (smac_in.nexthop) - 2);
                }

              /* 输入参数检查,入参错误时不加入链表*/
              valid = check_mac_in (&pmac_in, _ADD);

              if (valid < 0)
                {
                  valid = 0;
                  continue;
                }

              add_mac_in (&pmac_in);

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
                  memcpy (pint_out.ifname, sint_out.ifname + 1,
                          strlen (sint_out.ifname) - 2);
                }
              else if (strcmp (pint_out.inttype, "TUNNEL") == 0)
                {
                  nRet = fscanf (fp, "%*s%s", sint_out.peerip);
                  memcpy (pint_out.ifname, sint_out.ifname + 1,
                          strlen (sint_out.ifname) - 3);
                  memcpy (pint_out.peerip, sint_out.peerip + 1,
                          strlen (sint_out.peerip) - 2);
                }
              /*检查int_out是否合法*/
              valid = check_int_out (&pint_out);
              if (valid < 0)
                {
                  valid = 0;
                  continue;
                }
              add_int_out (&pint_out);

            }
          else if (strcmp (type, "\"ADD-ESI\",") == 0)
            {
              strcpy (pesi.type, "ADD-ESI");

              static_count = 0;

              char tmpbuf[1024];
              memset (tmpbuf, 0, sizeof(tmpbuf));

              nRet = fscanf (fp, "%*s%s %*s%d, %*s%[^]]", sesi.name,
                             &sesi.nexthopcount, tmpbuf);

              memcpy (pesi.name, sesi.name + 1, strlen (sesi.name) - 3);

              pesi.nexthopcount = sesi.nexthopcount;

              printf ("\n------------------>>>%s\n", tmpbuf);

              static_count = do_esi (tmpbuf, pesi.nexthopifx);
              printf ("\ncount------------%d\n", static_count);

              int i;
              for (i = 0; i < static_count; ++i)
                {
                  printf ("pesi.nexthopifx[i]=%d\n", pesi.nexthopifx[i]);
                }
              add_esi (&pesi, static_count);

            }
          else if (strcmp (type, "\"DEL-MAC\",") == 0)
            {

              strcpy (pmac_in.type, "DEL-MAC");

              nRet = fscanf (fp, "%*s%s %*s%s %*s%d, %*s%s", smac_in.proto,
                             smac_in.source, &smac_in.fid, smac_in.macaddress);

              memcpy (pmac_in.proto, smac_in.proto + 1,
                      strlen (smac_in.proto) - 3);

              memcpy (pmac_in.source, smac_in.source + 1,
                      strlen (smac_in.source) - 3);

              pmac_in.fid = smac_in.fid;

              memcpy (pmac_in.macaddress, smac_in.macaddress + 1,
                      strlen (smac_in.macaddress) - 2);

              /* 输入参数检查,入参错误时不加入链表*/
              valid = check_mac_in (&pmac_in, _DEL);
              if (valid < 0)
                {
                  valid = 0;
                  continue;
                }

              del_mac_in (&pmac_in);

            }
          else if (strcmp (type, "\"DEL-INT\",") == 0)
            {
              strcpy (pint_out.type, "DEL-INT");

              nRet = fscanf (fp, "%*s%d", &sint_out.ifx);

              pint_out.ifx = sint_out.ifx;

              /*检查ifx是否合法*/
              valid = check_ifx_nexthop (pint_out.ifx);
              if (valid < 0)
                {
                  valid = 0;
                  continue;
                }
              del_int_out (&pint_out);

            }
          else if (strcmp (type, "\"DEL-ESI\",") == 0)
            {
              strcpy (pesi.type, "DEL-ESI");

              nRet = fscanf (fp, "%*s%s", sesi.name);

              memcpy (pesi.name, sesi.name + 1, strlen (sesi.name) - 2);

              del_esi (&pesi);

            }
        }
      /*清除暫存變量值*/
      memset (type, 0, sizeof(type));
      memset (&smac_in, 0, sizeof(struct mac_in));
      memset (&pmac_in, 0, sizeof(struct mac_in));
      memset (&sint_out, 0, sizeof(struct int_out));
      memset (&pint_out, 0, sizeof(struct int_out));
      memset (&sesi, 0, sizeof(struct esi));
      memset (&pesi, 0, sizeof(struct esi));

    }
  return 0;
}

void
vid_vni_show (struct mac_type *tmp, char *p)
{
  if (tmp->type == 0)
    {
      //VID
      sprintf (p, "%d/%d", tmp->val, 0);
    }
  else
    {
      //VNI
      sprintf (p, "%d/%d", 0, tmp->val);
    }
}

int
deal_with_cmd (FILE *fp)
{
  struct out_tab stab;

  struct mac_in *pin, *nin;
  struct int_out *pout, *nout;
  struct esi *pesi, *nesi;
  char buf[32];
  int nRet = 0;

  memset (&stab, 0, sizeof(struct out_tab));

  struct mac_type tmp;
  memset (&tmp, 0, sizeof(struct mac_type));

  /*遍历mac_in*/
  list_for_each_entry_safe(pin,nin,&mac_head,list)
    {
      nRet = fid2mac_type (pin->fid, &tmp);
      printf ("\n++++++++++++++++++\n");
      printf ("fid=%d,tmp->type:%d,tmp->val:%d\n", pin->fid, tmp.type, tmp.val);
      printf ("\n++++++++++++++++++\n");
      if (nRet < 0)
        {
          continue;
        }
      if (strcmp (pin->nexthoptype, "INTERFACE") == 0)
        {
          pout = look_up_by_intkey (atoi (pin->nexthop));
          if (!pout)
            {
              /*mac表项出口不存在，怎么处理？*/
              //删除？？
              continue;
            }
          memset (buf, 0, sizeof(buf));
          vid_vni_show (&tmp, buf);
          strcpy (stab.strfid, buf);
          strcpy (stab.macaddress, pin->macaddress);
          strcpy (stab.source, pin->source);
          if (strcmp (pout->inttype, "ETHERNET") == 0)
            {
              strcpy (stab.nexthop, pout->ifname);
            }
          else if (strcmp (pout->inttype, "TUNNEL") == 0)
            {
              strcpy (stab.nexthop, pout->peerip);
            }
          stab.flg = 0;
          stab.set.val = tmp.val;
          stab.set.type = tmp.type;
          add_out_tab (&stab);
        }
      else if (strcmp (pin->nexthoptype, "ESI") == 0)
        {
          pesi = look_up_by_esikey (pin->nexthop);
          if (!pesi)
            {
              /*mac表项出口不存在，怎么处理？*/
              //删除？？
              continue;
            }
          memset (buf, 0, sizeof(buf));
          vid_vni_show (&tmp, buf);
          strcpy (stab.strfid, buf);
          strcpy (stab.macaddress, pin->macaddress);
          strcpy (stab.source, pin->source);
          int i;
          for (i = 0; i < static_count; ++i)
            {
              pout = look_up_by_intkey (pesi->nexthopifx[i]);
              if (!pout)
                {
                  /*mac表项出口不存在，怎么处理？*/
                  //删除？？
                  continue;
                }

              if (strcmp (pout->inttype, "ETHERNET") == 0)
                {
                  strcpy (stab.nexthop, pout->ifname);
                }
              else if (strcmp (pout->inttype, "TUNNEL") == 0)
                {
                  strcpy (stab.nexthop, pout->peerip);
                }
              /*添加out_table*/
              stab.flg = 1;
              stab.set.val = tmp.val;
              stab.set.type = tmp.type;
              add_out_tab (&stab);
            }
        }
    }

  return 0;
}

void
show (FILE *fp)
{
  struct out_tab *p, *n;

  int i = 0;

  bubble_sort (&out_head, cmp_proto_type);

  bubble_sort (&out_head, cmp_proto_val);

  bubble_sort (&out_head, cmp_macaddress);

  list_for_each_entry_safe(p,n,&out_head,list)
    {
#if 1
      if (p->flg != 0)
        {
          if (i == 0)
            {
              fprintf (fp, "%s %s %s %s\n", p->strfid, p->macaddress, p->source,
                       p->nexthop);
            }
          else
            {
              fprintf (fp, "                         %s\n", p->nexthop);
            }
          i++;
        }
      else
        {
          fprintf (fp, "%s %s %s %s\n", p->strfid, p->macaddress, p->source,
                   p->nexthop);
          /*清除此标志，等待下一次ESI*/
          i = 0;
        }
#else
      fprintf (fp, "%s %s %s %s\n", p->strfid, p->macaddress, p->source,
          p->nexthop);
#endif
    }
}

