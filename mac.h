/*
 * mac.h
 *
 *  Created on: 2018年12月12日
 *      Author: CW
 */

#ifndef MAC_H_
#define MAC_H_
#include "list.h"

struct mac
{
    char type[16];
    char proto[16];
    char source[16];
    char fid[16];
    char macaddress[32];
    char nexthoptype[32];
    char nexthop[16]; /*取值范围[1~65535]*/
    struct list_head list;
};

struct mac_in
{
    char type[16];
    char proto[16];
    char source[16];
    int fid;
    char macaddress[32];
    char nexthoptype[32];
    int nexthop; /*取值范围[1~65535]*/
    struct list_head list;
};

struct int_out
{
    char type[8];
    char inttype[8];
    int ifx;
    char ifname[16];
    char peerip[16];
};

struct esi
{
    char type[8];
    char name[8];
    int nexthopcount;
    char nexthopifx[1024];
};

#endif /* MAC_H_ */
