/*
 * mac.h
 *
 *  Created on: 2018年12月12日
 *      Author: CW
 */

#ifndef MAC_H_
#define MAC_H_
#include "list.h"
#include "jhash.h"
#include "tools.h"

#define MAX(X,Y) ((X)>(Y)?(X):(Y))
#define MIN(X,Y) ((X)<(Y)?(X):(Y))

#define TYPE_LEN 16
#define ROTO_LEN 16
#define SOURCE_LEN 16
#define MACADDRESS_LEN 32
#define NEXTHOPTYPE_LEN 32
#define NEXTHOP_LEN 64
#define INTTYPE_LEN 16
#define IFNAME_LEN 32
#define PEERIP_LEN 32
#define NAME_LEN 32
#define NEXTHOPIFX_LEN 1024
#define STRFID_LEN  16

#define VID_MIN 1
#define VID_MAX 4094
#define VNI_MIN 1
#define VNI_MAX 16777215

enum
{
  _ADD, _DEL
};

struct mac_in
{
  char type[TYPE_LEN];
  char proto[ROTO_LEN];
  char source[SOURCE_LEN];
  int fid;
  char macaddress[MACADDRESS_LEN];
  char nexthoptype[NEXTHOPTYPE_LEN];
  char nexthop[NEXTHOP_LEN];
  struct list_head list;
};

struct int_out
{
  char type[TYPE_LEN];
  char inttype[INTTYPE_LEN];
  int ifx;
  char ifname[IFNAME_LEN];
  char peerip[PEERIP_LEN];
  struct list_head list;
};

struct esi
{
  char type[TYPE_LEN];
  char name[NAME_LEN];
  int nexthopcount;
  int nexthopifx[NEXTHOPIFX_LEN];
  struct list_head list;
};

struct out_tab
{
  char strfid[STRFID_LEN];
  char macaddress[MACADDRESS_LEN];
  char source[SOURCE_LEN];
  char nexthop[NEXTHOP_LEN];
  int flg;
  struct mac_type set;
  struct list_head list;
};

/*全局变量*/
struct list_head mac_head;
struct list_head int_head;
struct list_head esi_head;
struct list_head out_head;

int
copy_to_mac_in (struct mac_in *p, struct mac_in *s);
int
add_mac_in (struct mac_in *s);
int
del_mac_in (struct mac_in *s);
int
check_mac_in (struct mac_in *p, int add_del);
u32
get_mac_in_key (int fid, const char *mac_addr);
int
copy_to_int_out (struct int_out *p, struct int_out *s);
struct int_out *
look_up_int_out (struct int_out *s);
int
add_int_out (struct int_out *s);
int
del_int_out (struct int_out *s);
u32
get_int_out_key (int ifx);
int
copy_to_esi (struct esi *p, struct esi *s, int count);
int
add_esi (struct esi *s, int count);
int
del_esi (struct esi *s);
int
copy_to_out_tab (struct out_tab *p, struct out_tab *s);
int
add_out_tab (struct out_tab *s);
int
free_mac_in (void);
int
free_esi (void);
int
free_int_out (void);
int
free_out_tab (void);
void
free_list (void);

int
sort_out_tab (struct out_tab *new);

#endif /* MAC_H_ */
