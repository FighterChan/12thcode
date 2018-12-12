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
    int nexthop;  /*取值范围[1~65535]*/
};

struct mac_out
{

};

#endif /* MAC_H_ */
