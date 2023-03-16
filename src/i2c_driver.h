// An interface to RPI Linux i2c device intended to be callable from Ada.
// Based on generic example code
// raspberry-projects.com/pi/programming-in-c/i2c/using-the-i2c-interface.
//
// Author    : David Haley
// Created   : 03/03/2023
// Last Edit : 03/03/2023

int I2C_Open (unsigned char I2C_Device);
// I2C_Device 0 .. 1
// Returns 0 if successful, otherwise the error code from open

int Set_IC_Address (int IC_Address);
// Sets the IC_Address to be used for subsequent read and write operations.
// only 7 bit addresses are supported address should lie in range 0 .. 7F.
// Returns 0 if successful, otherwise the error code from ioctl

int I2C_Write (unsigned char *Tx_Buffer, unsigned short Length);
// Tx_Buffer must be at least Length bytes long.
// Transfers Length bytes from the Tx_Buffer to the specified IC_Address.
// Return positive values the number of bytes written, negative values are the
// error code returned from the call to write.

int I2C_Read (unsigned char *Rx_Buffer, unsigned short Length);
// Rx_Buffer must be at least Length bytes long.
// Transfers Length bytes into the RxBuffer.
// Return positive values the number of btres read, negative values are the
// error code returned from the call to read.

int I2C_Close ();
// Close  the SPI device should be called before program termination.
// Return values:
// 0: Success;
// Non zero values are the error code returned from the call to close
