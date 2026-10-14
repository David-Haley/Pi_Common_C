// An interface to RPI Linux i2c device intended to be callable from Ada.
// Based on generic example code
// raspberry-projects.com/pi/programming-in-c/i2c/using-the-i2c-interface.
//
// Author    : David Haley
// Created   : 03/03/2023
// Last Edit : 15/10/2026

// 20261015 : Handle initialised to -1 and reset on close, so that a close
// without an open cannot close an unrelated file descriptor. Errors returned
// as -errno rather than printed. Any /dev/i2c-N device can be opened.

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include "i2c_driver.h"

static int I2C_Handle = -1;

int I2C_Open (unsigned char I2C_Device)
// Opens /dev/i2c-I2C_Device.
// Returns 0 if successful, -EBUSY if a device is already open, otherwise
// -errno from open.

{ // I2C_Open
   char Device_Name [16];

   if (I2C_Handle >= 0)
      return -EBUSY;
   snprintf (Device_Name, sizeof (Device_Name), "/dev/i2c-%u",
             (unsigned int) I2C_Device);
   I2C_Handle = open (Device_Name, O_RDWR | O_CLOEXEC);
   if (I2C_Handle < 0)
   {
      I2C_Handle = -1;
      return -errno;
   } // if (I2C_Handle < 0)
   return 0;
} // I2C_Open

int Set_IC_Address (int IC_Address)
// Sets the IC_Address to be used for subsequent read and write operations.
// only 7 bit addresses are supported address should lie in range 0 .. 7F.
// Returns 0 if successful, otherwise -errno from ioctl, -EBADF if the device
// is not open.

{ // Set_IC_Address
   if (I2C_Handle < 0)
      return -EBADF;
   if (ioctl (I2C_Handle, I2C_SLAVE, IC_Address) < 0)
      return -errno;
   return 0;
} // Set_IC_Address

int I2C_Write (unsigned char *Tx_Buffer, unsigned short Length)
// Tx_Buffer must be at least Length bytes long.
// Transfers Length bytes from the Tx_Buffer to the specified IC_Address.
// Return positive values the number of bytes written, negative values are
// -errno from write, -EBADF if the device is not open.

{ // I2C_Write
   ssize_t Bytes_Written;

   if (I2C_Handle < 0)
      return -EBADF;
   Bytes_Written = write (I2C_Handle, Tx_Buffer, Length);
   if (Bytes_Written < 0)
      return -errno;
   return (int) Bytes_Written;
} // I2C_Write

int I2C_Read (unsigned char *Rx_Buffer, unsigned short Length)
// Rx_Buffer must be at least Length bytes long.
// Transfers Length bytes into the RxBuffer.
// Return positive values the number of bytes read, negative values are
// -errno from read, -EBADF if the device is not open.

{ // I2C_Read
   ssize_t Bytes_Read;

   if (I2C_Handle < 0)
      return -EBADF;
   Bytes_Read = read (I2C_Handle, Rx_Buffer, Length);
   if (Bytes_Read < 0)
      return -errno;
   return (int) Bytes_Read;
} // I2C_Read

int I2C_Close (void)
// Close the I2C device should be called before program termination.
// Return values:
// 0: Success;
// Negative values are -errno from close, -EBADF if the device is not open.

{ // I2C_Close
   int Result;

   if (I2C_Handle < 0)
      return -EBADF;
   Result = close (I2C_Handle);
   I2C_Handle = -1;
   // The descriptor is released even if close reports an error.
   if (Result != 0)
      return -errno;
   return 0;
} // I2C_Close
