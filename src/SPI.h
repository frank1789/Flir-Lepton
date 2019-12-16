/*
 * SPI testing utility (using spidev driver)
 *
 * Copyright (c) 2007  MontaVista Software, Inc.
 * Copyright (c) 2007  Anton Vorontsov <avorontsov@ru.mvista.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License.
 *
 * Cross-compile with cross-gcc -I/path/to/cross-kernel/include
 */

#ifndef SPI_H
#define SPI_H

extern int spi_cs0_fd;
extern int spi_cs1_fd;
extern unsigned char spi_mode;
extern unsigned char spi_bits_per_word;
extern unsigned int spi_speed;

int leptonSPI_OpenPort(int spi_device);
int leptonSPI_ClosePort(int spi_device);

#endif
