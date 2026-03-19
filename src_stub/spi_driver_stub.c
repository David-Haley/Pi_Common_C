// No-op SPI stub for simulation builds.
// Implements the same symbols as spi_driver.c but without
// <linux/spi/spidev.h> or any hardware device access.
// SPI_Open returns 0 (success) so Ada TLC5940 elaboration does not raise
// SPI_Error. SPI_Transfer zeroes the receive buffer and returns 0.

#include <string.h>

int SPI_Open (unsigned char SPI_Device, unsigned char Mode,
              unsigned long Speed)
{
    (void)SPI_Device;
    (void)Mode;
    (void)Speed;
    return 0; // success
}

int SPI_Transfer (unsigned char *Tx_Buffer, unsigned char *Rx_Buffer,
                  unsigned short Length)
{
    (void)Tx_Buffer;
    if (Rx_Buffer != 0 && Length > 0)
        memset(Rx_Buffer, 0, Length);
    return 0; // success
}

int SPI_Close (void)
{
    return 0;
}
