// Simple GPIO driver for RPI intended to be callable from Ada. The
// driver uses libgpiod version 2.
// Code based on https://github.com/jdfin/libgpiod_examples/.
// Author    : David Haley
// Created   : 22/07/2025
// Last Edit : 14/10/2026

// 20261014 : Line numbers range checked, resources freed on all error paths,
// errors returned as -errno rather than printed, input bias left as is and
// Open_Output_Initial added.
// 20260219 : Ported to libgpiod version 2

# include <errno.h>
# include <gpiod.h>
# include "gpio_driver.h"

# define RPi_Lines 54
static const char *Consumer_Name = "gpio_driver.c";
static struct gpiod_chip *GPIO_Chip = NULL;
static struct gpiod_line_request *Line_Array [RPi_Lines];

static int Error_Code (void)
// Returns -errno, or -EIO if a library call failed without setting errno.

{
	return errno != 0 ? -errno : -EIO;
} // Error_Code

static int Line_Open (int Line_Number)
// True if Line_Number is in range and the line has been opened.

{
	return Line_Number >= 0 && Line_Number < RPi_Lines &&
	  Line_Array [Line_Number] != NULL;
} // Line_Open

static int Set_Line (int Line_Number, enum gpiod_line_value Value)
// Sets an open output line to Value, returns 0 or -errno on failure.

{
	if (!Line_Open (Line_Number))
		return -EINVAL;
	errno = 0;
	if (gpiod_line_request_set_value (Line_Array [Line_Number],
	                                  (unsigned int) Line_Number, Value))
		return Error_Code ();
	return 0;
} // Set_Line

static int Request_Line (const char *Line_Name,
                         struct gpiod_line_settings *Settings)
// Requests the named line with the given Settings and records the request
// in Line_Array. Returns the line number or -errno on failure. Settings
// remains the responsibility of the caller.

{
	int Line_Number;
	int Result;
	unsigned int Offset;
	struct gpiod_line_config *Line_Config = NULL;
	struct gpiod_request_config *Request_Config = NULL;
	struct gpiod_line_request *Request;

	if (GPIO_Chip == NULL)
		return -ENODEV;
	errno = 0;
	Line_Number = gpiod_chip_get_line_offset_from_name (GPIO_Chip, Line_Name);
	if (Line_Number < 0)
		return Error_Code ();
	if (Line_Number >= RPi_Lines)
		return -ERANGE;
	if (Line_Array [Line_Number] != NULL)
		return -EBUSY;
	Line_Config = gpiod_line_config_new ();
	Request_Config = gpiod_request_config_new ();
	if (Line_Config == NULL || Request_Config == NULL)
	{
		Result = -ENOMEM;
		goto Cleanup;
	} // if (Line_Config == NULL || Request_Config == NULL)
	Offset = (unsigned int) Line_Number;
	errno = 0;
	if (gpiod_line_config_add_line_settings (Line_Config, &Offset, 1,
	                                         Settings))
	{
		Result = Error_Code ();
		goto Cleanup;
	} // if (gpiod_line_config_add_line_settings (...
	gpiod_request_config_set_consumer (Request_Config, Consumer_Name);
	// Cannot fail with a valid Request_Config
	errno = 0;
	Request = gpiod_chip_request_lines (GPIO_Chip, Request_Config,
	                                    Line_Config);
	if (Request == NULL)
	{
		Result = Error_Code ();
		goto Cleanup;
	} // if (Request == NULL)
	Line_Array [Line_Number] = Request;
	Result = Line_Number;
Cleanup:
	// Request_Config and Line_Config are not needed after the request
	if (Request_Config != NULL)
		gpiod_request_config_free (Request_Config);
	if (Line_Config != NULL)
		gpiod_line_config_free (Line_Config);
	return Result;
} // Request_Line

int Open_GPIO_Chip (const char *Chip_Name)
// Must be called before any other GPIO functions are used.
// Returns 0 if no error, otherwise -errno.

{
	if (GPIO_Chip != NULL)
		return -EBUSY;
	errno = 0;
	GPIO_Chip = gpiod_chip_open (Chip_Name);
	if (GPIO_Chip == NULL)
		return Error_Code ();
	for (int I = 0; I < RPi_Lines; I++)
		Line_Array [I] = NULL;
	return 0;
} // Open_GPIO_Chip

void Close_GPIO_Line (int Line_Number)
// Close one line only. Can be used to close a line whilst retaining
// others in use. Close_GPIO_Chip closes all lines currently in use.

{
	// Only attempt to close a line that is in use.
	if (Line_Open (Line_Number))
	{
		gpiod_line_request_release (Line_Array [Line_Number]);
		Line_Array [Line_Number] = NULL;
	} // if (Line_Open (Line_Number))
} // Close_GPIO_Line

void Close_GPIO_Chip (void)
// Should be called when the use of GPIO is no longer required. Closes
// all lines currently in use.

{
	for (int I = 0; I < RPi_Lines; I++)
		Close_GPIO_Line (I);
	if (GPIO_Chip != NULL)
	{
		gpiod_chip_close (GPIO_Chip);
		GPIO_Chip = NULL;
	} // if (GPIO_Chip != NULL)
} // Close_GPIO_Chip

int Open_Output_Initial (const char *Line_Name, int Initial_High)
// Opens the named line as an output, initially high if Initial_High is
// non zero otherwise low. Returns the line number or -errno if the call
// fails.

{
	int Result;
	struct gpiod_line_settings *Settings = gpiod_line_settings_new ();

	if (Settings == NULL)
		return -ENOMEM;
	errno = 0;
	if (gpiod_line_settings_set_direction (Settings,
	                                       GPIOD_LINE_DIRECTION_OUTPUT) ||
	    gpiod_line_settings_set_drive (Settings, GPIOD_LINE_DRIVE_PUSH_PULL) ||
	    gpiod_line_settings_set_output_value (Settings, Initial_High ?
	                                          GPIOD_LINE_VALUE_ACTIVE :
	                                          GPIOD_LINE_VALUE_INACTIVE))
		Result = Error_Code ();
	else
		Result = Request_Line (Line_Name, Settings);
	gpiod_line_settings_free (Settings);
	return Result;
} // Open_Output_Initial

int Open_Output (const char *Line_Name)
// Opens the named line as an output, initially low, and returns the line
// number or -errno if the call fails.

{
	return Open_Output_Initial (Line_Name, 0);
} // Open_Output

int Set_High (int Line_Number)
// Set the specified line high, returns 0 on success or -errno on failure.

{
	return Set_Line (Line_Number, GPIOD_LINE_VALUE_ACTIVE);
} // Set_High

int Set_Low (int Line_Number)
// Set the specified line low, returns 0 on success or -errno on failure.

{
	return Set_Line (Line_Number, GPIOD_LINE_VALUE_INACTIVE);
} // Set_Low

int Pulse_High (int Line_Number)
// Sets a previously low output, high for the minimum time possible,
// time will depend on Pi speed and overhead. if output was initially
// high it will be set low, this is not treated as an error.
// Returns 0 on success or -errno on failure.

{
	int Result = Set_Line (Line_Number, GPIOD_LINE_VALUE_ACTIVE);

	if (Result != 0)
		return Result;
	return Set_Line (Line_Number, GPIOD_LINE_VALUE_INACTIVE);
} // Pulse_High

int Pulse_Low (int Line_Number)
// Sets a previously high output low for the minimum time possible,
// time will depend on Pi speed and overhead. if output was initially
// low it will be set high, this is not treated as an error.
// Returns 0 on success or -errno on failure.

{
	int Result = Set_Line (Line_Number, GPIOD_LINE_VALUE_INACTIVE);

	if (Result != 0)
		return Result;
	return Set_Line (Line_Number, GPIOD_LINE_VALUE_ACTIVE);
} // Pulse_Low

int Open_Input (const char *Line_Name)
// Opens the named line as an input and returns the line number or -errno
// if the call fails. Bias (pull up / down) is left as configured.

{
	int Result;
	struct gpiod_line_settings *Settings = gpiod_line_settings_new ();

	if (Settings == NULL)
		return -ENOMEM;
	errno = 0;
	if (gpiod_line_settings_set_direction (Settings,
	                                       GPIOD_LINE_DIRECTION_INPUT) ||
	    gpiod_line_settings_set_edge_detection (Settings,
	                                            GPIOD_LINE_EDGE_NONE) ||
	    gpiod_line_settings_set_bias (Settings, GPIOD_LINE_BIAS_AS_IS))
		Result = Error_Code ();
	else
	{
		gpiod_line_settings_set_debounce_period_us (Settings, 0);
		Result = Request_Line (Line_Name, Settings);
	} // if (gpiod_line_settings_set_direction (...
	gpiod_line_settings_free (Settings);
	return Result;
} // Open_Input

int Read_Input (int Line_Number)
// Reads one line, input or output, returns 1 for high 0 for low and -errno
// on failure.

{
	enum gpiod_line_value Value;

	if (!Line_Open (Line_Number))
		return -EINVAL;
	errno = 0;
	Value = gpiod_line_request_get_value (Line_Array [Line_Number],
	                                      (unsigned int) Line_Number);
	switch (Value)
	{
		case GPIOD_LINE_VALUE_ACTIVE:
			return 1;
		case GPIOD_LINE_VALUE_INACTIVE:
			return 0;
		default:
			return Error_Code ();
	} // switch (Value)
} // Read_Input
