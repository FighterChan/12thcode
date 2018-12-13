/*
 * main.h
 *
 *  Created on: 2018年12月13日
 *      Author: CW
 */

#ifndef MAIN_H_
#define MAIN_H_

/* 命令类型 */
typedef enum
{
    TYPE_UNKNOWN = 0,
    ADD_MAC,
    ADD_INT,
    ADD_ESI,
    DEL_MAC,
    DEL_INT,
    DEL_ESI
} cli_cmd_e;

static cli_cmd_t cli_cmd_tables[] =
    {
        { "ADD-MAC", ADD_MAC },
        { "ADD-INT", ADD_INT },
        { "ADD-ESI", ADD_ESI },
        { "DEL-MAC", DEL_MAC },
        { "DEL-INT", DEL_INT },
        { "DEL-ESI", DEL_ESI },
    };

#endif /* MAIN_H_ */
