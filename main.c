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

int static_count = 0;

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
  INIT_LIST_HEAD (&out_head);

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
              /*检查ifx是否合法*/
              valid = check_ifx_nexthop (pint_out.ifx);
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

              char tmpbuf[512];
              memset (tmpbuf, 0, sizeof(tmpbuf));

              nRet = fscanf (fp, "%*s%s %*s%d, %*s%[^]]", sesi.name,
                             &sesi.nexthopcount, tmpbuf);

              memcpy (pesi.name, sesi.name + 1, strlen (sesi.name) - 3);

              pesi.nexthopcount = sesi.nexthopcount;

              printf ("\n------------------>>>%s\n", tmpbuf);

//                            int count;
              static_count = do_esi (tmpbuf, pesi.nexthopifx);
              printf ("\ncount------------%d\n", static_count);

              printf ("pesi.nexthopifx[0]=%d\n", pesi.nexthopifx[0]);
              printf ("pesi.nexthopifx[1]=%d\n", pesi.nexthopifx[1]);
//                            static_count = count;
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

  int next_hop = 0;
  memset (&stab, 0, sizeof(struct out_tab));

  struct mac_type tmp;
  memset (&tmp, 0, sizeof(struct mac_type));
  /*遍历int*/
  list_for_each_entry_safe(pout,nout,&int_head,list)
    {
      list_for_each_entry_safe(pin,nin,&mac_head,list)
        {
          if (strcmp (pin->nexthoptype, "INTERFACE") == 0)
            {
              next_hop = atoi (pin->nexthop);
              if (pout->ifx == next_hop)
                {
                  fid2mac_type (pin->fid, &tmp);
                  memset (buf, 0, sizeof(buf));
                  vid_vni_show (&tmp, buf);
                  /*赋值*/
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
                  stab.set.type = tmp.type;
                  stab.set.val = tmp.val;
                  add_out_tab (&stab);
                }
            }
        }
    }

  /*遍历2*/
  list_for_each_entry_safe(pin,nin,&mac_head,list)
    {
      list_for_each_entry_safe(pesi,nesi,&esi_head,list)
        {
          if (strcmp (pin->nexthoptype, "ESI") == 0)
            {
              printf ("pesi->name:%s\n", pesi->name);
              printf ("pin->nexthop:%s\n", pin->nexthop);

              if (strcmp (pesi->name, pin->nexthop) == 0)
                {
                  fid2mac_type (pin->fid, &tmp);
                  memset (buf, 0, sizeof(buf));
                  vid_vni_show (&tmp, buf);

                  /*赋值*/
                  strcpy (stab.strfid, buf);
                  strcpy (stab.macaddress, pin->macaddress);
                  strcpy (stab.source, pin->source);
                  int i;
                  for (i = 0; i < static_count; ++i)
                    {
                      list_for_each_entry_safe(pout,nout,&int_head,list)
                        {
                          if (pout->ifx == pesi->nexthopifx[i])
                            {
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
                              add_out_tab (&stab);
                            }
                        }

                    }

                }
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

  list_for_each_entry_safe(p,n,&out_head,list)
    {
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
        }
    }
}

/**
 * main()
 */
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

  show (outfp);

  free_list ();

  fclose (outfp);

  return 0;
}
