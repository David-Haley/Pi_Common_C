// Simple GPIO driver for RPI intended to be callable from Ada. The
// driver uses libgpiod.
// Author    : David Haley
// Created   : 22/07/2025
// Last Edit : 31/07/2025

int Open_GPIO_Chip (const char *Chip_Name);
// Must be called before any other GPIO functions are used.
// Returns 0 if no error, otherwise the errno set by the library call.

void Close_GPIO_Line (int Line_Number);
// Close one line only. Can be used to close a line whilst retaining
// others in use. Close_GPIO_Chip closes all lines currently in use.

void Close_GPIO_Chip (void);
// Should be called when the use of GPIO is no longer required. Closes
// all lines currently in use.

int Open_Output (const char *Line_Name);
// Opens the named line as an output and returns the line number or -1
// if the call fails.

int Set_High (int Line_Number);
// Set the specified line high, returns -1 on failure.

int Set_Low (int Line_Number);
// Set the specified line low, returns -1 on failure.

int Pulse_High (int Line_Number);
// Sets a previously low output high for the minimum time possible,
// time will depend on Pi speed and overhead. if output was initially
// high it will be set low, this is not treated as an error.
// Returns error number if a failure occurs.

int Pulse_Low (int Line_Number);
// Sets a previously high output low for the minimum time possible,
// time will depend on Pi speed and overhead. if output was initially
// low it will be set high, this is not treated as an error.
// Returns error number if a failure occurs.

int Open_Input (const char *Line_Name);
// Opens the named line as an input and returns the line number or -1
// if the call fails.

int Read_Input (int Line_Number);
// Reads one line returns 1 for high 0 for low and -1 on failure.
