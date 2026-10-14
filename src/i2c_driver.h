// An interface to RPI Linux i2c device intended to be callable from Ada.
// Based on generic example code
// raspberry-projects.com/pi/programming-in-c/i2c/using-the-i2c-interface.
//
// Author    : David Haley
// Created   : 03/03/2023
// Last Edit : 15/10/2026

// 20261015 : Errors returned as -errno, calls made when the device is not
// open are rejected.

#ifndef I2C_DRIVER_H
#define I2C_DRIVER_H

int I2C_Open (unsigned char I2C_Device);
// Opens /dev/i2c-I2C_Device.
// Returns 0 if successful, -EBUSY if a device is already open, otherwise
// -errno from open.

int Set_IC_Address (int IC_Address);
// Sets the IC_Address to be used for subsequent read and write operations.
// only 7 bit addresses are supported address should lie in range 0 .. 7F.
// Returns 0 if successful, otherwise -errno from ioctl, -EBADF if the device
// is not open.

int I2C_Write (unsigned char *Tx_Buffer, unsigned short Length);
// Tx_Buffer must be at least Length bytes long.
// Transfers Length bytes from the Tx_Buffer to the specified IC_Address.
// Return positive values the number of bytes written, negative values are
// -errno from write, -EBADF if the device is not open.

int I2C_Read (unsigned char *Rx_Buffer, unsigned short Length);
// Rx_Buffer must be at least Length bytes long.
// Transfers Length bytes into the RxBuffer.
// Return positive values the number of bytes read, negative values are
// -errno from read, -EBADF if the device is not open.

int I2C_Close (void);
// Close the I2C device should be called before program termination.
// Return values:
// 0: Success;
// Negative values are -errno from close, -EBADF if the device is not open.

#endif // I2C_DRIVER_H
