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
