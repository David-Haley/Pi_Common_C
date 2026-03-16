// No-op I2C stub for simulation builds.
// Implements the same symbols as i2c_driver.c but without
// <linux/i2c-dev.h> or any hardware device access.

#include <string.h>

int I2C_Open (unsigned char I2C_Device)
{
    (void)I2C_Device;
    return 0; // success
}

int Set_IC_Address (int IC_Address)
{
    (void)IC_Address;
    return 0;
}

int I2C_Write (unsigned char *Tx_Buffer, unsigned short Length)
{
    (void)Tx_Buffer;
    return (int)Length; // report all bytes written
}

int I2C_Read (unsigned char *Rx_Buffer, unsigned short Length)
{
    if (Rx_Buffer != 0 && Length > 0)
        memset(Rx_Buffer, 0, Length);
    return (int)Length; // report all bytes read
}

int I2C_Close (void)
{
    return 0;
}
