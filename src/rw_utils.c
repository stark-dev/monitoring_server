#include "../include/rw_utils.h"

/*******************************************************************************
* read/write functions
*******************************************************************************/

int read_64(void *buffer, uint64_t *data, endianness e)
{
  unsigned int i = 0;

  uint8_t *src, *dest;

  src = (uint8_t *) buffer;
  dest = (uint8_t *) data;

  for(i=0; i< sizeof(uint64_t); i++)
  {
    if(e == BE)
    {
      memset((void *)(dest + i), *(src + sizeof(uint64_t) - 1 - i), 1);
    }
    else
    {
      memset((void *)(dest + i), *(src + i), 1);
    }
  }

  return sizeof(uint64_t);
}

int read_32(void *buffer, uint32_t *data, endianness e)
{
  unsigned int i = 0;

  uint8_t *src, *dest;

  src = (uint8_t *) buffer;
  dest = (uint8_t *) data;

  for(i=0; i< sizeof(uint32_t); i++)
  {
    if(e == BE)
    {
      memset((void *)(dest + i), *(src + sizeof(uint32_t) - 1 - i), 1);
    }
    else
    {
      memset((void *)(dest + i), *(src + i), 1);
    }
  }

  return sizeof(uint32_t);
}

int read_16(void *buffer, uint16_t *data, endianness e)
{
  unsigned int i = 0;

  uint8_t *src, *dest;

  src = (uint8_t *) buffer;
  dest = (uint8_t *) data;

  for(i=0; i< sizeof(uint16_t); i++)
  {
    if(e == BE)
    {
      memset((void *)(dest + i), *(src + sizeof(uint16_t) - 1 - i), 1);
    }
    else
    {
      memset((void *)(dest + i), *(src + i), 1);
    }
  }

  return sizeof(uint16_t);
}

int read_8(void *buffer, uint8_t *data)
{
  uint8_t *src = (uint8_t *) buffer;

  memset((void *)data, *src, 1);

  return sizeof(uint8_t);
}

int write_32(uint32_t *data, void* buffer, endianness e)
{
  unsigned int i = 0;

  uint8_t *src, *dest;

  src = (uint8_t *) data;
  dest = (uint8_t *) buffer;

  for(i=0; i < sizeof(uint32_t); i++)
  {
    if(e == BE)
    {
      memset((void *)(dest + i), *(src + sizeof(uint32_t) - 1 - i), 1);
    }
    else
    {
      memset((void *)(dest + i), *(src + i), 1);
    }
  }

  return sizeof(uint32_t);
}

int write_64(uint64_t *data, void* buffer, endianness e)
{
  unsigned int i = 0;

  uint8_t *src, *dest;

  src = (uint8_t *) data;
  dest = (uint8_t *) buffer;

  for(i=0; i < sizeof(uint64_t); i++)
  {
    if(e == BE)
    {
      memset((void *)(dest + i), *(src + sizeof(uint64_t) - 1 - i), 1);
    }
    else
    {
      memset((void *)(dest + i), *(src + i), 1);
    }
  }

  return sizeof(uint64_t);
}


int write_16(uint16_t *data, void* buffer, endianness e)
{
  unsigned int i = 0;

  uint8_t *src, *dest;

  src = (uint8_t *) data;
  dest = (uint8_t *) buffer;

  for(i=0; i < sizeof(uint16_t); i++)
  {
    if(e == BE)
    {
      memset((void *)(dest + i), *(src + sizeof(uint16_t) - 1 - i), 1);
    }
    else
    {
      memset((void *)(dest + i), *(src + i), 1);
    }
  }

  return sizeof(uint16_t);
}

int write_8(uint8_t *data, void* buffer)
{
  uint8_t *dest;

  dest = (uint8_t *) buffer;
  memset((void *)dest, *data, 1);

  return sizeof(uint8_t);
}