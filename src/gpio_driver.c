// Simple GPIO driver for RPI intended to be callable from Ada. The
// driver uses libgpiod.
// Author    : David Haley
// Created   : 22/07/2025
// Last Edit : 01/08/2025

#include <unistd.h>
#include <stdio.h>
# include <gpiod.h>
# include <errno.h>
# include "gpio_driver.h"

# define RPi_Lines 54
static const char *Consumer_Name = "gpio_driver.c";
static struct gpiod_chip *GPIO_Chip = NULL;
static struct gpiod_line *Line_Array [RPi_Lines];

int Open_GPIO_Chip (const char *Chip_Name)
// Must be called before any other GPIO functions are used.
// Returns 0 if no error, otherwise the errno set by the library call.

{
	GPIO_Chip = gpiod_chip_open_by_name (Chip_Name);
	if (GPIO_Chip != NULL)
		return 0;
	else
	{
		perror ("gpiod_chip_open_by_name failed");
		return errno;
	} //if (GPIO_Chip != NULL)
} // Open_GPIO_Chip

void Close_GPIO_Line (int Line_Number)
// Close one line only. Can be used to close a line whilst retaining
// others in use. Close_GPIO_Chip closes all lines currently in use.

{
	// Only attempt to close a line that is in use.
	if (Line_Array [Line_Number] != NULL)
		gpiod_line_release (Line_Array [Line_Number]);
} // Close_GPIO_Line

void Close_GPIO_Chip (void)
// Should be called when the use of GPIO is no longer required. Closes
// all lines currently in use.

{
	unsigned int Line_Index;
	// Release all used lines
	for (Line_Index = 0; Line_Index < RPi_Lines; Line_Index++)
		if (Line_Array [Line_Index] != NULL)
			gpiod_line_release (Line_Array [Line_Index]);
	gpiod_chip_close (GPIO_Chip); // Release Chip
} // Close_GPIO_Chip

int Open_Output (const char *Line_Name)
// Opens the named line as an output and returns the line number or -1
// if the call fails.

{
	struct gpiod_line *Current_Line;
	int Line_Number;
	Current_Line = gpiod_chip_find_line (GPIO_Chip, Line_Name);
	if (Current_Line == NULL)
	{ 
		perror ("Line name not found");
		return -1;
	}
	else
	{
		Line_Number = gpiod_line_offset (Current_Line);
		// Cannot fail, must exist because valid pointer was returned,
		// no error return stated in gpiod.h
		if (gpiod_line_request_output (Current_Line, Consumer_Name, 0))
			// 0 parameter output initially low
		{
			perror ("Request line as output failed");
			return -1;
		}
		else
		{
			Line_Array [Line_Number] = Current_Line;
			return Line_Number;
		} // if (gpiod_line_request_output (Current_Line, ...
	} // if (Current_Line == NULL)
} // Open_Output


int Set_High (int Line_Number)
// Set the specified line high, returns -1 on failure.

{
	if (gpiod_line_set_value (Line_Array [Line_Number], 1))
	{
		perror ("Set_High_Failed failed");
		return errno;
	} // if (gpiod_line_set_value (Line_Array [Line_Number], 1))
	else
		return 0;
} // Set_High

int Set_Low (int Line_Number)
// Set the specified line low, returns -1 on failure.

{
	if (gpiod_line_set_value (Line_Array [Line_Number], 0))
	{
		perror ("Set_High_Failed failed");
		return errno;
	} // if (gpiod_line_set_value (Line_Array [Line_Number], 1))
	else
		return 0;
} // Set_Low

int Pulse_High (int Line_Number)
// Sets a previously low output high for the minimum time possible,
// time will depend on Pi speed and overhead. if output was initially
// high it will be set low, this is not treated as an error.
// Returns error number if a failure occurs.

{
	if (gpiod_line_set_value (Line_Array [Line_Number], 1) ||
	  gpiod_line_set_value (Line_Array [Line_Number], 0))
	{
		perror ("Pulse_High failed");
		return errno;
	} //if (gpiod_line_set_value (Line_Array [Line_Number], 1) || ...
	else
		return 0;
} // Pulse_High

int Pulse_Low (int Line_Number)
// Sets a previously high output low for the minimum time possible,
// time will depend on Pi speed and overhead. if output was initially
// low it will be set high, this is not treated as an error.
// Returns error number if a failure occurs.

{
	if (gpiod_line_set_value (Line_Array [Line_Number], 0) ||
	  gpiod_line_set_value (Line_Array [Line_Number], 1))
	{
		perror ("Pulse_Low failed");
		return errno;
	} //if (gpiod_line_set_value (Line_Array [Line_Number], 0) || ...
	else
		return 0;
} // Pulse_Low

int Open_Input (const char *Line_Name)
// Opens the named line as an input and returns the line number or -1
// if the call fails.

{
	struct gpiod_line *Current_Line;
	int Line_Number;
	Current_Line = gpiod_chip_find_line (GPIO_Chip, Line_Name);
	if (Current_Line == NULL)
	{ 
		perror ("Line name not found");
		return -1;
	}
	else
	{
		Line_Number = gpiod_line_offset (Current_Line);
		// Cannot fail, must exist because valid pointer was returned,
		// no error return stated in gpiod.h
		if (gpiod_line_request_input (Current_Line, Consumer_Name))
			// 0 parameter output initially low
		{
			perror ("Request line as input failed");
			return -1;
		}
		else
		{
			Line_Array [Line_Number] = Current_Line;
			return Line_Number;
		} // if (gpiod_line_request_output (Current_Line, ...
	} // if (Current_Line == NULL)
} // Open_Input

int Read_Input (int Line_Number)
// Reads one line returns 1 for high 0 for low and -1 on failure.

{
	return gpiod_line_get_value (Line_Array [Line_Number]);
} // Read_Input
