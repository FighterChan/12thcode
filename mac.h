/*
 * mac.h
 *
 *  Created on: 2018年12月12日
 *      Author: CW
 */

#ifndef MAC_H_
#define MAC_H_
#include "list.h"

#define TYPE_LEN 16
#define ROTO_LEN 16
#define SOURCE_LEN 16
#define MACADDRESS_LEN 32
#define NEXTHOPTYPE_LEN 32
#define INTTYPE_LEN 16
#define IFNAME_LEN 32
#define PEERIP_LEN 32
#define NAME_LEN 16
#define NEXTHOPIFX_LEN 256

#define VID_MIN 1
#define VID_MAX 4094
#define VNI_MIN 1
#define VNI_MAX 16777215

char *strproto[8] =
    { "VXLAN", "DOT1Q" };

char *strsource[8] =
    { "STATIC", "LOCAL", "MLAG", "EVPN" };

char *strnexthoptype[16] =
    { "INTERFACE", "ESI" };

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
    char nexthopifx[NEXTHOPIFX_LEN];
    struct list_head list;
};

#endif /* MAC_H_ */
