/* ice_lib.c - iCE40 interface lib for raspberry pi                          */
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

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <errno.h>
#include <math.h>
#include "ice_lib.h"

/*
 * GPIO access routines kanged from the internet
 */

#define IN  0
#define OUT 1
 
#define LOW  0
#define HIGH 1

static int GPIOExport(int pin)
{
#define BUFFER_MAX 3
	char buffer[BUFFER_MAX];
	ssize_t bytes_written;
	int fd;
 
	fd = open("/sys/class/gpio/export", O_WRONLY);
	if (-1 == fd) {
		fprintf(stderr, "Failed to open export for writing!\n");
		return(-1);
	}
 
	bytes_written = snprintf(buffer, BUFFER_MAX, "%d", pin);
	write(fd, buffer, bytes_written);
	close(fd);
	return(0);
}
 
static int GPIOUnexport(int pin)
{
	char buffer[BUFFER_MAX];
	ssize_t bytes_written;
	int fd;
 
	fd = open("/sys/class/gpio/unexport", O_WRONLY);
	if (-1 == fd) {
		fprintf(stderr, "Failed to open unexport for writing!\n");
		return(-1);
	}
 
	bytes_written = snprintf(buffer, BUFFER_MAX, "%d", pin);
	write(fd, buffer, bytes_written);
	close(fd);
	return(0);
}
 
static int GPIODirection(int pin, int dir)
{
	static const char s_directions_str[]  = "in\0out";
 
#define DIRECTION_MAX 35
	char path[DIRECTION_MAX];
	int fd;
 
	snprintf(path, DIRECTION_MAX, "/sys/class/gpio/gpio%d/direction", pin);
	fd = open(path, O_WRONLY);
	if (-1 == fd) {
		fprintf(stderr, "Failed to open gpio direction for writing!\n");
		return(-1);
	}
 
	if (-1 == write(fd, &s_directions_str[IN == dir ? 0 : 3], IN == dir ? 2 : 3)) {
		fprintf(stderr, "Failed to set direction!\n");
		return(-1);
	}
 
	close(fd);
	return(0);
}
 
static int GPIORead(int pin)
{
#define VALUE_MAX 30
	char path[VALUE_MAX];
	char value_str[3];
	int fd;
 
	snprintf(path, VALUE_MAX, "/sys/class/gpio/gpio%d/value", pin);
	fd = open(path, O_RDONLY);
	if (-1 == fd) {
		fprintf(stderr, "Failed to open %s for reading!\n", path);
		return(-1);
	}
 
	if (-1 == read(fd, value_str, 3)) {
		fprintf(stderr, "Failed to read %s!\n", path);
		return(-1);
	}
 
	close(fd);
 
	return(atoi(value_str));
}
 
static int GPIOWrite(int pin, int value)
{
	static const char s_values_str[] = "01";
 
	char path[VALUE_MAX];
	int fd;
 
	snprintf(path, VALUE_MAX, "/sys/class/gpio/gpio%d/value", pin);
	fd = open(path, O_WRONLY);
	if (-1 == fd) {
		fprintf(stderr, "Failed to open %s for writing!\n", path);
		return(-1);
	}
 
	if (1 != write(fd, &s_values_str[LOW == value ? 0 : 1], 1)) {
		fprintf(stderr, "Failed to write %s\n", path);
		return(-1);
	}
 
	close(fd);
	return(0);
}

/*
 * My stuff based on BCC history
 */

/* wrapper for fprintf(stderr, ...) to support verbose control */
void qprintf(iceblk *s, char *fmt, ...)
{
	va_list args;
	
	if(s->verbose)
	{
		va_start(args, fmt);
		vfprintf(stderr, fmt, args);
		va_end(args);
	}
}

/* SPI Transmit/Receive */
int ice_spi_txrx(iceblk *s, uint8_t *tx, uint8_t *rx, __u32 len)
{
	struct spi_ioc_transfer tr = {
		.tx_buf = (unsigned long)tx,
		.rx_buf = (unsigned long)rx,
		.len = len,
		.delay_usecs = 0,
		.speed_hz = 2000000,
		.bits_per_word = 8,
	};
	
	return ioctl(s->spi_file, SPI_IOC_MESSAGE(1), &tr);
}

/* initialize our FPGA interface */
iceblk *ice_init(int cfg, int verbose)
{
	iceblk *s;
	char filename[20];
	uint32_t speed = 10000000;
	uint8_t mode = 0;
	//uint8_t mode = SPI_CPHA;
	//uint8_t mode = SPI_CPOL;
	
	/* allocate the object */
	if((s = calloc(1, sizeof(iceblk))) == NULL)
	{
		qprintf(s, "ice_init: Couldn't allocate bfpga object\n");
		goto fail0;
	}
	
	/* set verbose level */
	s->verbose = verbose;
    
    /* do we need to configure? */
    s->cfg = cfg;
    if(cfg)
    {
	qprintf(s, "ice_init: opening GPIO\n");

        /* Open the GPIO pins */
        if(-1 == GPIOExport(RST_PIN) ||
            -1 == GPIOExport(SS_PIN) ||
            -1 == GPIOExport(DONE_PIN))
            goto fail1;
     
        /*
         * Set GPIO directions
         */
        if(-1 == GPIODirection(RST_PIN, OUT) ||
            -1 == GPIODirection(SS_PIN, OUT) ||
            -1 == GPIODirection(DONE_PIN, IN))
            goto fail2;
    
    		/* default outputs */
		GPIOWrite(RST_PIN, 1);
		GPIOWrite(SS_PIN, 1);
		
		/* Check if FPGA already configured */
		if(GPIORead(DONE_PIN)==0)
		{
			qprintf(s, "ice_init: FPGA not already configured - DONE not high\n\r");
		}	
		else
			qprintf(s, "ice_init: FPGA already configured - Done is high\n");
	}
    
	/* Open the SPI port */
	sprintf(filename, "/dev/spidev%d.%d", SPI_BUS, SPI_ADD);
	s->spi_file = open(filename, O_RDWR);
	
	if(s->spi_file < 0)
	{
		qprintf(s, "ice_init: Couldn't open spi device %s\n", filename);
		goto fail2;
	}
	else
		qprintf(s, "ice_init: opened spi device %s\n", filename);

	if(ioctl(s->spi_file, SPI_IOC_WR_MAX_SPEED_HZ, &speed) == -1)
	{
		qprintf(s, "ice_init: Couldn't set SPI clock to %d Hz\n", speed);
		goto fail2;
	}
	else
		qprintf(s, "ice_init: Set SPI clock to %d Hz\n", speed);
	
	if(ioctl(s->spi_file, SPI_IOC_WR_MODE, &mode) == -1)
	{
		qprintf(s, "ice_init: Couldn't set SPI mode\n");
		goto fail2;
	}
	else
		qprintf(s, "ice_init: Set SPI mode\n");
	
	/* success */
	return s;

	/* failure modes */
//fail3:
//	close(s->spi_file);		/* close the SPI device */
fail2:
	if(s->cfg)
    {
        GPIOUnexport(DONE_PIN); /* release the GPIO pins */
        GPIOUnexport(SS_PIN);
        GPIOUnexport(RST_PIN);
    }
fail1:
	free(s);				/* free the structure */
fail0:
	return NULL;
}

/* Send a bitstream to the FPGA */
int ice_cfg(iceblk *s, char *bitfile)
{
	FILE *fd;
    int read;
	long ct;
	unsigned char byte, rxbyte, dummybuf[READBUFSIZE];
	char readbuf[READBUFSIZE];

	/* open file or return error*/
	if(!(fd = fopen(bitfile, "r")))
	{
		qprintf(s, "ice_cfg: open file %s failed\n\r", bitfile);
		return 0;
	}
	else
	{
		qprintf(s, "ice_cfg: opened bitstream file %s\n\r", bitfile);
	}

    /* set SS low for spi config */
	GPIOWrite(SS_PIN, 0);
    
	/* pulse RST low min 500 ns */
	GPIOWrite(RST_PIN, 0);
	usleep(1);			// wait a bit
	
	/* Wait for DONE low */
	qprintf(s, "ice_cfg: RST low, Waiting for DONE low\n\r");
	while(GPIORead(DONE_PIN)==1)
	{
		asm volatile ("nop");	//"nop" means no-operation.  We don't want to do anything during the delay
	}
	
	/* Release RST */
	GPIOWrite(RST_PIN, 1);

	/* wait 5us */
	usleep(5);
	qprintf(s, "ice_cfg: Sending bitstream\n\r");
	
	/* Read file & send bitstream to FPGA via SPI */
	ct = 0;
	while( (read=fread(readbuf, sizeof(char), READBUFSIZE, fd)) > 0 )
	{
		/* Send bitstream */
		ice_spi_txrx(s, (unsigned char *)readbuf, dummybuf, read);
		ct += read;
		
		/* diagnostic to track buffers */
		qprintf(s, ".");
		if(s->verbose)
			fflush(stdout);
	}
	
	/* close file */
	qprintf(s, "\n\rice_cfg: sent %ld bytes\n\r", ct);
	qprintf(s, "ice_cfg: bitstream sent, closing file\n\r");
	fclose(fd);
	
	/* send dummy data while waiting for DONE */
 	qprintf(s, "ice_cfg: sending dummy clocks, waiting for DONE or fail\n\r");
	byte = 0xFF;
	ct = 0;
	while((GPIORead(DONE_PIN)==0) && (ct < 10000))
	{
		/* Dummy - all ones */
		ice_spi_txrx(s, &byte, &rxbyte, 1);
		ct++;
	}
	
	if(ct < 10000)
	{
	 	qprintf(s, "ice_cfg: %d dummy clocks sent\n\r", ct*8);
    	}
	else
	{
		qprintf(s, "ice_cfg: timeout waiting for DONE\n\r");
	}

	/* send 7 bytes to finish */
	ice_spi_txrx(s, &byte, &rxbyte, 1);
	ice_spi_txrx(s, &byte, &rxbyte, 1);
	ice_spi_txrx(s, &byte, &rxbyte, 1);
	ice_spi_txrx(s, &byte, &rxbyte, 1);
	ice_spi_txrx(s, &byte, &rxbyte, 1);
	ice_spi_txrx(s, &byte, &rxbyte, 1);
	ice_spi_txrx(s, &byte, &rxbyte, 1);
    
	/* return status */
	if(GPIORead(DONE_PIN)==0)
	{
		qprintf(s, "ice_cfg: cfg failed - DONE not high\n\r");
		return 1;	// Done = 0 - error
	}
	else	
	{
		qprintf(s, "ice_cfg: success\n\r");
		return 0;	// Done = 1 - OK
	}
}

/* Clean shutdown of our FPGA interface */
void ice_delete(iceblk *s)
{
	/* Release SPI bus */
	close(s->spi_file);		/* close the SPI device */
	if(s->cfg)
    {
        GPIOUnexport(DONE_PIN); /* release the GPIO pins */
        GPIOUnexport(SS_PIN);
        GPIOUnexport(RST_PIN);
	}
    free(s);				/* free the structure */
}
