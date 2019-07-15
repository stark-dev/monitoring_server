#ifndef RW_UTILS_H
#define RW_UTILS_H

/*******************************************************************************
* included libraries
*******************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/*******************************************************************************
* constants
*******************************************************************************/

// endianness
typedef enum
{
  BE, // big endian
  LE  // little endian
} endianness;

/*******************************************************************************
* read/write functions
*******************************************************************************/
int read_64(void *buffer, uint64_t *data, endianness e);
int read_32(void *buffer, uint32_t *data, endianness e);
int read_16(void *buffer, uint16_t *data, endianness e);
int read_8(void *buffer, uint8_t *data);

int write_64(uint64_t *data, void* buffer, endianness e);
int write_32(uint32_t *data, void* buffer, endianness e);
int write_16(uint16_t *data, void* buffer, endianness e);
int write_8(uint8_t *data, void* buffer);

#endif