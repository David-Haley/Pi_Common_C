// An interface to RPI Linux i2c device intended to be callable from Ada.
// Based on generic example code
// raspberry-projects.com/pi/programming-in-c/i2c/using-the-i2c-interface.
//
// Author    : David Haley
// Created   : 03/03/2023
// Last Edit : 03/03/2023

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>	

static int I2C_Handle;

int I2C_Open (unsigned char I2C_Device)
// I2C_Device 0 .. 1
// Returns 0 if successful, otherwise the error code from open

{ // I2C_Open
   switch (I2C_Device)
   {
      case 0:
         I2C_Handle = open("/dev/i2c-0", O_RDWR);
      break;
      case 1:
         I2C_Handle = open("/dev/i2c-1", O_RDWR);
      break;
      default:
         perror("Unknown I2C device");
         return -255;
      break;
   } //switch (SPI_Device)
   if (I2C_Handle < 0)
   {
      perror ("Error opening I2C device");
      return I2C_Handle;
   } // if (I2C_Handle < 0)
   else
      return 0;
} // I2C_Open

int Set_IC_Address (int IC_Address)
// Sets the IC_Address to be used for subsequent read and write operations.
// only 7 bit addresses are supported address should lie in range 0 .. 7F.
// Returns 0 if successful, otherwise the error code from ioctl

{ // Set_IC_Address
   int Result;
   Result = ioctl (I2C_Handle, I2C_SLAVE, IC_Address);
   if (Result < 0)
   {
      perror ("Error setting Slave Address");
   } //if (Result < 0)
      return Result;
} // Set_IC_Address

int I2C_Write (unsigned char *Tx_Buffer, unsigned short Length)
// Tx_Buffer must be at least Length bytes long.
// Transfers Length bytes from the Tx_Buffer to the specified IC_Address.
// Return positive values the number of bytes written, negative values are the
// error code returned from the call to write.

{ // I2C_Write
   int Bytes_Written;
   Bytes_Written = write (I2C_Handle, Tx_Buffer, (int) Length);
   if (Length != Bytes_Written)
   {
      perror ("Error writing to I2C device");
   } // if (Length != Bytes_Written)
   return Bytes_Written;
} // I2C_Write

int I2C_Read (unsigned char *Rx_Buffer, unsigned short Length)
// Rx_Buffer must be at least Length bytes long.
// Transfers Length bytes into the RxBuffer.
// Return positive values the number of btres read, negative values are the
// error code returned from the call to read.

{ // I2C_Read
   int Bytes_Read;
   Bytes_Read = read (I2C_Handle, Rx_Buffer, (int) Length);
   if (Length != Bytes_Read)
   {
      perror ("Error reading from I2C device");
   } // if (Length != Bytes_Read)
   return Bytes_Read;
} // I2C_Read

int I2C_Close ()
// Close  the SPI device should be called before program termination.
// Return values:
// 0: Success;
// Non zero values are the error code returned from the call to close

{ // I2C_Close
   int Result;
   Result = close (I2C_Handle);
   if (Result != 0)
   {
      perror ("Error closing I2C device");
   }
   return Result;
} // I2C_Close
