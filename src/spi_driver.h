// An interface to RPI Linux SPI device intended to be callable from Ada.
// Copied from an example on the RPi forum.
// Author    : David Haley
// Created   : 29/04/2022
// Last Edit : 30/04/2022
// 20220490: Comments corrected

int SPI_Open (unsigned char SPI_Device, unsigned char Mode,
   unsigned long Speed);
// SPI_Device 0 .. 1
// Mode 0 .. 3
// Speed 3125 .. 125000000
// Must be called before any transfers, returns 0 if successful.
// Return values:
//  0: Success
// -1: Unknown SPI device
// -2: Error opening SPI device
// -3: Unknown SPI mode requested
// -4: Error setting Mode
// -5: Bad speed requested
//

int SPI_Transfer (unsigned char *Tx_Buffer, unsigned char *Rx_Buffer,
   unsigned short Length);
// Tx_Buffer and Rx_Buffer should be the same length and must be at least Length
// bytes long;
// Transfers Length bytes from the Tx_Buffer to the SPI interface and reads back
// Length bytes into the RxBuffer.
// Return values.
// 0: Success
// Negative values are the error code returned from call to ioctl with second
// parameter SPI_IOC_MESSAGE(1)
//

int SPI_Close ();
// Close  the SPI device should be called before program termination.
// Return values:
// 0: Success;
// Non zero values are the error code returned from call to close
