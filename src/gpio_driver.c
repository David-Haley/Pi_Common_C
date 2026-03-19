// Simple GPIO driver for RPI intended to be callable from Ada. The
// driver uses libgpiod version 2.
// Code based on https://github.com/jdfin/libgpiod_examples/.
// Author    : David Haley
// Created   : 22/07/2025
// Last Edit : 23/02/2025

// 20260219 : Ported to libgpiod version 2

# include <unistd.h>
# include <stdio.h>
# include <gpiod.h>
# include "gpio_driver.h"

# define RPi_Lines 54
static const char *Consumer_Name = "gpio_driver.c";
static struct gpiod_chip *GPIO_Chip = NULL;
static struct gpiod_line_request *Line_Array [RPi_Lines];
static const enum gpiod_line_value Output_Low = GPIOD_LINE_VALUE_INACTIVE;
static const enum gpiod_line_value Output_High = GPIOD_LINE_VALUE_ACTIVE;

int Open_GPIO_Chip (const char *Chip_Name)
// Must be called before any other GPIO functions are used.
// Returns 0 if no error, otherwise the errno set by the library call.

{
	GPIO_Chip = gpiod_chip_open (Chip_Name);
	if (GPIO_Chip != NULL)
	{
		for (int I = 1; I < RPi_Lines; I++)
		{
			Line_Array [I] = NULL;
		} // for (int I = 1; I < RPi_Lines; I++)
		return 0;
	}
	else
	{
		perror ("gpiod_chip_open failed");
		return -1;
	} // if (GPIO_Chip != NULL)
} // Open_GPIO_Chip

void Close_GPIO_Line (int Line_Number)
// Close one line only. Can be used to close a line whilst retaining
// others in use. Close_GPIO_Chip closes all lines currently in use.

{
	// Only attempt to close a line that is in use.
	if (Line_Array [Line_Number] != NULL)
	{
		gpiod_line_request_release (Line_Array [Line_Number]);
		Line_Array [Line_Number] = NULL;
    } // if (Line_Array [Line_Number] != NULL)
} // Close_GPIO_Line

void Close_GPIO_Chip (void)
// Should be called when the use of GPIO is no longer required. Closes
// all lines currently in use.

{
	unsigned int Line_Index;
	// Release all used lines
	for (Line_Index = 0; Line_Index < RPi_Lines; Line_Index++)
		if (Line_Array [Line_Index] != NULL)
		{
			gpiod_line_request_release (Line_Array [Line_Index]);
		    Line_Array [Line_Index] = NULL;
		} // if (Line_Array [Line_Index] != NULL)
	gpiod_chip_close (GPIO_Chip); // Release Chip
} // Close_GPIO_Chip

int Open_Output (const char *Line_Name)
// Opens the named line as an output and returns the line number or -1
// if the call fails.

{
	int Return_Code;
	struct gpiod_line_config *Line_Config = gpiod_line_config_new ();
	if (Line_Config == NULL)
	{ 
		perror ("Line_Config structure not created");
		return -1;
	} // (Line_Config == NULL)
	struct gpiod_line_settings *Settings = gpiod_line_settings_new();
	if (Settings == NULL)
	{ 
		perror ("Settings structure not created");
		return -1;
	} // (Settings == NULL)
	Return_Code =
	  gpiod_line_settings_set_direction (Settings,
		                                 GPIOD_LINE_DIRECTION_OUTPUT);
	if (Return_Code != 0)
	{
		perror ("Setting as output failed");
		return -1;
	} // if (Return_Code != 0)
	Return_Code =
      gpiod_line_settings_set_drive (Settings, GPIOD_LINE_DRIVE_PUSH_PULL);
	if (Return_Code != 0)
	{
		perror ("Setting output as push pull failed");
		return -1;
	} // if (Return_Code != 0)
	unsigned int Line_Number;
	Line_Number = gpiod_chip_get_line_offset_from_name (GPIO_Chip, Line_Name);
	if (Line_Number == -1)
	{
		perror ("Failed to obtain LIne_Number");
		return -1;
	} // if (Line_Number == -1)
	Return_Code =
	  gpiod_line_config_add_line_settings (Line_Config, &Line_Number, 1, Settings);
	if (Return_Code != 0)
	{ 
		perror ("Failed to attach Settings to Line_Config");
		return -1;
	} // (Return_Code != 0)
	gpiod_line_settings_free (Settings);
	// Settings are no longer needed, can be freed
    Return_Code = gpiod_line_config_set_output_values (Line_Config, &Output_Low, 1);
	if (Return_Code != 0)
	{ 
		perror ("Failed to attach Initial_Value to Line_Config");
		return -1;
	} // (Return_Code != 0)
	struct gpiod_request_config *Request_Config = gpiod_request_config_new ();
	if (Request_Config == NULL)
	{ 
		perror ("Request_Config structure not created");
		return -1;
	} // (Line_Config == NULL)
    gpiod_request_config_set_consumer (Request_Config, Consumer_Name);
    // Cannot fail with a valid Request_Config
    Line_Array [Line_Number] =
	  gpiod_chip_request_lines (GPIO_Chip, Request_Config, Line_Config);
	if (Line_Array [Line_Number] == NULL)
	{ 
		perror ("Creation of request in Line_Array failed");
		return -1;
	}
	else
	{
		gpiod_request_config_free (Request_Config);
	    gpiod_line_config_free (Line_Config);
	   	// Request_Config and Line_Config no longer needed, can be freed
		return Line_Number;
	} // if (Line_Array [Line_Number] == NULL)
} // Open_Output


int Set_High (int Line_Number)
// Set the specified line high, returns -1 on failure.

{
	if (gpiod_line_request_set_value (Line_Array [Line_Number], Line_Number,
		                              Output_High))
	{
		perror ("Set_High failed");
		return -1;
	} // gpiod_line_request_set_value (Line_Array [Line_Number] ...
	else
		return 0;
} // Set_High

int Set_Low (int Line_Number)
// Set the specified line low, returns -1 on failure.

{
	if (gpiod_line_request_set_value (Line_Array [Line_Number], Line_Number,
		                              Output_Low))
	{
		perror ("Set_Low failed");
		return -1;
	} // gpiod_line_request_set_value (Line_Array [Line_Number] ...
	else
		return 0;
} // Set_Low

int Pulse_High (int Line_Number)
// Sets a previously low output, high for the minimum time possible,
// time will depend on Pi speed and overhead. if output was initially
// high it will be set low, this is not treated as an error.
// Returns -1 if a failure occurs.

{
	if (gpiod_line_request_set_value (Line_Array [Line_Number], Line_Number,
		                              Output_High) ||
	    gpiod_line_request_set_value (Line_Array [Line_Number], Line_Number,
		                              Output_Low))
	{
		perror ("Pulse_High failed");
		return -1;
	} // if (gpiod_line_request_set_value (Line_Array [Line_Number], ...
	else
		return 0;
} // Pulse_High

int Pulse_Low (int Line_Number)
// Sets a previously high output low for the minimum time possible,
// time will depend on Pi speed and overhead. if output was initially
// low it will be set high, this is not treated as an error.
// Returns -1 if a failure occurs.

{
	if (gpiod_line_request_set_value (Line_Array [Line_Number], Line_Number,
		                              Output_Low) ||
	    gpiod_line_request_set_value (Line_Array [Line_Number], Line_Number,
		                              Output_High))
	{
		perror ("Pulse_Low failed");
		return -1;
	} // if (gpiod_line_request_set_value (Line_Array [Line_Number] ...
	else
		return 0;
} // Pulse_Low

int Open_Input (const char *Line_Name)
// Opens the named line as an input and returns the line number or -1
// if the call fails.

{
	unsigned int Return_Code;
	struct gpiod_line_config *Line_Config = gpiod_line_config_new ();
	if (Line_Config == NULL)
	{ 
		perror ("Line_Config structure not created");
		return -1;
	} // (Line_Config == NULL)
	struct gpiod_line_settings *Settings = gpiod_line_settings_new();
	if (Settings == NULL)
	{ 
		perror ("Settings structure not created");
		return -1;
	} // (Settings == NULL)
	Return_Code =
	  gpiod_line_settings_set_direction (Settings,
		                                 GPIOD_LINE_DIRECTION_INPUT);
	if (Return_Code != 0)
	{
		perror ("Setting as input failed");
		return -1;
	} // if (Return_Code != 0)
	Return_Code =
      gpiod_line_settings_set_edge_detection (Settings, GPIOD_LINE_EDGE_NONE);
	if (Return_Code != 0)
	{
		perror ("Setting input edge failed");
		return -1;
	} // if (Return_Code != 0)
	Return_Code =
      gpiod_line_settings_set_bias (Settings, GPIOD_LINE_BIAS_DISABLED);
	if (Return_Code != 0)
	{
		perror ("Setting input bias failed");
		return -1;
	} // if (Return_Code != 0)
      gpiod_line_settings_set_debounce_period_us (Settings, 0);
	unsigned int Line_Number;
	Line_Number = gpiod_chip_get_line_offset_from_name (GPIO_Chip, Line_Name);
	if (Line_Number == -1)
	{
		perror ("Failed to obtain LIne_Number");
		return -1;
	} // if (Line_Number == -1)
	Return_Code =
	  gpiod_line_config_add_line_settings (Line_Config, &Line_Number, 1, Settings);
	if (Return_Code != 0)
	{ 
		perror ("Failed to attach Settings to Line_Config");
		return -1;
	} // (Return_Code != 0)
	gpiod_line_settings_free (Settings);
	// Settings are no longer needed, can be freed
	struct gpiod_request_config *Request_Config = gpiod_request_config_new ();
	if (Request_Config == NULL)
	{ 
		perror ("Request_Config structure not created");
		return -1;
	} // (Line_Config == NULL)
    gpiod_request_config_set_consumer (Request_Config, Consumer_Name);
    // Cannot fail with a valid Request_Config
    Line_Array [Line_Number] =
	  gpiod_chip_request_lines (GPIO_Chip, Request_Config, Line_Config);
	if (Line_Array [Line_Number] == NULL)
	{ 
		perror ("Creation of request in Line_Array failed");
		return -1;
	}
	else
	{
		gpiod_request_config_free (Request_Config);
	    gpiod_line_config_free (Line_Config);
	   	// Request_Config and Line_Config no longer needed, can be freed
		return Line_Number;
	} // if (Line_Array [Line_Number] == NULL)
} // Open_Input

int Read_Input (int Line_Number)
// Reads one line returns 1 for high 0 for low and -1 on failure.

{
	return gpiod_line_request_get_value (Line_Array [Line_Number],
		                                 Line_Number);
	// The comment in API is as above, it is an enum which could have
	// potentially have other return values in the future. Consider a
	// switch statement with the default returning -1 for future
	// proofing.
} // Read_Input
