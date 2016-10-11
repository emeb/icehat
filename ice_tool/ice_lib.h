/* ice_lib.h - iCE40 interface lib for raspberry pi                          */
/* Copyright 2016 Eric Brombaugh                                             */
/*   This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
    MA 02110-1301 USA.
*/

#ifndef __ice_lib__
#define __ice_lib__

#include <stdint.h>
#include <linux/types.h>

#define READBUFSIZE 512

/*
 * I/O pins / buses
 */
#define RST_PIN 24
#define SS_PIN 25
#define DONE_PIN 23
#define SPI_BUS 0
#define SPI_ADD 0

/* state structure */
typedef struct
{
	int spi_file;		/* SPI device */
    int cfg;            /* GPIO for config is enabled */
	int verbose;		/* Verbose level */
} iceblk;

int ice_spi_txrx(iceblk *s, uint8_t *tx, uint8_t *rx, __u32 len);
iceblk *ice_init(int cfg, int verbose);
FILE *ice_open_bitfile(iceblk *s, char *bitfile, long *n);
int ice_cfg(iceblk *s, char *bitfile);
void ice_delete(iceblk *s);

#endif
