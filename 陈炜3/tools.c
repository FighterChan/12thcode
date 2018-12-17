/* 
 * tools.c
 * Original Author: chenwei3@ruijie.com.cn, 2018-12-13
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
#include "tools.h"
#include "mac.h"

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
check_ifx_nexthop (int ifx)
{
  if (ifx < 1 || ifx > 65535)
    {
      return -1;
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
      sprintf (outpath, "%s%s", token, "_result.txt");
    }
  else
    {
      return -1;
    }
  return 0;
}

int
do_esi (char *in, int data[])
{
  int i = 0;
  char *p = in + 2;
  char *token;
  while ((token = strsep (&p, ",")) != NULL)
    {
      data[i] = atoi (token);
      i++;
    }
  return i;
}
