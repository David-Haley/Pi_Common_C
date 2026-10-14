// An interface to RPI Linux SPI device intended to be callable from Ada.
// Copied from an example on the RPi forum.
// Author    : David Haley
// Created   : 29/04/2022
// Last Edit : 15/10/2026
// 20261015: Handle initialised to -1 and reset on close, so that a close
// without an open cannot close an unrelated file descriptor. Arguments
// validated before the device is opened, device closed if configuration
// fails. Errors returned as -errno rather than printed.
// 20251011: Corrected some potential errors in managing the SPI transfer
// 20220501: corrected CS state change

#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include "spi_driver.h"

static int SPI_Handle = -1;
static __u32 SPI_Speed;
static const unsigned char Bits = 8; // byte assumed to match Ada Stream_Element

static int Close_On_Error (int Result)
// Closes the partly configured device and returns Result.

{
   close (SPI_Handle);
   SPI_Handle = -1;
   return Result;
} // Close_On_Error

int SPI_Open (unsigned char SPI_Device, unsigned char Mode, unsigned long Speed)
{
   static const __u32 SPI_Modes [] =
      {SPI_MODE_0, SPI_MODE_1, SPI_MODE_2, SPI_MODE_3};
   char Device_Name [24];
   __u32 SPI_Mode, Mode_Check;
   unsigned char Bits_Check;

   if (SPI_Device > 1 || Mode > 3 || Speed < 3125 || Speed > 125000000)
      return -EINVAL;
   if (SPI_Handle >= 0)
      return -EBUSY;
   snprintf (Device_Name, sizeof (Device_Name), "/dev/spidev0.%u",
             (unsigned int) SPI_Device);
   SPI_Handle = open (Device_Name, O_RDWR | O_CLOEXEC);
   if (SPI_Handle < 0)
   {
      SPI_Handle = -1;
      return -errno;
   } // if (SPI_Handle < 0)
   SPI_Mode = SPI_Modes [Mode];
   Mode_Check = SPI_Mode;
   if (ioctl (SPI_Handle, SPI_IOC_WR_MODE32, &SPI_Mode) < 0 ||
       ioctl (SPI_Handle, SPI_IOC_RD_MODE32, &SPI_Mode) < 0)
      return Close_On_Error (-errno);
   if (SPI_Mode != Mode_Check)
      return Close_On_Error (-EIO);
   if (ioctl (SPI_Handle, SPI_IOC_WR_BITS_PER_WORD, &Bits) < 0 ||
       ioctl (SPI_Handle, SPI_IOC_RD_BITS_PER_WORD, &Bits_Check) < 0)
      return Close_On_Error (-errno);
   if (Bits != Bits_Check)
      return Close_On_Error (-EIO);
   SPI_Speed = (__u32) Speed;
   if (ioctl (SPI_Handle, SPI_IOC_WR_MAX_SPEED_HZ, &SPI_Speed) < 0)
      return Close_On_Error (-errno);
   return 0;
} // SPI_Open

int SPI_Transfer (unsigned char* Tx_Buffer, unsigned char* Rx_Buffer,
   unsigned short Length)
{
   struct spi_ioc_transfer SPI_Data = {0};
   // Fields not set below are zero, cs_change 0 leaves CS asserted during the
   // transfer, tx_nbits and rx_nbits 0 select a single wire.

   if (SPI_Handle < 0)
      return -EBADF;
   SPI_Data.tx_buf = (__u64) (uintptr_t) Tx_Buffer;
   SPI_Data.rx_buf = (__u64) (uintptr_t) Rx_Buffer;
   SPI_Data.len = (__u32) Length;
   SPI_Data.speed_hz = SPI_Speed;
   SPI_Data.bits_per_word = Bits;
   if (ioctl (SPI_Handle, SPI_IOC_MESSAGE(1), &SPI_Data) < 0)
      return -errno;
   return 0;
} // SPI_Transfer

int SPI_Close (void)
{
   int Result;

   if (SPI_Handle < 0)
      return -EBADF;
   Result = close (SPI_Handle);
   SPI_Handle = -1;
   // The descriptor is released even if close reports an error.
   if (Result != 0)
      return -errno;
   return 0;
} // SPI_Close
