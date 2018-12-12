/*
 * mac.h
 *
 *  Created on: 2018年12月12日
 *      Author: CW
 */

#ifndef MAC_H_
#define MAC_H_

struct mac_in
{
    char type[8];
    char proto[8];
    char source[8];
    int fid;
    char macaddress[16];
    char nexthoptype[16];
    int nexthop; /*取值范围[1~65535]*/
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
