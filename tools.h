/* /12thcode/tools.h */
#ifndef _TOOLS_H_
#define _TOOLS_H_

struct mac_type
{
    unsigned char type;
    int val;
};

int
mac_type2fid (struct mac_type *p);
int
fid2mac_type (int fid, struct mac_type *p);
int
check_ifx_nexthop (int ifx);
int
conver_filename (char *infile, char *outfile);
int
do_esi (char *in, int data[]);

/* body of tools.h */
#endif /* _TOOLS_H_ */
