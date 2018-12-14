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

#define MAX(X,Y) ((X)>(Y)?(X):(Y))

#define TYPE_LEN 16
#define ROTO_LEN 16
#define SOURCE_LEN 16
#define MACADDRESS_LEN 32
#define NEXTHOPTYPE_LEN 32
#define INTTYPE_LEN 16
#define IFNAME_LEN 32
#define PEERIP_LEN 32
#define NAME_LEN 16
#define NEXTHOPIFX_LEN 1024

#define VID_MIN 1
#define VID_MAX 4094
#define VNI_MIN 1
#define VNI_MAX 16777215

enum {
    _ADD,
    _DEL
};

struct mac_in
{
    char type[TYPE_LEN];
    char proto[ROTO_LEN];
    char source[SOURCE_LEN];
    int fid;
    char macaddress[MACADDRESS_LEN];
    char nexthoptype[NEXTHOPTYPE_LEN];
    int nexthop; /*取值范围[1~65535]*/
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

struct list_head mac_head;
struct list_head int_head;
struct list_head esi_head;

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
copy_to_esi (struct esi *p, struct esi *s);
int
add_esi (struct esi *s);
int
del_esi (struct esi *s);

#endif /* MAC_H_ */
