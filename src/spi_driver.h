// An interface to RPI Linux SPI device intended to be callable from Ada.
// Copied from an example on the RPi forum.
// Author    : David Haley
// Created   : 29/04/2022
// Last Edit : 15/10/2026
// 20261015: Errors returned as -errno, calls made in the wrong state rejected.
// 20251011: Check on word length added
// 20220490: Comments corrected

#ifndef SPI_DRIVER_H
#define SPI_DRIVER_H

int SPI_Open (unsigned char SPI_Device, unsigned char Mode,
   unsigned long Speed);
// SPI_Device 0 .. 1
// Mode 0 .. 3
// Speed 3125 .. 125000000
// Must be called before any transfers, returns 0 if successful.
// Return values:
//  0: Success
// -EINVAL: SPI_Device, Mode or Speed out of range
// -EBUSY: An SPI device is already open
// -EIO: Mode or word length did not read back as set
// Other negative values are -errno from open or ioctl. The device is closed
// if any part of the configuration fails.

int SPI_Transfer (unsigned char *Tx_Buffer, unsigned char *Rx_Buffer,
   unsigned short Length);
// Tx_Buffer and Rx_Buffer should be the same length and must be at least Length
// bytes long;
// Transfers Length bytes from the Tx_Buffer to the SPI interface and reads back
// Length bytes into the RxBuffer.
// Return values.
// 0: Success
// -EBADF: The SPI device is not open
// Other negative values are -errno from ioctl with second parameter
// SPI_IOC_MESSAGE(1)

int SPI_Close (void);
// Close the SPI device should be called before program termination.
// Return values:
// 0: Success;
// -EBADF: The SPI device is not open
// Other negative values are -errno from close.

#endif // SPI_DRIVER_H
