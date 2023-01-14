// An interface to RPI Linux SPI device intended to be callable from Ada.
// Copied from an example on the RPi forum.
// Author    : David Haley
// Created   : 29/04/2022
// Last Edit : 01/05/2022
// 2022051: corrected CS state change

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include "spi_driver.h"

static int SPI_Handle;
static struct spi_ioc_transfer SPI_Data;

int SPI_Open (unsigned char SPI_Device, unsigned char Mode, unsigned long Speed)
{
   unsigned char SPI_Mode;
   switch (SPI_Device)
   {
      case 0:
         SPI_Handle = open("/dev/spidev0.0", O_RDWR);
      break;
      case 1:
         SPI_Handle = open("/dev/spidev0.1", O_RDWR);
      break;
      default:
         perror("Unknown SPI device");
         return -1;
      break;
   } //switch (SPI_Device)
   if (SPI_Handle < 0)
   {
      perror ("Error opening SPI device");
      return -2;
   } // if (SPI_Handle < 0)
   switch (Mode)
   {
      case 0:
         SPI_Mode = SPI_MODE_0;
      break;
      case 1:
         SPI_Mode = SPI_MODE_1;
      break;
      case 2:
         SPI_Mode = SPI_MODE_2;
      break;
      case 3:
         SPI_Mode = SPI_MODE_3;
      break;
      default:
         perror ("Bad SPI mode requested");
         return -3;
      break;
   } // switch (Mode)
   if (ioctl (SPI_Handle, SPI_IOC_WR_MODE, &SPI_Mode) < 0)
   {
      perror ("Error setting Mode");
      return -4;
   } // if (ioctl (SPI_Handle, SPI_IOC_WR_MODE, &Mode) < 0)
   if (3125 <= Speed && Speed <= 125000000)
   {
      SPI_Data.speed_hz = (__u32)Speed;
      if (ioctl (SPI_Handle, SPI_IOC_WR_MAX_SPEED_HZ, &Speed) < 0)
      {
         perror ("Error setting speed");
         return -5;
      } // (ioctl (SPI_Handle, SPI_IOC_WR_MAX_SPEED_HZ, &Speed) < 0)
   }
   else
   {
      perror ("Bad speed requested");
      return -4;
   } // if (3125 <= Speed && Speec <= 125000000)
   return 0;
} // SPI_Open

int SPI_Transfer (unsigned char *Tx_Buffer, unsigned char *Rx_Buffer,
   unsigned short Length)
{
   const unsigned char Bits = 8; // byte assumed to match Ada Stream_Element
   int Return_Value;
   SPI_Data.tx_buf = (unsigned long)Tx_Buffer;
   SPI_Data.rx_buf = (unsigned long) Rx_Buffer;
   SPI_Data.len = (__u32) Length;
   SPI_Data.delay_usecs = 0; // No additional delay
   SPI_Data.word_delay_usecs = 0;
   SPI_Data.tx_nbits = Bits;
   SPI_Data.rx_nbits = Bits;
   SPI_Data.cs_change = 0; // Change state of CS after each transfer
   Return_Value = ioctl (SPI_Handle, SPI_IOC_MESSAGE(1), &SPI_Data);
   if(Return_Value < 1)
   {
       perror("Error transferring data");
       return Return_Value;
   }
   return 0;
}

int SPI_Close ()
{
  return close (SPI_Handle);
}
