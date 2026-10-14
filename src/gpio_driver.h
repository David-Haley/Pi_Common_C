// Simple GPIO driver for RPI intended to be callable from Ada. The
// driver uses libgpiod.
// Author    : David Haley
// Created   : 22/07/2025
// Last Edit : 14/10/2026

// 20261014 : Errors returned as -errno, Open_Output_Initial added.
// 20260217 : Ported to libgpiod version 2

#ifndef GPIO_DRIVER_H
#define GPIO_DRIVER_H

int Open_GPIO_Chip (const char *Chip_Name);
// Must be called before any other GPIO functions are used.
// Returns 0 if no error, otherwise -errno.

void Close_GPIO_Line (int Line_Number);
// Close one line only. Can be used to close a line whilst retaining
// others in use. Closing a line that is not open has no effect.

void Close_GPIO_Chip (void);
// Should be called when the use of GPIO is no longer required. Closes
// all lines currently in use.

int Open_Output (const char *Line_Name);
// Opens the named line as an output, initially low, and returns the line
// number or -errno if the call fails.

int Open_Output_Initial (const char *Line_Name, int Initial_High);
// Opens the named line as an output, initially high if Initial_High is
// non zero otherwise low. Returns the line number or -errno if the call
// fails.

int Set_High (int Line_Number);
// Set the specified line high, returns 0 on success or -errno on failure.

int Set_Low (int Line_Number);
// Set the specified line low, returns 0 on success or -errno on failure.

int Pulse_High (int Line_Number);
// Sets a previously low output high for the minimum time possible,
// time will depend on Pi speed and overhead. if output was initially
// high it will be set low, this is not treated as an error.
// Returns 0 on success or -errno on failure.

int Pulse_Low (int Line_Number);
// Sets a previously high output low for the minimum time possible,
// time will depend on Pi speed and overhead. if output was initially
// low it will be set high, this is not treated as an error.
// Returns 0 on success or -errno on failure.

int Open_Input (const char *Line_Name);
// Opens the named line as an input and returns the line number or -errno
// if the call fails. Bias (pull up / down) is left as configured.

int Read_Input (int Line_Number);
// Reads one line, input or output, returns 1 for high 0 for low and -errno
// on failure.

#endif // GPIO_DRIVER_H
